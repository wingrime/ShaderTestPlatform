#include "ObjModel.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include "OGL.h"

/* shader class*/
#include "Shader.h"
#include "r_shader.h"
#include "mat_math.h"
#include "RBO.h"
#include "Texture.h"
#include "ProjectionMatrix.h"
#include "RenderContext.h"
#include "MAssert.h"

#include "string_format.h"
#include "Log.h"
#include "ObjParser.h"
#include "ErrorCodes.h"


#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include "IndexedMesh.h"

int SObjModel::ConfigureProgram(SShader& sprog){
    if (!IsReady)
        return EFAIL;
    sprog.SetUniform("matrixModel",model);

    sprog.SetUniform("samplerAlbedo",0);
    sprog.SetUniform("samplerNormalMap",1);
    sprog.SetUniform("samplerAlphaMap",2);

    sprog.SetUniform("samplerShadowMap",5);
    sprog.SetUniform("samplerTex1",6);
    sprog.SetUniform("samplerEnvCubeMap",7);
    sprog.SetUniform("samplerTex2",8);
    sprog.SetUniform("samplerEnvSH",9);
    sprog.SetUniform("samplerRandomNoise",10);
    sprog.Bind();

    return 0;
}

SObjModel::SObjModel(const std::string&  fname)
{
    LOGV("Opening obj model %s" ,fname.c_str());
    CObjMeshParser parser;
    ObjCtx * ctx = parser.ParseFromFile(fname);

    if (!ctx)
        {
            LOGE("Unable open model");
            return;
        }

    auto submesh_set = ctx->subMeshSet;
    long total_triangles = 0;
    for (auto it = submesh_set.begin(); it != submesh_set.end();++it) {
        MeshIndexer idx;
        auto res = idx.IndexNonIndexedMesh(*it);
        (*it)->vn.clear();
        total_triangles += res->vn.size();
        d_sm.push_back(res);
   }
    std::vector<std::string> mtlrefs = ctx->refMaterialFiles;
    CObjMeshParser::ReleaseContext(ctx);
    LOGV("Mesh information: Submesh count:%d, Total mesh triangles:%d", d_sm.size(), total_triangles );

    LOGV("Load materials");

    //std::string s = mtlrefs[0];
    //printf("RefMaterial:%s\n",s.c_str());

    if (!mtlrefs.empty())
    {
        std::string json_fname = std::string(mtlrefs[0]+std::string(".json"));

        //if (CheckFileExists(json_fname)) {
        if (false) {
        /* Load */
            std::ifstream ifs(json_fname);
            {
                cereal::JSONInputArchive arch(ifs);
                arch(d_materials);
            }

        } else {
            MTLParser mtl_p(mtlrefs[0]);
            d_materials = mtl_p.GetMaterials(); //OMG copy!! FIX ME
            mtl_p.SaveToJSON(json_fname);

        }
    }

    LoadTextures();
    BindVAOs();

    LOGV("Model configured");
    IsReady = true;

}
void SObjModel::SetModelMat(const SMat4x4& m){
	model = m;
}
void SObjModel::BindTextures(SMaterial &m) {
    if (m.alphaMaskTex && m.alphaMaskTex->IsReady)
        m.alphaMaskTex->Bind(2);
    else
        texDiffuse->Bind(2);
    if (m.albedoTex && m.albedoTex->IsReady)
        m.albedoTex->Bind(0);
    else
        texDiffuse->Bind(0);
    if (m.bumpMapTex && m.bumpMapTex->IsReady)
        m.bumpMapTex->Bind(1);
    else
        texNormal->Bind(1);
}

