#include "MTLParser.h"
#include "string_format.h"
#include "Log.h"
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
                        d_materials[mtln] = std::unique_ptr<CMTLMaterial>(new CMTLMaterial());
                        d_materials[mtln]->name_hash = hash_fn(mtln);
                    }
            } else if (line.find("map_Ka" ) != std::string::npos) {
                    d_materials[mtln]->map_Ka = Parsemap_Ka(line);
            } else if (line.find("map_Kd") != std::string::npos) {
                    d_materials[mtln]->map_Kd = Parsemap_Kd(line);
            } else if (line.find("map_bump")!= std::string::npos ) {
                    d_materials[mtln]->map_bump = Parsemap_bump(line);
            } else if (line.find("map_d")!= std::string::npos ) {
                    d_materials[mtln]->map_d = Parsemap_d(line);
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
        LOGE(string_format("Unable open MTL file %s\n", fname.c_str()));
    }

    tst.close();


    if (d_materials.empty()) {
         LOGE("Add empty default material\n");
        d_materials["default"] = (std::unique_ptr<CMTLMaterial>(new CMTLMaterial()));
    }


}

std::unordered_map<std::string, std::shared_ptr<CMTLMaterial> > &MTLParser::GetMaterials()
{
    return d_materials;
}
std::string MTLParser::ParseNEWMTL(const std::string& str) {
    char buf[90];
    sscanf(str.c_str(),"newmtl %80s",buf);
    return std::string(buf);
}

float MTLParser::ParseNs(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," Ns %f", &r);
    return r;
}
float MTLParser::ParseNi(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," Ni %f", &r);
    return r;
}
float MTLParser::Parsed(const std::string& v_desc) {
    float r = 0.0;
    sscanf(v_desc.c_str()," d %f", &r);
    return r;
}

int MTLParser::Parseillum(const std::string& v_desc) {
    int r = 0;
    sscanf(v_desc.c_str()," illum %d", &r);
    return r;
}

CMTLColor MTLParser::ParseKa(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ka %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor MTLParser::ParseKd(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Kd %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor MTLParser::ParseKs(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ks %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor MTLParser::ParseKe(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Ke %f %f %f", &c.r, &c.g, &c.b);
    return c;
}
CMTLColor MTLParser::ParseTf(const std::string& v_desc) {
    CMTLColor c;
    sscanf(v_desc.c_str()," Tf %f %f %f", &c.r, &c.g, &c.b);
    return c;
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
