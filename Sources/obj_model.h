#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/wglew.h>

/* shader class*/
#include "r_sprog.h"
#include "mat_math.h"
#include "viewport.h"
#include "r_texture.h"
#if OLD_SERIALIZE
#include "c_config.h"
#endif
#include "r_projmat.h"

#include "r_context.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

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


struct CObjV3 {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;
    // TODO: try use external
    /*serialize support */ 
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(x),CEREAL_NVP(y),CEREAL_NVP(z));
    }
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


};

struct CObjIdx {
    int v_idx = 0;
    int vt_idx = 0;
    int n_idx = 0;
};
struct CObjFaceI {
    CObjIdx  f [4];
};

struct CObjVertex {
    CObjV3 p;
    CObjV2 tc;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(p),CEREAL_NVP(tc));
    }
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

};

struct CObjSubmesh {
    std::string name; /* submesh name*/
    std::string m_name; /* material name*/
    bool flag_normals; /* is normals used */
    std::vector<CObjVertex> v; /* vertex list without normals*/
    std::vector<CObjVertexN> vn; /* vertex list with normals*/
   
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(name),CEREAL_NVP(m_name),CEREAL_NVP(flag_normals),CEREAL_NVP(v),CEREAL_NVP(vn));
    }
};
struct CMTLColor {
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(r),CEREAL_NVP(g),CEREAL_NVP(b));
    }
};

struct CMTLMaterial {
    CMTLColor Ka; /* ambient color   */
    CMTLColor Kd; /* diffuse color   */
    CMTLColor Ks; /* specular color  */
    CMTLColor Ke; /* emmission color */
    CMTLColor Tf; /* transmission filter color */
    float     Ns = 0.0;  /* specular coeff*/
    float     Ni = 0.0; /* IOR */
    float     d = 0.0; /*dissolve (transpernt) */
    int illum = 0;   /*illumination model */

    std::string map_Ka; /*ambient map */
    std::string map_Kd; /*diffuse map*/
    std::string map_Ns; /*Specular shinnes map*/
    std::string map_d; /*alpha mask*/
    std::string map_bump; /*bump */

    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( CEREAL_NVP(Ka),CEREAL_NVP(Kd),CEREAL_NVP(Ks),CEREAL_NVP(Ke),CEREAL_NVP(Tf),CEREAL_NVP(Ns),CEREAL_NVP(Ni),CEREAL_NVP(d), CEREAL_NVP(illum),
            CEREAL_NVP(map_Ka),CEREAL_NVP(map_Kd),CEREAL_NVP(map_Ns),CEREAL_NVP(map_d),CEREAL_NVP(map_bump));
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
    std::vector<CObjVertex> BuildVerts(std::vector<CObjV3> &glv,std::vector<CObjV2> &glt , const CObjFaceI& face);
    std::vector<CObjVertexN> BuildVertsN(std::vector<CObjV3> &glv,std::vector<CObjV2> &glt,std::vector<CObjV3> &gln , const CObjFaceI& face);
    CObjVertex BuildVert(CObjV3 &p, CObjV2 &tc);
    CObjVertexN BuildVertN(CObjV3 &p, CObjV2 &tc,CObjV3 &n);
    std::string ParseG(const std::string& str);
    std::string ParseUSEMTL(const std::string& str);
    std::string ParseMTLLIB(const std::string& str);
    CObjFaceI ParseF(const std::string& v_desc);
    CObjV2 ParseVt(const std::string& v_desc);
    CObjV3 ParseV(const std::string& v_desc);
    CObjV3 ParseVn(const std::string& v_desc);
    std::string ParseO(const std::string& str);
    void SortByMaterial();

    long int d_vertex_count; /* total vertex count */

    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(d_sm),CEREAL_NVP(d_mtllibs),CEREAL_NVP(d_vertex_count));
    }


};

CObjVertex CObjMeshParser::BuildVert(CObjV3 &p, CObjV2 &tc) {
    CObjVertex  v = { p , tc }; 
    return v;
}
CObjVertexN CObjMeshParser::BuildVertN(CObjV3 &p, CObjV2 &tc,CObjV3 &n) {
    CObjVertexN  v = { p , n , tc}; 
    return v;
}

std::vector<CObjVertex> CObjMeshParser::BuildVerts(std::vector<CObjV3> &glv,std::vector<CObjV2> &glt , const CObjFaceI& face) {
    std::vector<CObjVertex> res;
    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
        CObjV2 empty_v2;

        if ( face.f[3].v_idx == 0 ) { 
        // consider this is single triangle 
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ) );
            res.push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2  ) );
            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2  ) );
        } else {
        // consider this is face
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2  ) );
            res.push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2  ) );
            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2  ) );

            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2  ) );
            res.push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  , empty_v2  ) );
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2  ) );
        }
    } else {
                if ( face.f[3].v_idx == 0 ) { 
        // consider this is single triangle 
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ) );
        } else {
        // consider this is face
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ) );

            res.push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  ) );
            res.push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ) );
        }
    }
    return res;
}