void SObjModel::LoadTextures() {

    texDiffuse = (new STexture("AssetBase/empty_texture.png"));
    if (!texDiffuse->IsReady) {
        LOGE(" diffuse texture file not found");
        return;
    }
    texNormal = (new STexture("AssetBase/empty_normal.png",false));
    if (!texNormal->IsReady) {
       LOGE("normal texture file not found");
        return;
    }

    for (auto it = d_sm.begin(); it != d_sm.end();++it) {

        auto &submesh =  (*it);
        if (d_materials.find(submesh->m_name) == d_materials.end()) {
           LOGE("no material found - \"%s\" ",submesh->m_name.c_str());
        } else {

            auto &material = d_materials[submesh->m_name];


            std::string &diffuse = material.albedoTexFileName;
            if (d_textures.find(diffuse) == d_textures.end()) {
                LOGV("material %s Diffuse %s Bump %s Alpha %s",submesh->m_name.c_str(),
                                   material.albedoTexFileName.c_str(),
                                   material.bumpMapTexFileName.c_str(),
                                   material.alphaMaskTexFileName.c_str());

                d_materials[submesh->m_name].albedoTex =  new STexture(submesh->m_dir+diffuse);
                d_textures[diffuse].reset( d_materials[submesh->m_name].albedoTex);
                if (!d_textures[diffuse]->IsReady) {
                   LOGE("OBJ:Diffuse texture load failed %s",(submesh->m_dir+diffuse).c_str());
                }
            }

            std::string &bump = material.bumpMapTexFileName;
            if (d_textures.find(bump) == d_textures.end()) {
                d_materials[submesh->m_name].bumpMapTex =  new STexture(submesh->m_dir+bump,false);
                d_textures[bump].reset(d_materials[submesh->m_name].bumpMapTex);
                if (!d_textures[bump]->IsReady) {
                  LOGE("OBJ:Bump texture load failed %s",(submesh->m_dir+bump).c_str());
                }
            }
            std::string &alpha = material.alphaMaskTexFileName;
            if (d_textures.find(alpha) == d_textures.end()) {
                d_materials[submesh->m_name].alphaMaskTex = new STexture(submesh->m_dir+alpha);
                d_textures[alpha].reset(d_materials[submesh->m_name].alphaMaskTex);
                if (!d_textures[alpha]->IsReady) {
                   LOGE("OBJ:Alpha mask texture load failed %s",(submesh->m_dir+alpha).c_str());
                }
            }



        }


    }
}

