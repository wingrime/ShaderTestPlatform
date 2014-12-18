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
    unsigned int id;/* number of submesh in obj*/
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

class MeshIndexer {
public:
    MeshIndexer(const std::shared_ptr<CObjSubmesh> &submesh)
        :d_inmesh(submesh)
    {}
    std::shared_ptr<CObjSubmesh> Do();
private:
    const std::shared_ptr<CObjSubmesh> d_inmesh;
};


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



        std::unordered_map<unsigned int , unsigned int > submesh_vbo;
        std::unordered_map<unsigned int , unsigned int > submesh_vao;
        std::unordered_map<unsigned int , unsigned int > submesh_ibo;
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
