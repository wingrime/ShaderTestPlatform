#ifndef MTLPARSER_H
#define MTLPARSER_H
#pragma once
#include <string>
#include <unordered_map>
#include <cereal/access.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>

class STexture;

struct SMaterial {
    /*name hash for fast comparsion*/
    std::size_t name_hash;


    std::string map_Ka; /*ambient map */ /* OBSOLUTE*/
    std::string albedoTexFileName; /*diffuse map*/
    std::string alphaMaskTexFileName; /*alpha mask*/
    std::string bumpMapTexFileName; /*bump */

    STexture * albedoTex;
    STexture *bumpMapTex;
    STexture *alphaMaskTex;


    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(name_hash),CEREAL_NVP(map_Ka),CEREAL_NVP(albedoTexFileName),CEREAL_NVP(alphaMaskTexFileName),CEREAL_NVP(bumpMapTexFileName));
    }
};

class MTLParser {
public:
     MTLParser(const std::string& fname);
     std::unordered_map<std::string, SMaterial > &GetMaterials();
     int SaveToJSON(const std::string &filejs);

private:
    std::unordered_map<std::string, SMaterial > d_materials;
    float Parsed(const std::string &v_desc);
     std::string ParseNEWMTL(const std::string &str);
     float ParseNs(const std::string &v_desc);
     float ParseNi(const std::string &v_desc);
     int Parseillum(const std::string &v_desc);
     std::string Parsemap_bump(const std::string &str);
     std::string Parsemap_d(const std::string &str);
      std::string Parsemap_Ka(const std::string &str);
     std::string Parsemap_Kd(const std::string &str);
};

#endif // MTLPARSER_H
