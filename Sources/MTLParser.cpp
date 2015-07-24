#include "MTLParser.h"
#include "string_format.h"
#include "Log.h"
#include <fstream>
/*serialization*/
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
MTLParser::MTLParser(const std::string& fname) {
    std::ifstream tst;
    std::hash<std::string> hash_fn;
    std::string mtln("default_mtl");

    tst.open(fname);
    if (tst) {
        for( std::string line; getline( tst, line ); ) {
            if (!line.find("newmtl") ){
                    mtln = ParseNEWMTL(line);
                    if (d_materials.count(mtln) == 0 ) {/* all materials are unique*/
                        d_materials[mtln] = std::unique_ptr<SMaterial>(new SMaterial());
                        d_materials[mtln]->name_hash = hash_fn(mtln);
                    }
            } else if (line.find("map_Ka" ) != std::string::npos) {
                    d_materials[mtln]->map_Ka = Parsemap_Ka(line);
            } else if (line.find("map_Kd") != std::string::npos) {
                    d_materials[mtln]->albedoTexFileName = Parsemap_Kd(line);
            } else if (line.find("map_bump")!= std::string::npos ) {
                    d_materials[mtln]->bumpMapTexFileName = Parsemap_bump(line);
            } else if (line.find("map_d")!= std::string::npos ) {
                    d_materials[mtln]->alphaMaskTexFileName = Parsemap_d(line);

            }
        }
    } else {
        LOGE(string_format("Unable open MTL file %s\n", fname.c_str()));
    }

    tst.close();


    if (d_materials.empty()) {
         LOGE("Add empty default material\n");
        d_materials["default"] = (std::unique_ptr<SMaterial>(new SMaterial()));
    }


}

std::unordered_map<std::string, std::shared_ptr<SMaterial> > &MTLParser::GetMaterials()
{
    return d_materials;
}

int MTLParser::SaveToJSON(const std::string& filejs)
{
    std::ofstream os(filejs);
   {
       /*use raii */
       cereal::JSONOutputArchive archive( os);
       archive( CEREAL_NVP( d_materials));
   }
    return 0;
}
std::string MTLParser::ParseNEWMTL(const std::string& str) {
    char buf[90];
    sscanf(str.c_str(),"newmtl %80s",buf);
    return std::string(buf);
}
float MTLParser::Parsed(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," d %f", &r);
    return r;
}

std::string MTLParser::Parsemap_Ka(const std::string& str) {
    char buf[90];
    sscanf(str.c_str()," map_Ka %80s",buf);
    return std::string(buf);
}
std::string  MTLParser::Parsemap_Kd(const std::string& str) {
    char buf[90];
    sscanf(str.c_str()," map_Kd %80s",buf);
    return std::string(buf);
}
std::string MTLParser::Parsemap_d(const std::string& str) {
    char buf[90];
    sscanf(str.c_str()," map_d %80s",buf);
    return std::string(buf);
}
std::string MTLParser::Parsemap_bump(const std::string& str) {
    char buf[90];

    sscanf(str.c_str()," map_bump %80s",buf);
    return std::string(buf);
}
