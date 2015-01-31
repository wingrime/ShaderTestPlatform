#pragma once
#include <algorithm>
/*serialization*/
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>


#include "MTLParser.h"
#include "ObjParser.h"
#include "mat_math.h"


class RenderContext;
class SShader;
class STexture;

struct Material {
STexture * diffuse;
STexture *bump;
STexture *alpha;
std::size_t name_hash;

};
struct SubMeshIDs {
unsigned int vao;
unsigned int vbo;
unsigned int ibo;
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

        // ready flag
        bool IsReady = false;
        void SetModelMat(const SMat4x4& m);
    private:
        /*Utils*/
        void BindTextures(Material *m);
        void LoadTextures();
        void BindVAOs ();



        SMat4x4 model;
        // default textures for fail prove
        std::shared_ptr<STexture> texDiffuse;
        std::shared_ptr<STexture> texNormal;



        std::vector<std::shared_ptr<CObjSubmesh> > d_sm;



        std::unordered_map<unsigned int , SubMeshIDs > submesh_idx;

        std::unordered_map<std::string, std::unique_ptr<STexture> > d_textures;
        std::unordered_map<std::string, Material> d_materails;
        std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > d_materials;

        /*https://devtalk.nvidia.com/default/topic/561172/gldrawarrays-without-vao-for-procedural-geometry-using-gl_vertexid-doesn-t-work-in-debug-context/ */
        unsigned int d_emptyVAO;


        friend class cereal::access;
        /*serialize support */
        template <class Archive>
        void serialize( Archive & ar )
        {
            ar( CEREAL_NVP(IsReady),
                CEREAL_NVP(model),
                CEREAL_NVP(d_materials),
                CEREAL_NVP(d_textures),
                CEREAL_NVP(d_sm)
                );
        }




};
