#pragma once
#include "MTLParser.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "r_sprog.h"
#include "mat_math.h"
#include "viewport.h"
#include "r_texture.h"

#include "r_projmat.h"

#include "r_context.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <map>
#include "string_format.h"
#include "r_context.h"

/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>

#include <algorithm>

#include "MAssert.h"
#include "mat_math.h"
struct CObjV3 {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    // TODO: try use external
    /*serialize support */ 
//    friend class cereal::access;
//    template <class Archive>
//    void serialize( Archive & ar )
//    {
//        ar(CEREAL_NVP(x),CEREAL_NVP(y),CEREAL_NVP(z));
//    }

    bool operator<(const CObjV3& p) const {
        if (x != p.x )
            return (x < p.x);
        if (y != p.y )
            return (y < p.y);

        return (z < p.z);

    };
    bool operator!=(const CObjV3& p) const {
        if (x != p.x )
            return true;
        if (y != p.y )
            return true;
        if (z != p.z )
            return true;
        return false;

    };
};
struct CObjV2 {
    float u = 0.0;
    float v = 0.0;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(u),CEREAL_NVP(v));
    }
    bool operator<(const CObjV2& p) const {
        if (u != p.u )
            return (u < p.u);
        return (v < p.v);
    };
    bool operator!=(const CObjV2& p) const {
        if (u != p.u )
            return true;
        if (v != p.v )
            return true;
        return false;
    };

};

struct CObjIdx {
    int v_idx = 0;
    int vt_idx = 0;
    int n_idx = 0;
};
struct CObjFaceI {
    CObjIdx  f [4];
};

struct CObjVertexN {
    CObjV3 p;
    CObjV3 n;
    CObjV2 tc;
        /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(p),CEREAL_NVP(n),CEREAL_NVP(tc));
    }

    bool operator<(const CObjVertexN& o) const {
        if (p != o.p )
            return (p < o.p );
        if (n != o.n )
            return (n < o.n );
       // if (tc != o.tc )
         return (tc < o.tc );
    };

};

struct CObjSubmesh {
    std::string name; /* submesh name*/
    std::string m_name; /* material name*/
    bool flag_normals; /* is normals used */
    std::vector<CObjVertexN> vn; /* vertex list with normals*/
    std::vector<unsigned int> indexes; /*indexed mesh*/
   
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(name),CEREAL_NVP(m_name),CEREAL_NVP(flag_normals),CEREAL_NVP(vn),CEREAL_NVP(indexes));
    }
};



class CObjMeshParser {
public:
    CObjMeshParser(const std::string &fname);
    void Reflect();

    std::vector<std::shared_ptr<CObjSubmesh> > d_sm; /*set of submeshes*/
    std::vector<std::string> d_mtllibs; /* set of reference MTL files */
    bool IsReady = false;

    long int getVertexCount() const {return d_vertex_count;};

private:
    std::vector<CObjVertexN> BuildVerts(const std::vector<CObjV3> &glv, const std::vector<CObjV2> &glt , const CObjFaceI& face);
    std::vector<CObjVertexN> BuildVertsN(const std::vector<CObjV3> &glv, const std::vector<CObjV2> &glt, const std::vector<CObjV3> &gln , const CObjFaceI& face);
    CObjVertexN BuildVertN(const CObjV3 &p, const CObjV2 &tc, const CObjV3 &n);
    std::string ParseG(const std::string& str);
    std::string ParseUSEMTL(const std::string& str);
    std::string ParseMTLLIB(const std::string& str);
    CObjFaceI ParseF(const std::string& v_desc);
    CObjV2 ParseVt(const std::string& v_desc);
    CObjV3 ParseV(const std::string& v_desc);
    CObjV3 ParseVn(const std::string& v_desc);
    std::string ParseO(const std::string& str);
    void SortByMaterial();

    CObjV3 CalcNormal(const CObjV3 &v1, const CObjV3 &v2, const CObjV3 &v3);

    long int d_vertex_count; /* total vertex count */

    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(d_sm),CEREAL_NVP(d_mtllibs),CEREAL_NVP(d_vertex_count));
    }


};

