#pragma once

#include "r_context.h"
#include "MTLParser.h"

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


#include "ObjParser.h"

struct Material {
STexture * diffuse;
STexture *bump;
STexture * alpha;

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
        void BindTextures(const std::shared_ptr<CObjSubmesh> &submesh);
        void LoadTextures();
        void BindVAOs ();



        SMat4x4 model;
        // default textures for fail prove
        std::shared_ptr<STexture> texDiffuse;
        std::shared_ptr<STexture> texNormal;



        std::vector<std::shared_ptr<CObjSubmesh> > d_sm;



        std::unordered_map<unsigned int , unsigned int > submesh_vbo;
        std::unordered_map<unsigned int , unsigned int > submesh_vao;
        std::unordered_map<unsigned int , unsigned int > submesh_ibo;
        std::unordered_map<std::string, std::unique_ptr<STexture> > d_textures;
        std::unordered_map<std::string, Material> d_textures_loaded;
        std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > d_materials;

        
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
