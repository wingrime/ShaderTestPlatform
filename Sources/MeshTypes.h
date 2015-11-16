#ifndef __MESHTYPES_H_
#define __MESHTYPES_H_
#pragma once
#include <string>
#include <vector>

#include "mat_math.h"
struct UVNVertex {
    vec3 p;
    vec3 n;
    vec2 tc;
    bool operator==(const UVNVertex& o) const {
        if (p != o.p )
            return false;
        if (n != o.n )
            return false;
        if (tc != o.tc )
            return false;
         return true;
    }

};



struct NonIndexedMesh {
    std::string name; /* submesh name*/
    std::string m_name; /* material name*/
    std::string m_dir; /* model working dir*/
    std::vector<UVNVertex> vn; /* vertex list with normals*/
    unsigned int id;/* number of submesh in obj*/
};


struct IndexedMesh {
    std::string name; /* submesh name*/
    std::string m_name; /* material name*/
    std::string m_dir; /* model working dir*/
    std::vector<UVNVertex> vn; /* vertex list with normals*/
    std::vector<unsigned int> indexes; /*indexed mesh*/
    unsigned int id;/* number of submesh in obj*/
};


#endif
