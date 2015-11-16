#pragma once
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <fstream>
#ifdef OBJPARSER_SERIALIZE
/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#endif
#include "mat_math.h"
#include "MeshTypes.h"

    //bool operator<(const vec3& p) const {
    //    if (x != p.x )
    //        return (x < p.x);
    //    if (y != p.y )
    //        return (y < p.y);

    //     return (z < p.z);
//}

struct CVertexIdx {
    int v_idx;
    int vt_idx;
    int n_idx;
};
struct CObjFace {
    CVertexIdx  f [4];
};



struct ObjCtx{
    std::string srcFileName;
    std::vector<NonIndexedMesh *> subMeshSet;
    std::vector<std::string > refMaterialFiles;
};

class CObjMeshParser {
public:
    CObjMeshParser();
    bool IsReady = false;

    ObjCtx* ParseFromFile(const std::string &fname);
    static int Reflect(ObjCtx *ctx);
    static void ReleaseContext(ObjCtx *ctx);
    static void SortByMaterial(ObjCtx *ctx);

private:

    int BuildVerts(const std::vector<vec3> &glv, const std::vector<vec2> &glt , const CObjFace& face, std::vector<UVNVertex> *res);
    int BuildVertsN(const std::vector<vec3> &glv, const std::vector<vec2> &glt, const std::vector<vec3> &gln , const CObjFace& face, std::vector<UVNVertex> *res);
    bool SanitizeFace(const CObjFace& face, int vertexCount, int normalCount, int txCount);
    UVNVertex BuildVert(const vec3 &p, const vec2 &tc, const vec3 &n);
    std::string ParseG(const std::string& str);
    std::string ParseUSEMTL(const std::string& str);
    std::string ParseMTLLIB(const std::string& str);
    CObjFace ParseF(const std::string& v_desc);
    vec2 ParseVt(const std::string& v_desc);
    vec3 ParseV(const std::string& v_desc);
    vec3 ParseVn(const std::string& v_desc);
    std::string ParseO(const std::string& str);

    vec3 CalcNormal(const vec3 &v1, const vec3 &v2, const vec3 &v3);

#ifdef OBJPARSER_SERIALIZE
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
      //  ar(CEREAL_NVP(d_sm),CEREAL_NVP(d_mtllibs),CEREAL_NVP(d_vertex_count));
    }
#endif

};