std::vector<CObjVertexN> CObjMeshParser::BuildVertsN(std::vector<CObjV3> &glv,std::vector<CObjV2> &glt ,std::vector<CObjV3> &gln, const CObjFaceI& face) {
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
    char buf[256];
    sscanf(str.c_str(),"g %80s",buf);
    return std::string(buf);
}

std::string CObjMeshParser::ParseO(const std::string &str) {
    char buf[256];
    sscanf(str.c_str(),"o %80s",buf);
    return std::string(buf);
}

void CObjMeshParser::SortByMaterial()
{
    std::sort(d_sm.begin(), d_sm.end(), [](std::shared_ptr<CObjSubmesh> a , std::shared_ptr<CObjSubmesh>  b) -> bool  { return (a->m_name >  b->m_name);} );
}
std::string CObjMeshParser::ParseUSEMTL(const std::string &str) {
    char buf[256];
    sscanf(str.c_str(),"usemtl %80s",buf);
    return std::string(buf);
}
std::string CObjMeshParser::ParseMTLLIB(const std::string &str) {
    char buf[256];
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
        printf("SubMesh name=%s m_name=%s c =%d cn = %d \n", (*it)->name.c_str() ,  (*it)->m_name.c_str() , (*it)->v.size(),(*it)->vn.size() );

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
                if (subm->m_name != "") {
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
                    std::vector<CObjVertex> a_v = BuildVerts(d_glv,d_gltc,fi);
                    d_vertex_count += a_v.size();
                    std::vector<CObjVertex> &o_v = subm->v;
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


std::string ParseNEWMTL(const std::string& str) {
    char buf[256];
    sscanf(str.c_str(),"newmtl %80s",buf);
    return std::string(buf);
}

float ParseNs(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," Ns %f", &r);
    return r;
}
float ParseNi(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," Ni %f", &r);
    return r;
}
float Parsed(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," d %f", &r);
    return r;
}

int Parseillum(const std::string& v_desc) {
    int r = 0;
    sscanf(v_desc.c_str()," illum %d", &r);
    return r;
}

