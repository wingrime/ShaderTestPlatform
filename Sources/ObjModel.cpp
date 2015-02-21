#include "ObjModel.h"
#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "r_sprog.h"
#include "r_shader.h"
#include "mat_math.h"
#include "RBO.h"
#include "r_texture.h"
#include "r_projmat.h"
#include "r_context.h"
#include "MAssert.h"

#include "string_format.h"
#include "Log.h"
#include "ObjParser.h"
#include "ErrorCodes.h"
std::shared_ptr<CObjSubmesh> MeshIndexer::Do()
{
    LOGV(string_format("Indexing submesh name=%s,m_name=%s, id= %d,triangles=%d ",d_inmesh->name.c_str(),d_inmesh->m_name.c_str(),d_inmesh->id, d_inmesh->vn.size()));
    std::shared_ptr<CObjSubmesh> mesh;
    mesh.reset(new CObjSubmesh());
    mesh->m_name = d_inmesh->m_name;
    mesh->name = d_inmesh->name;
    mesh->id = d_inmesh->id;
    unsigned int current_index = 0;

    std::map<CObjVertexN, unsigned int  > vn_map;


    for( auto it = std::begin(d_inmesh->vn); it != std::end(d_inmesh->vn); ++it) {
        std::map<CObjVertexN, unsigned int  >::const_iterator k = vn_map.find(*it);
        if (k == vn_map.end()) {
        /* if there is no index, then add new index*/
            vn_map[*it] = current_index;
            mesh->vn.push_back(*it);
            mesh->indexes.push_back(current_index);
            current_index++;
        } else {
            /*if it is in index*/
            mesh->indexes.push_back(vn_map[*it]);
        }

    }
    LOGV(string_format("Indexing result: indepened_triangles=%d",mesh->vn.size()) );
    return  (mesh);

}

int SObjModel::ConfigureProgram(SShader& sprog){
    if (!IsReady)
        return EFAIL;

    sprog.SetUniform("texIMG",0);
    sprog.SetUniform("texBUMP",1);
    sprog.SetUniform("texture_alpha_sampler",2);
    sprog.SetUniform("model",model);

    sprog.SetUniform("sm_depth_sampler",5);
    sprog.SetUniform("rsm_normal_sampler",6);
    sprog.SetUniform("rsm_vector_sampler",7);
    sprog.SetUniform("rsm_albedo_sampler",8);
    sprog.SetUniform("sh_bands_sampler",9);
    sprog.Bind();

    return 0;
}

