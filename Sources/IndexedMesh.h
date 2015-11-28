#ifndef __INDEXEDMESH_H_
#define __INDEXEDMESH_H_
#pragma once
#include "ObjParser.h"
#include "MeshTypes.h"
class MeshIndexer {
public:
    MeshIndexer() {}
    IndexedMesh *IndexNonIndexedMesh(const NonIndexedMesh *submesh);

    bool IsReady = false;

    static int Reflect(ObjCtx *ctx);
    static void ReleaseContext(ObjCtx *ctx);
};
#endif