void SObjModel::BindVAOs() {
    /* load descriptor*/
    //unsigned int temp_vao;
    unsigned int temp_vbo;
    unsigned int temp_ibo;
    /*
        std::string map_Ka; ambient map
        std::string map_Kd; diffuse map
        std::string map_Ns; Specular shinnes map
        std::string map_d; alpha map
        std::string map_bump;
    */

    /*https://devtalk.nvidia.com/default/topic/561172/gldrawarrays-without-vao-for-procedural-geometry-using-gl_vertexid-doesn-t-work-in-debug-context/ */
    glGenVertexArrays ( 1, &d_emptyVAO );
   // glBindVertexArray(d_emptyVAO);
        for (auto it = d_sm.begin(); it != d_sm.end();++it) {
            auto &submesh =  (*it);
            /*gen buffers for submesh*/
            /*vertixes*/
            temp_vbo = 0;
            temp_ibo = 0;
            glGenBuffers ( 1, &temp_vbo );
            glGenBuffers(1, &temp_ibo);

            //MASSERT(submesh->vn.empty());
           // MASSERT(submesh->indexes.empty());
            //gl 4.5 without EXT
            #ifndef __APPLE__
            glNamedBufferDataEXT(temp_vbo,submesh->vn.size() *sizeof(UVNVertex), submesh->vn.data(), GL_STATIC_DRAW);
            glNamedBufferDataEXT(temp_ibo,submesh->indexes.size() *sizeof(unsigned int), submesh->indexes.data(), GL_STATIC_DRAW);
            SubMeshIDs idx;
            idx.vbo = temp_vbo;
            idx.ibo = temp_ibo;

            submesh_idx[submesh->id] = idx;
            #else
            //todo
            #endif
            glBindBuffer(GL_ARRAY_BUFFER,0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        }
        //glBindVertexArray(0);
}
void SObjModel::Render(RenderContext &r) {
    glBindVertexArray(d_emptyVAO);

    /*activate shader and load model matrix*/
    if (r.shader->IsReady) {
        r.shader->Bind();//Commit changes
        SProg *s  = r.shader->getDirect();
        s->Bind();
        /*request shader locations*/
        int locPositions = s->LookupAttribLocation("position");
        int locNormals = s->LookupAttribLocation("normal");
        int locUV = s->LookupAttribLocation("UV");
        /*ARB_vertex_attrib_binding*/
     /*   if (locPositions != EFAIL) {
        glVertexAttribFormat ( locPositions, 3 ,GL_FLOAT,  GL_FALSE,   offsetof(CObjVertexN,p) );
        glVertexAttribBinding(0, 0);
        }
        if (locNormals != EFAIL) {
        glVertexAttribFormat ( locNormals, 3 ,GL_FLOAT,  GL_FALSE,   offsetof(CObjVertexN,n) );
        glVertexAttribBinding(1, 0);
        }
        if (locUV != EFAIL) {
        glVertexAttribFormat ( locUV, 2 ,GL_FLOAT,  GL_FALSE, offsetof(CObjVertexN,tc) );
        glVertexAttribBinding(2, 0);
        }
    */
        s->SetUniform(r.d_modelMatrixLoc,model);
        s->SetUniform(r.d_viewMatrixLoc,r.d_V);
        s->SetUniform(r.d_projMatrixLoc,r.d_P);
        /*optimize*/
         s->SetUniform(r.d_uniformMVP,r.d_P*r.d_V*model);
         s->SetUniform(r.d_uniformMV,r.d_V*model);//small todo

        std::size_t last_hash = -1;
        for (auto it = d_sm.begin(); it != d_sm.end();++it) {
            auto &submesh =  (*it);
            SMaterial m = d_materials[submesh->m_name];
            //glBindVertexArray ( submesh_idx[submesh->id].vao );

            if (last_hash != m.name_hash){
                last_hash = m.name_hash;
                BindTextures(m);
                for (int i = 0 ; i < r.MAX_RBO_TEXTURES;i++)
                {
                    if (r.d_RBOTexture[i]) {
                       r.d_RBOTexture[i]->Bind(5+i);
                   }
                }
                /*shader*/
                /*shadow mapping*/
            }
            auto& idx = submesh_idx[submesh->id];
            glBindBuffer(GL_ARRAY_BUFFER, idx.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx.ibo);
            /*direct approach*/

            if (locPositions != EFAIL) {
            glVertexAttribPointer ( locPositions, 3 ,GL_FLOAT,  GL_FALSE, sizeof(UVNVertex), (const GLvoid*) offsetof(UVNVertex,p) );
            glEnableVertexAttribArray ( locPositions );
            }
            if (locNormals != EFAIL) {
            glVertexAttribPointer ( locNormals, 3 ,GL_FLOAT,  GL_FALSE, sizeof(UVNVertex), (const GLvoid*) offsetof(UVNVertex,n) );
            glEnableVertexAttribArray ( locNormals );
            }
            if (locUV != EFAIL) {
            glVertexAttribPointer ( locUV, 2 ,GL_FLOAT,  GL_FALSE, sizeof(UVNVertex), (const GLvoid*) offsetof(UVNVertex,tc) );
            glEnableVertexAttribArray ( locUV );
            }

            //glBindVertexBuffer(0,idx.vbo,0, sizeof(CObjVertexN));

            int idx_c = submesh->indexes.size();
            glDrawElements(GL_TRIANGLES, idx_c, GL_UNSIGNED_INT, (GLvoid *)0);
            //glBindVertexArray ( 0 );

        }
    }
    glBindVertexArray(0);
}

SObjModel::~SObjModel() {
}