SObjModel::SObjModel(const std::string&  fname) 
{
    LOGV(std::string("Opening obj model ")+fname);
    CObjMeshParser parser(fname);
    if (!parser.IsReady)
        {
            LOGE(std::string("Unable open model"));
            return;
        }

    auto submesh_set = parser.getSM();
    long total_triangles = 0;
    for (auto it = submesh_set.begin(); it != submesh_set.end();++it) {
        MeshIndexer idx(*it);
        auto res = std::shared_ptr<CObjSubmesh>(idx.Do());
        total_triangles += res->vn.size();
        d_sm.push_back(res);
        (*it).reset();
   }
    LOGV(string_format("Mesh information: Submesh count:%d, Total mesh triangles:%d", d_sm.size(), total_triangles ));

    LOGV("Load materials");

    {
        auto mtlrefs = parser.getMTLs();
        if (!mtlrefs.empty())
        {
            MTLParser mtl_p(mtlrefs[0]);
            d_materials = mtl_p.GetMaterials(); //OMG copy!! FIX ME
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
void SObjModel::BindTextures(Material * m) {
    if (m->alpha && m->alpha->IsReady)
        m->alpha->Bind(2);
    else
        texDiffuse->Bind(2);
    if (m->diffuse && m->diffuse->IsReady)
        m->diffuse->Bind(0);
    else
        texDiffuse->Bind(0);
    if (m->bump && m->bump->IsReady)
        m->bump->Bind(1);
    else
        texNormal->Bind(1);
}

void SObjModel::LoadTextures() {

    texDiffuse.reset(new STexture("empty_texture.png"));
    if (!texDiffuse->IsReady) {
        LOGE(std::string(" diffuse texture file not found"));
        return;
    }
    texNormal.reset(new STexture("empty_normal.png",false));
    if (!texNormal->IsReady) {
       LOGE(std::string("normal texture file not found"));
        return;
    }

    for (auto it = d_sm.begin(); it != d_sm.end();++it) {

        auto &submesh =  (*it);
        if (d_materials.find(submesh->m_name) == d_materials.end()) {
           LOGE(std::string("no material found - ") + submesh->m_name);
        } else {

            auto &material = d_materials[submesh->m_name];


            std::string &diffuse = material->map_Kd;
            if (d_textures.find(diffuse) == d_textures.end()) {
                LOGV(string_format("material %s Diffuse %s Bump %s Alpha %s",submesh->m_name.c_str(),
                                   material->map_Kd.c_str(),
                                   material->map_bump.c_str(),
                                   material->map_d.c_str()));
                d_materails[submesh->m_name].name_hash = material->name_hash;
                d_materails[submesh->m_name].diffuse =  new STexture(diffuse);
                d_textures[diffuse].reset( d_materails[submesh->m_name].diffuse);
                if (!d_textures[diffuse]->IsReady) {
                   LOGE(string_format("OBJ:Diffuse texture load failed %s",diffuse.c_str()));
                }
            }

            std::string &bump = material->map_bump;
            if (d_textures.find(bump) == d_textures.end()) {
                d_materails[submesh->m_name].bump =  new STexture(bump,false);
                d_textures[bump].reset(d_materails[submesh->m_name].bump);
                if (!d_textures[bump]->IsReady) {
                  LOGE(string_format("OBJ:Bump texture load failed %s",bump.c_str()));
                }
            }
            std::string &alpha = material->map_d;
            if (d_textures.find(alpha) == d_textures.end()) {
                d_materails[submesh->m_name].alpha = new STexture(alpha);
                d_textures[alpha].reset(d_materails[submesh->m_name].alpha);
                if (!d_textures[alpha]->IsReady) {
                   LOGE(string_format("OBJ:Alpha mask texture load failed %s",alpha.c_str()));
                }
            }



        }


    }
}

void SObjModel::BindVAOs() {
    /* load descriptor*/
    unsigned int temp_vao;
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

            MASSERT(submesh->vn.empty());
            MASSERT(submesh->indexes.empty());
            //gl 4.5 without EXT
            glNamedBufferDataEXT(temp_vbo,submesh->vn.size() *sizeof(CObjVertexN), submesh->vn.data(), GL_STATIC_DRAW);
            glNamedBufferDataEXT(temp_ibo,submesh->indexes.size() *sizeof(unsigned int), submesh->indexes.data(), GL_STATIC_DRAW);
            SubMeshIDs idx;
            idx.vbo = temp_vbo;
            idx.ibo = temp_ibo;

            submesh_idx[submesh->id] = idx;
            //glBindBuffer(GL_ARRAY_BUFFER,0);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

        }
        //glBindVertexArray(0);
}
void SObjModel::Render(RenderContext& r) {
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
        s->SetUniform(r.d_viewMatrixLoc,r.camera->getViewMatrix());
        s->SetUniform(r.d_projMatrixLoc,r.camera->getProjMatrix());
        /*optimize*/
         s->SetUniform(r.d_MVP,r.camera->getViewProjectMatrix());
         s->SetUniform(r.d_MV,r.camera->getViewMatrix()*model);//small todo

        std::size_t last_hash = -1;
        for (auto it = d_sm.begin(); it != d_sm.end();++it) {
            auto &submesh =  (*it);
            Material m = d_materails[submesh->m_name];
            //glBindVertexArray ( submesh_idx[submesh->id].vao );

            if (last_hash != m.name_hash){
                last_hash = m.name_hash;
                BindTextures(&m);

                /*shader*/
                /*shadow mapping*/
                if (r.sm_texture) {
                   r.sm_texture->Bind(5);
                }
                if (r.rsm_normal_texture) {
                   r.rsm_normal_texture->Bind(6);
               }
                if (r.rsm_vector_texture) {
                   r.rsm_vector_texture->Bind(7);
                }
                if (r.rsm_albedo_texture) {
                   r.rsm_albedo_texture->Bind(8);
                }
                if (r.sh_bands) {
                   r.sh_bands->Bind(9);
                }
            }
            auto& idx = submesh_idx[submesh->id];
            glBindBuffer(GL_ARRAY_BUFFER, idx.vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx.ibo);
            /*direct approach*/

            if (locPositions != EFAIL) {
            glVertexAttribPointer ( locPositions, 3 ,GL_FLOAT,  GL_FALSE, sizeof(CObjVertexN), (const GLvoid*) offsetof(CObjVertexN,p) );
            glEnableVertexAttribArray ( locPositions );
            }
            if (locNormals != EFAIL) {
            glVertexAttribPointer ( locNormals, 3 ,GL_FLOAT,  GL_FALSE, sizeof(CObjVertexN), (const GLvoid*) offsetof(CObjVertexN,n) );
            glEnableVertexAttribArray ( locNormals );
            }
            if (locUV != EFAIL) {
            glVertexAttribPointer ( locUV, 2 ,GL_FLOAT,  GL_FALSE, sizeof(CObjVertexN), (const GLvoid*) offsetof(CObjVertexN,tc) );
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
