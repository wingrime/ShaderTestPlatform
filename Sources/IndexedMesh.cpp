#include "IndexedMesh.h"
#include "string_format.h"
#include "Log.h"

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
namespace std
{
    template <>
    struct hash<UVNVertex>
    {
        size_t operator()(const UVNVertex& k) const
        {
            // Compute individual hash values for two data members and combine them using XOR and bit shifting
            //return ((hash<float>()(k.n.x) ^ (hash<float>()(k.n.y) << 1)  ) >> 1);
            std::size_t seed = 0;
            hash_combine(seed, k.n.x);
            hash_combine(seed, k.n.y);
            hash_combine(seed, k.n.z);
            hash_combine(seed, k.p.x);
            hash_combine(seed, k.p.y);
            hash_combine(seed, k.p.z);
            hash_combine(seed, k.tc.u);
            hash_combine(seed, k.tc.v);
            return seed;
        }
    };
}
IndexedMesh *MeshIndexer::IndexNonIndexedMesh(const NonIndexedMesh *submesh)
{
    LOGV(string_format("Indexing submesh name=%s,m_name=%s, id= %d,triangles=%d ",submesh->name.c_str(),submesh->m_name.c_str(),submesh->id, submesh->vn.size()));
    IndexedMesh* mesh =  new IndexedMesh();
    mesh->m_name = submesh->m_name;
    mesh->name = submesh->name;
    mesh->id = submesh->id;
    mesh->m_dir = submesh->m_dir;
    unsigned int current_index = 0;

    std::unordered_map<UVNVertex, unsigned int  > vn_map;

    for( auto it = std::begin(submesh->vn); it != std::end(submesh->vn); ++it) {
        auto k = vn_map.find(*it);
        if (k == vn_map.end()) {
        /* if there is no index, then add new index*/
            vn_map[*it] = current_index;
            mesh->vn.push_back(*it);
            mesh->indexes.push_back(current_index);
            current_index++;
        } else {
            /*if it is in index*/
            mesh->indexes.push_back(vn_map[*it]);
        }

    }
    LOGV(string_format("Indexing result: indepened_triangles=%d",mesh->vn.size()) );
    return mesh;

}