CMTLColor ParseKa(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ka %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor ParseKd(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Kd %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor ParseKs(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ks %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor ParseKe(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ke %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor ParseTf(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Tf %f %f %f", &c.r, &c.g, &c.b);
    return c;
}

std::string Parsemap_Ka(const std::string& str) {
    char buf[256];
    sscanf(str.c_str()," map_Ka %80s",buf);
    return std::string(buf);
}
std::string Parsemap_Kd(const std::string& str) {
    char buf[256];
    sscanf(str.c_str()," map_Kd %80s",buf);
    return std::string(buf);
}
std::string Parsemap_d(const std::string& str) {
    char buf[256];
    sscanf(str.c_str()," map_d %80s",buf);
    return std::string(buf);
}
std::string Parsemap_bump(const std::string& str) {
    char buf[256];
    
    sscanf(str.c_str()," map_bump %80s",buf);
    return std::string(buf);
}

int ReflectCMTLMaterial(CMTLMaterial & mat) {

    //printf();
return 0;
}

std::unordered_map<std::string, std::unique_ptr<CMTLMaterial> > ParseMTL(std::string fname) {
    printf("Load MTL file %s \n",fname.c_str());
    std::ifstream tst;
    std::unordered_map<std::string, std::unique_ptr<CMTLMaterial> > res;

    std::string mtln("default_mtl");

    tst.open(fname);
    if (tst) {
        for( std::string line; getline( tst, line ); ) {
            if (!line.find("newmtl") ){
                    mtln = ParseNEWMTL(line);
                    if (res.count(mtln) == 0 ) {/* all materials are unique*/
                        res[mtln] = std::unique_ptr<CMTLMaterial>(new CMTLMaterial());
                    }
            } else if (line.find("map_Ka" ) != std::string::npos) {
                    res[mtln]->map_Ka = Parsemap_Ka(line);  
            } else if (line.find("map_Kd") != std::string::npos) {
                    res[mtln]->map_Kd = Parsemap_Kd(line);  
            } else if (line.find("map_bump")!= std::string::npos ) {
                    res[mtln]->map_bump = Parsemap_bump(line);  
            } else if (line.find("map_d")!= std::string::npos ) {
                    res[mtln]->map_d = Parsemap_d(line);  
            //} else if (line.find("Ns") != std::string::npos) {
            //        res[mtln]->Ns = ParseNs(line);  
            //} else if (line.find("Ni") != std::string::npos) {
            //        res[mtln]->Ni = ParseNi(line);  
            // } else if (line.find("d") != std::string::npos) {
            //         res[mtln]->Ni = Parsed(line);
            //} else if (line.find("Ka") != std::string::npos) {
            //        res[mtln]->Ka = ParseKa(line);  
            //} else if (!line.find("Kd") != std::string::npos) {
            //        res[mtln]->Kd = ParseKd(line);
            //} else if (line.find("Ks") != std::string::npos) {
            //        res[mtln]->Ks = ParseKs(line);
            //} else if (line.find("Ke") != std::string::npos) {
            //        res[mtln]->Ke = ParseKe(line);      
            //} else if (line.find("Tf") != std::string::npos) {
            //        res[mtln]->Tf = ParseTf(line);  
            //} else if (line.find("illum") != std::string::npos) {
            //        res[mtln]->illum = Parseillum(line);


            }
        }
    } else {
        printf ("Unable open MTL file %s\n", fname.c_str());
    } 

    tst.close();


    if (res.empty()) {
        printf ("Add empty material\n");
        res["default"] = (std::unique_ptr<CMTLMaterial>(new CMTLMaterial()));
    }
    return res;

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
        
        void BindTextures(std::shared_ptr<CObjSubmesh> submesh);
        /*Remorval?*/
        


        std::unique_ptr<CObjMeshParser> parser;

        std::unordered_map<std::string, unsigned int > submesh_vbo;
        std::unordered_map<std::string, unsigned int > submesh_vao;
        std::unordered_map<std::string, std::unique_ptr<STexture> > d_textures;
        std::unordered_map<std::string, std::unique_ptr<CMTLMaterial> > d_materials;

        bool flag_normals = false;
        
        friend class cereal::access;
        /*serialize support */
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( CEREAL_NVP(IsReady),
                CEREAL_NVP(model),
                CEREAL_NVP(d_materials),
                CEREAL_NVP(parser), //to big for output
                CEREAL_NVP(d_textures)
                );
        }




};
int SObjModel::ConfigureProgram(SShader& sprog){
    if (!IsReady)
        return EFAIL;
    sprog.Bind();
    for (auto it = parser->d_sm.begin(); it != parser->d_sm.end();++it) {
        auto &submesh =  (*it);
        glBindVertexArray( submesh_vao[(*it)->name] );
        glBindBuffer ( GL_ARRAY_BUFFER, submesh_vbo[(*it)->name] );

        if (flag_normals) {
            sprog.SetAttrib( "position", 3, sizeof(CObjVertexN), offsetof(CObjVertexN,p),GL_FLOAT);
            sprog.SetAttrib( "normal", 3, sizeof(CObjVertexN),  offsetof(CObjVertexN,n),GL_FLOAT);
            sprog.SetAttrib( "UV", 2, sizeof(CObjVertexN),  offsetof(CObjVertexN,tc),GL_FLOAT);
        } else {
            sprog.SetAttrib( "position", 3, sizeof(CObjVertex),  offsetof(CObjVertex,p),GL_FLOAT);
            sprog.SetAttrib( "normal", 3, sizeof(CObjVertex),  (0),GL_FLOAT);
            sprog.SetAttrib( "UV", 2, sizeof(CObjVertex),  offsetof(CObjVertex,tc),GL_FLOAT);
        }
        if (flag_normals)
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
    :parser(new CObjMeshParser(fname))
{
    if (!parser->IsReady)
        {
            //EMSGS(std::string("Unable open model :") + fname);
            std::cout <<   std::string("Unable open model :") << fname << std::endl;
        return;
        }
    /* load descriptor*/
    unsigned int temp_vao;
    unsigned int temp_vbo;
    if (parser->d_mtllibs.empty())
        d_materials = ParseMTL("default.mtl");
    else
        d_materials = ParseMTL(parser->d_mtllibs[0]);
    printf("materials file parsed\n");

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

    for (auto it = parser->d_sm.begin(); it != parser->d_sm.end();++it) {

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
        glGenVertexArrays ( 1, &temp_vao );
        glGenBuffers ( 1, &temp_vbo );
        glBindVertexArray(temp_vao);
        glBindBuffer(GL_ARRAY_BUFFER,temp_vbo);
        if (flag_normals)
            glBufferData ( GL_ARRAY_BUFFER, submesh->vn.size() *sizeof(CObjVertexN) , submesh->vn.data(), GL_STATIC_DRAW);
        else
            glBufferData ( GL_ARRAY_BUFFER, submesh->v.size() *sizeof(CObjVertex) , submesh->v.data(), GL_STATIC_DRAW);
     
        
        submesh_vbo [ submesh->name ] = temp_vbo;
        submesh_vao [submesh->name] = temp_vao; 


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
void SObjModel::BindTextures(std::shared_ptr<CObjSubmesh> submesh) {
    
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
        for (auto it = parser->d_sm.begin(); it != parser->d_sm.end();++it) {
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

            if (submesh->flag_normals)
               glDrawArrays(GL_TRIANGLES,0,submesh->vn.size());
            else
               glDrawArrays(GL_TRIANGLES,0,submesh->v.size());
            glBindVertexArray ( 0 );


        }
    }
}

SObjModel::~SObjModel() {
}
