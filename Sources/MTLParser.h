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

struct CMTLColor {
    float r;
    float g;
    float b;
    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(r),CEREAL_NVP(g),CEREAL_NVP(b));
    }
};

struct CMTLMaterial {
    /*name hash for fast comparsion*/
    std::size_t name_hash;


    std::string map_Ka; /*ambient map */
    std::string map_Kd; /*diffuse map*/
    std::string map_d; /*alpha mask*/
    std::string map_bump; /*bump */

    /*serialize support */
    friend class cereal::access;
    template <class Archive>
    void serialize( Archive & ar )
    {
        ar(CEREAL_NVP(map_Ka),CEREAL_NVP(map_Kd),CEREAL_NVP(map_d),CEREAL_NVP(map_bump));
    }
};

class MTLParser {
public:
     MTLParser(const std::string& fname);
     std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > &GetMaterials();
     int SaveToJSON(const std::string &filejs);

private:
    std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > d_materials;
    float Parsed(const std::string &v_desc);
     std::string ParseNEWMTL(const std::string &str);
     float ParseNs(const std::string &v_desc);
     float ParseNi(const std::string &v_desc);
     int Parseillum(const std::string &v_desc);
     CMTLColor ParseKa(const std::string &v_desc);
     std::string Parsemap_bump(const std::string &str);
     CMTLColor ParseKd(const std::string &v_desc);
     std::string Parsemap_d(const std::string &str);
     CMTLColor ParseKs(const std::string &v_desc);
     CMTLColor ParseKe(const std::string &v_desc);
     CMTLColor ParseTf(const std::string &v_desc);
     std::string Parsemap_Ka(const std::string &str);
     std::string Parsemap_Kd(const std::string &str);
};

#endif // MTLPARSER_H