CObjVertexN CObjMeshParser::BuildVertN(const CObjV3 &p, const CObjV2 &tc,const CObjV3 &n) {
    CObjVertexN  v = { p , n , tc}; 
    return v;
}

std::vector<CObjVertexN> CObjMeshParser::BuildVerts(const std::vector<CObjV3> &glv,const std::vector<CObjV2> &glt , const CObjFaceI& face) {
    std::vector<CObjVertexN> res;
    CObjV3 normal = CalcNormal(glv[ face.f[0].v_idx - 1.0 ],glv[ face.f[1].v_idx - 1.0 ],glv[ face.f[2].v_idx - 1.0 ]);
    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
        CObjV2 empty_v2;

        if ( face.f[3].v_idx == 0 ) { 
        // consider this is single triangle 

            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        } else {
        // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        }
    } else {
                if ( face.f[3].v_idx == 0 ) { 
        // consider this is single triangle 
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal) );
        } else {
        // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  ,normal) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ,normal) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  ,normal) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ,normal) );
        }
    }
    return res;
}



std::vector<CObjVertexN> CObjMeshParser::BuildVertsN(const std::vector<CObjV3> &glv,const std::vector<CObjV2> &glt ,const std::vector<CObjV3> &gln, const CObjFaceI& face) {
    std::vector<CObjVertexN> res;

    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
            CObjV2 empty_v2;
        if ( face.f[3].v_idx == 0 ) { 
            // consider this is single triangle 
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2, gln[ face.f[0].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,empty_v2   , gln[ face.f[3].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }

    } else {
        if ( face.f[3].v_idx == 0 ) { 
            // consider this is single triangle 
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] , gln[ face.f[0].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  , gln[ face.f[3].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }
    }
    return res;
}

std::string CObjMeshParser::ParseG(const std::string &str) {
    char buf[90];
    sscanf(str.c_str(),"g %80s",buf);
    return std::string(buf);
}

std::string CObjMeshParser::ParseO(const std::string &str) {
    char buf[90];
    sscanf(str.c_str(),"o %80s",buf);
    return std::string(buf);
}

void CObjMeshParser::SortByMaterial()
{
    std::sort(d_sm.begin(), d_sm.end(), [](std::shared_ptr<CObjSubmesh> a , std::shared_ptr<CObjSubmesh>  b) -> bool  { return (a->m_name >  b->m_name);} );
}

CObjV3 CObjMeshParser::CalcNormal(const CObjV3& v1, const CObjV3& v2, const CObjV3& v3)
{
    SVec4 vec1(v1.x,v1.y,v1.z,0.0);

    SVec4 vec2(v2.x,v2.y,v2.z,0.0);

    SVec4 vec3(v3.x,v3.y,v3.z,0.0);

    SVec4 n = SVec4::Normalize( SVec4::Cross( (vec1 - vec2) ,(vec1 - vec3)) );

    CObjV3 e;
    e.x = n.vec.x;
    e.y = n.vec.y;
    e.z = n.vec.z;
    return e;

}
std::string CObjMeshParser::ParseUSEMTL(const std::string &str) {
    char buf[90];
    sscanf(str.c_str(),"usemtl %80s",buf);
    return std::string(buf);
}
std::string CObjMeshParser::ParseMTLLIB(const std::string &str) {
    char buf[90];
    sscanf(str.c_str(),"mtllib %80s",buf);
    return std::string(buf);
}
// TODO: optimize
CObjFaceI CObjMeshParser::ParseF(const std::string &v_desc) {
    CObjFaceI obj;

    if (v_desc.find("//") != std::string::npos)  /* vector//normal format  - no texture coordiates */ {
      
        int ret = sscanf(v_desc.c_str(),"f %d//%d %d//%d %d//%d %d//%d"  
            , &obj.f[0].v_idx,  &obj.f[0].n_idx 
            , &obj.f[1].v_idx,  &obj.f[1].n_idx 
            , &obj.f[2].v_idx,  &obj.f[2].n_idx 
            , &obj.f[3].v_idx,  &obj.f[3].n_idx);

            obj.f[0].vt_idx  = 0;
            obj.f[1].vt_idx  = 0;
            obj.f[2].vt_idx  = 0;
            obj.f[3].vt_idx  = 0; 
            
           if (ret != 6) { /*in triangle case should be 6 parsable otherwice 8*/
             ret = sscanf(v_desc.c_str(),"f %d//%d %d//%d %d//%d" 
                , &obj.f[0].v_idx , &obj.f[0].n_idx  
                , &obj.f[1].v_idx , &obj.f[1].n_idx  
                , &obj.f[2].v_idx , &obj.f[2].n_idx  
                );
            }
           return obj;
    }


    int ret = sscanf(v_desc.c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d"
            , &obj.f[0].v_idx , &obj.f[0].vt_idx , &obj.f[0].n_idx 
            , &obj.f[1].v_idx , &obj.f[1].vt_idx , &obj.f[1].n_idx 
            , &obj.f[2].v_idx , &obj.f[2].vt_idx , &obj.f[2].n_idx 
            , &obj.f[3].v_idx , &obj.f[3].vt_idx , &obj.f[3].n_idx);

    if (ret != 9) {

    ret = sscanf(v_desc.c_str(),"f %d/%d %d/%d %d/%d %d/%d" 
            , &obj.f[0].v_idx , &obj.f[0].vt_idx  
            , &obj.f[1].v_idx , &obj.f[1].vt_idx  
            , &obj.f[2].v_idx , &obj.f[2].vt_idx  
            , &obj.f[3].v_idx , &obj.f[3].vt_idx );
    }
    return obj;
}

CObjV2 CObjMeshParser::ParseVt(const std::string &v_desc) {

    CObjV2 obj;
    float tmp;
    sscanf(v_desc.c_str(),"vt %f %f %f ", &obj.u, &obj.v, &tmp);
    return obj;
}

CObjV3 CObjMeshParser::ParseV(const std::string &v_desc) {

    CObjV3 obj;
    sscanf(v_desc.c_str(),"v %f %f %f", &obj.x, &obj.y, &obj.z);
    return obj;
}
CObjV3 CObjMeshParser::ParseVn(const std::string &v_desc) {

    CObjV3 obj;
    sscanf(v_desc.c_str(),"vn %f %f %f", &obj.x, &obj.y, &obj.z);
    return obj;
}

void CObjMeshParser::Reflect() {
    printf("Reflect for CObjMeshParser\n");
    printf("Total vertexes %lu\n",getVertexCount());
    for (auto it = d_sm.begin(); it != d_sm.end();++it) {
        printf("SubMesh name=%s m_name=%s c =%d \n", (*it)->name.c_str() ,  (*it)->m_name.c_str() ,(*it)->vn.size() );

           // for (std::vector<CObjVertex>::iterator it2 = (*it)->v.begin(); it2 != (*it)->v.end();++it2) {
           // auto &v = (*it2);
           // printf("%f %f %f : %f %f \n",  v.p.x, v.p.y, v.p.z, v.tc.u, v.tc.v);
            //}


    }
}


CObjMeshParser::CObjMeshParser(const std::string& fname)
 :d_vertex_count(0)
 {
    std::ifstream tst;

    std::vector<CObjV3> d_glv; /*list of vectors*/
    std::vector<CObjV2> d_gltc; /*list of texture coordinates*/
    std::vector<CObjV3> d_gln; /*list of normals*/
    bool d_normals = false; /*is normals used */
    tst.open(fname);
    if (tst) {

        for( std::string line; getline( tst, line ); ) {
            //std::cout << "Parsing line "<< line << std::endl; 
            auto &subm =  (d_sm.back());
            if (!line.find("vt") ){
                //(d_sm.back())->flag_normals = true;
                d_gltc.push_back(ParseVt(line));
            } else if (!line.find("vn")) { 
                d_normals = true;
                d_gln.push_back(ParseVn(line));
            } else if (!line.find("o")) { 
                    /*begin new submesh*/
                    std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                    s->flag_normals = true;
                    d_normals = true; /* thats crap*/
                    s->name = ParseO(line);
                    d_sm.push_back(s);

            } else if (!line.find("s")) {
                //TODO
            } else if (!line.find("usemtl")) {
                const std::string& mtl = ParseUSEMTL(line);
                /* sometimes can use different materials for single submesh*/
                if (!subm->m_name.empty()) {
                    /*begin new submesh*/
                    std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                    s->flag_normals = d_normals;
                    s->name = subm->name + std::string("_mat_") + mtl;
                    s->m_name = mtl;
                    d_sm.push_back(s);
                } else
                    subm->m_name = mtl;

            } else if (!line.find("mtllib")) {
                d_mtllibs.push_back(ParseMTLLIB(line));
            } else if (!line.find("f")) {
                CObjFaceI fi = ParseF(line);
                if (d_normals){
                    std::vector<CObjVertexN> a_v = BuildVertsN(d_glv,d_gltc,d_gln,fi);
                    std::vector<CObjVertexN> &o_v = subm->vn;
                    d_vertex_count += a_v.size();
                    o_v.insert(o_v.end(), a_v.begin(), a_v.end());
                } else {
                    std::vector<CObjVertexN> a_v = BuildVerts(d_glv,d_gltc,fi);
                    d_vertex_count += a_v.size();
                    std::vector<CObjVertexN> &o_v = subm->vn;
                    o_v.insert(o_v.end(), a_v.begin(), a_v.end());
                }
            } else if (!line.find("v")) {
                d_glv.push_back(ParseV(line));
            } else if (!line.find("g")) {
                std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                s->flag_normals = d_normals;
                s->name = ParseG(line);
                d_sm.push_back(s);
                //d_normals = true;
            } else if (!line.find("#")) {
                // comment
            } else if (!line.find(" ")) {
                // empty line with space
            } else if (line.empty()) {
                // empty line
            } else {
                std::cout << "Unrecogized OBJ tag: "<< line << " while parsing:"   << fname << std::endl;
            };
        }
    } else  {
        std::cout << "File Open failed!" << std::endl;
        return;
    }
    tst.close();
    SortByMaterial();
    IsReady = true;
}


class MeshIndexer {
public:
    MeshIndexer(const std::shared_ptr<CObjSubmesh> &submesh)
        :d_inmesh(submesh)
    {}
    std::shared_ptr<CObjSubmesh> Do();
private:
    const std::shared_ptr<CObjSubmesh> d_inmesh;
};

std::shared_ptr<CObjSubmesh> MeshIndexer::Do()
{

    CObjSubmesh *mesh = new CObjSubmesh ;

    mesh->m_name = d_inmesh->m_name;
    mesh->flag_normals = d_inmesh->flag_normals;
    mesh->name = d_inmesh->name;
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
    return  std::shared_ptr<CObjSubmesh>(mesh);

}
class SObjModel {
    public: 
        SObjModel(const std::string& fname);
        void Render(RenderContext& r);
        int ConfigureProgram(SShader& sprog);

        ~SObjModel();

        //matrices
        SMat4x4 model;


        // default textures for fail prove
        std::shared_ptr<STexture> texDiffuse;
        std::shared_ptr<STexture> texNormal;

        // ready flag
        bool IsReady = false;
        void SetModelMat(const SMat4x4& m);
    private:
        
        void BindTextures(const std::shared_ptr<CObjSubmesh> &submesh);
        
        std::vector<std::shared_ptr<CObjSubmesh> > d_sm;



        std::unordered_map<std::string, unsigned int > submesh_vbo;
        std::unordered_map<std::string, unsigned int > submesh_vao;
        std::unordered_map<std::string, unsigned int > submesh_ibo;
        std::unordered_map<std::string, std::unique_ptr<STexture> > d_textures;
        std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > d_materials;

        bool flag_normals = false;
        
        friend class cereal::access;
        /*serialize support */
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( CEREAL_NVP(IsReady),
                CEREAL_NVP(model),
                CEREAL_NVP(d_materials),
                CEREAL_NVP(d_textures)
                );
        }




};
int SObjModel::ConfigureProgram(SShader& sprog){
    if (!IsReady)
        return EFAIL;
    sprog.Bind();
    for (auto it = d_sm.begin(); it != d_sm.end();++it) {
        auto &submesh =  (*it);
        glBindVertexArray( submesh_vao[(*it)->name] );
        glBindBuffer ( GL_ARRAY_BUFFER, submesh_vbo[(*it)->name] );

            sprog.SetAttrib( "position", 3, sizeof(CObjVertexN), offsetof(CObjVertexN,p),GL_FLOAT);
            sprog.SetAttrib( "normal", 3, sizeof(CObjVertexN),  offsetof(CObjVertexN,n),GL_FLOAT);
            sprog.SetAttrib( "UV", 2, sizeof(CObjVertexN),  offsetof(CObjVertexN,tc),GL_FLOAT);

            sprog.SetUniform("mesh_flags",1);
        sprog.SetUniform("texIMG",0);
        sprog.SetUniform("texBUMP",1);
        sprog.SetUniform("texture_alpha_sampler",2);
        sprog.SetUniform("model",model);

        sprog.SetUniform("sm_depth_sampler",5);
        sprog.SetUniform("rsm_normal_sampler",6);
        sprog.SetUniform("rsm_vector_sampler",7);
        sprog.SetUniform("rsm_albedo_sampler",8);
        BindTextures(submesh);
        glBindVertexArray(0);
    }
    
    return 0;
}
SObjModel::SObjModel(const std::string&  fname) 
{
    CObjMeshParser parser(fname);
    if (!parser.IsReady)
        {
            //EMSGS(std::string("Unable open model :") + fname);
            std::cout <<   std::string("Unable open model :") << fname << std::endl;
        return;
        }
    printf("Indexing mesh\n");

    for (auto it = parser.d_sm.begin(); it != parser.d_sm.end();++it) {
        MeshIndexer idx(*it);
        d_sm.push_back(std::shared_ptr<CObjSubmesh>(idx.Do()));
        (*it).reset();
    }
    printf("Load materials\n");

    /* load descriptor*/
    unsigned int temp_vao;
    unsigned int temp_vbo;
    unsigned int temp_ibo;
    {
        if (parser.d_mtllibs.empty())
        {
            MTLParser mtl_p("default.mtl");
            d_materials = mtl_p.d_materials; //OMG copy!! FIX ME
        }
        else
        {
             MTLParser mtl_p(parser.d_mtllibs[0]);
            d_materials = mtl_p.d_materials; //OMG copy!! FIX ME
        }
    }


    texDiffuse.reset(new STexture("empty_texture.png"));
    if (!texDiffuse->IsReady) {
        EMSGS(std::string(" diffuse texture file not found"));
        return;
    }
    texNormal.reset(new STexture("empty_normal.png",false));
    if (!texNormal->IsReady) {
        EMSGS(std::string("normal texture file not found"));
        return;
    }

    /*configure mesh prop*/
    printf("loaded shader\n");

    for (auto it = d_sm.begin(); it != d_sm.end();++it) {

        auto &submesh =  (*it);
        flag_normals = submesh->flag_normals;
        if (d_materials.find(submesh->m_name) == d_materials.end()) {
           // printf("no material found - %s\n",submesh->m_name.c_str() );
        } else {
            
            auto &material = d_materials[submesh->m_name];
           
            std::string &diffuse = material->map_Kd;
            if (d_textures.find(diffuse) == d_textures.end()) {
               // printf("material %s Diffuse %s Bump %s Alpha %s \n",submesh->m_name.c_str(),  material->map_Kd.c_str(), material->map_bump.c_str(), material->map_d.c_str());
                d_textures[diffuse] = std::unique_ptr<STexture>(new STexture(diffuse));
                if (!d_textures[diffuse]->IsReady) {
                    EMSGS(string_format("OBJ:Diffuse texture load failed %s",diffuse.c_str()));
                }
            }

            std::string &bump = material->map_bump;
            if (d_textures.find(bump) == d_textures.end()) {

                d_textures[bump] = std::unique_ptr<STexture>(new STexture(bump,false));
                if (!d_textures[bump]->IsReady) {
                    EMSGS(string_format("OBJ:Bump texture load failed %s",bump.c_str()));
                }
            }
            std::string &alpha = material->map_d; 
            if (d_textures.find(alpha) == d_textures.end()) {

                d_textures[alpha] = std::unique_ptr<STexture>(new STexture(alpha));
                if (!d_textures[alpha]->IsReady) {
                    EMSGS(string_format("OBJ:Alpha mask texture load failed %s",alpha.c_str()));
                }
            }



        }
/*
    std::string map_Ka; ambient map 
    std::string map_Kd; diffuse map
    std::string map_Ns; Specular shinnes map
    std::string map_d; alpha map
    std::string map_bump; 
*/
            /*gen buffers for submesh*/
        /*vertixes*/
        temp_vao = 0;
        temp_vbo = 0;
        temp_ibo = 0;
        glGenVertexArrays ( 1, &temp_vao );
        glGenBuffers ( 1, &temp_vbo );
        glBindVertexArray(temp_vao);
        glBindBuffer(GL_ARRAY_BUFFER,temp_vbo);
        glGenBuffers(1, &temp_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, temp_ibo);

        MASSERT(submesh->vn.empty());
        glBufferData ( GL_ARRAY_BUFFER, submesh->vn.size() *sizeof(CObjVertexN) , submesh->vn.data(), GL_STATIC_DRAW);

        MASSERT(submesh->indexes.empty());
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,submesh->indexes.size() *sizeof(unsigned int), submesh->indexes.data(), GL_STATIC_DRAW);
        
        submesh_vbo [ submesh->name ] = temp_vbo;
        submesh_vao [submesh->name] = temp_vao; 
        submesh_ibo [submesh->name] = temp_ibo;


        glBindVertexArray(0);
    }
printf("model configured\n");

    IsReady = true;

}
void SObjModel::SetModelMat(const SMat4x4& m){
	model = m;
	/*update shader variable*/ 
   // sprog->SetUniform("model",model);
}
void SObjModel::BindTextures(const std::shared_ptr<CObjSubmesh> &submesh) {
    
    /*texture diffuse*/
    if (d_materials.find(submesh->m_name) != d_materials.end()) {
        auto &material = d_materials[submesh->m_name];
        if (d_textures.find(material->map_Kd) != d_textures.end()) {
            auto &diffuse_texture =  d_textures[material->map_Kd];
            if (diffuse_texture->IsReady)
                diffuse_texture->Bind(0);
            else
                texDiffuse->Bind(0);
        } else
            texDiffuse->Bind(0);   
    } else
    texDiffuse->Bind(0);

    /*texture bump*/
    if (d_materials.find(submesh->m_name) != d_materials.end()) {
        auto &material = d_materials[submesh->m_name];
        if (d_textures.find(material->map_bump) != d_textures.end()) {
            auto &bump_texture =  d_textures[material->map_bump];
            if (bump_texture->IsReady)
                bump_texture->Bind(1);
            else
                texNormal->Bind(1);
        } else
            texNormal->Bind(1);   
    } else
    texNormal->Bind(1);
        /*texture alpha*/
    if (d_materials.find(submesh->m_name) != d_materials.end()) {
        auto &material = d_materials[submesh->m_name];
        if (d_textures.find(material->map_d) != d_textures.end()) {
            auto &alpha_texture =  d_textures[material->map_d];
            if (alpha_texture->IsReady)
                alpha_texture->Bind(2);
            else
                texDiffuse->Bind(2);
        } else
            texDiffuse->Bind(2);   
    } else
        texDiffuse->Bind(2);
    

}
void SObjModel::Render(RenderContext& r) {
    /*activate shader and load model matrix*/
    glDepthMask(GL_TRUE);
    if (r.shader->IsReady) {
        for (auto it = d_sm.begin(); it != d_sm.end();++it) {
            auto &submesh =  (*it);

            BindTextures(submesh);
            glBindVertexArray ( submesh_vao[submesh->name] );
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
            //texDiffuse->Bind(0);
            //texNormal->Bind(1);
            r.shader->SetUniform("model",model);
            r.shader->SetUniform("view",r.camera->getViewMatrix());
            r.shader->SetUniform("cam_proj",r.camera->getProjMatrix());
            r.shader->Bind();

           // if (submesh->flag_normals)
             //  glDrawArrays(GL_TRIANGLES,0,submesh->vn.size());

           // else
           //    glDrawArrays(GL_TRIANGLES,0,submesh->v.size());


            int idx_c = submesh->indexes.size();
            glDrawElements(GL_TRIANGLES, idx_c, GL_UNSIGNED_INT, (GLvoid *)0);
            glBindVertexArray ( 0 );


        }
    }
}

SObjModel::~SObjModel() {
}
