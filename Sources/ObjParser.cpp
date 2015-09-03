#include "ObjParser.h"
#include "mat_math.h"
CObjVertexN CObjMeshParser::BuildVertN(const CObjV3 &p, const CObjV2 &tc,const CObjV3 &n) {
    CObjVertexN  v = { p , n , tc};
    return v;
}

std::vector<CObjVertexN> CObjMeshParser::BuildVerts(const std::vector<CObjV3> &glv,const std::vector<CObjV2> &glt , const CObjFaceI& face) {
    std::vector<CObjVertexN> res;
    CObjV3 normal = CalcNormal(glv[ face.f[0].v_idx - 1.0 ],glv[ face.f[1].v_idx - 1.0 ],glv[ face.f[2].v_idx - 1.0 ]);
    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
        CObjV2 empty_v2;

        if ( face.f[3].v_idx == 0 ) {
        // consider this is single triangle

            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        } else {
        // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        }
    } else {
                if ( face.f[3].v_idx == 0 ) {
        // consider this is single triangle
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal) );
        } else {
        // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  ,normal) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ,normal) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  ,normal) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ,normal) );
        }
    }
    return res;
}



std::vector<CObjVertexN> CObjMeshParser::BuildVertsN(const std::vector<CObjV3> &glv,const std::vector<CObjV2> &glt ,const std::vector<CObjV3> &gln, const CObjFaceI& face) {
    std::vector<CObjVertexN> res;

    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
            CObjV2 empty_v2;
        if ( face.f[3].v_idx == 0 ) {
            // consider this is single triangle
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2, gln[ face.f[0].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,empty_v2   , gln[ face.f[3].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }

    } else {
        if ( face.f[3].v_idx == 0 ) {
            // consider this is single triangle
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] , gln[ face.f[0].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res.push_back(BuildVertN(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  , gln[ face.f[3].n_idx - 1.0 ] ) );
            res.push_back(BuildVertN(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }
    }
    return res;
}

std::string CObjMeshParser::ParseG(const std::string &str) {
    char buf[100];
    sscanf(str.c_str(),"g %90s",buf);
    return std::string(buf);
}

std::string CObjMeshParser::ParseO(const std::string &str) {
    char buf[100];
    sscanf(str.c_str(),"o %90s",buf);
    return std::string(buf);
}

void CObjMeshParser::SortByMaterial()
{
    std::sort(d_sm.begin(), d_sm.end(), [](std::shared_ptr<CObjSubmesh> a , std::shared_ptr<CObjSubmesh>  b) -> bool  { return (a->m_name >  b->m_name);} );
}

CObjV3 CObjMeshParser::CalcNormal(const CObjV3& v1, const CObjV3& v2, const CObjV3& v3)
{

    CObjV3 e;

    SVec4 vec1(v1.x,v1.y,v1.z,0.0);
    SVec4 vec2(v2.x,v2.y,v2.z,0.0);
    SVec4 vec3(v3.x,v3.y,v3.z,0.0);
    /*normal direction depends on R L coordinate system*/
    SVec4 n_n = SVec4::Cross3((vec1 - vec2) ,(vec1 - vec3));

    if (n_n.Length() == 0 ) {
        /* normal is broken*/
        e.x = 0;
        e.y = 0;
        e.z = 1.0;
        return e;

    }
    SVec4 n = SVec4::Normalize( n_n );
    e.x = n.x;
    e.y = n.y;
    e.z = n.z;
    return e;

}
std::string CObjMeshParser::ParseUSEMTL(const std::string &str) {
    char buf[100];
    sscanf(str.c_str(),"usemtl %90s",buf);
    return std::string(buf);
}
std::string CObjMeshParser::ParseMTLLIB(const std::string &str) {
    char buf[100];
    sscanf(str.c_str(),"mtllib %90s",buf);
    return std::string(buf);
}
// TODO: optimize
CObjFaceI CObjMeshParser::ParseF(const std::string &v_desc) {
    CObjFaceI obj ;

    for (int i = 0 ; i < 4; i++) {
        obj.f[i].n_idx = 0;
        obj.f[i].v_idx = 0;
        obj.f[i].vt_idx = 0;
    }


    if (v_desc.find("//") != std::string::npos)  /* vector//normal format  - no texture coordiates */ {

        int ret = sscanf(v_desc.c_str(),"f %d//%d %d//%d %d//%d %d//%d"
            , &obj.f[0].v_idx,  &obj.f[0].n_idx
            , &obj.f[1].v_idx,  &obj.f[1].n_idx
            , &obj.f[2].v_idx,  &obj.f[2].n_idx
            , &obj.f[3].v_idx,  &obj.f[3].n_idx);

            obj.f[0].vt_idx  = 0;
            obj.f[1].vt_idx  = 0;
            obj.f[2].vt_idx  = 0;
            obj.f[3].vt_idx  = 0;

           if (ret != 6) { /*in triangle case should be 6 parsable otherwice 8*/
             ret = sscanf(v_desc.c_str(),"f %d//%d %d//%d %d//%d"
                , &obj.f[0].v_idx , &obj.f[0].n_idx
                , &obj.f[1].v_idx , &obj.f[1].n_idx
                , &obj.f[2].v_idx , &obj.f[2].n_idx
                );
            }
           return obj;
    }


    int ret = sscanf(v_desc.c_str(),"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d"
            , &obj.f[0].v_idx , &obj.f[0].vt_idx , &obj.f[0].n_idx
            , &obj.f[1].v_idx , &obj.f[1].vt_idx , &obj.f[1].n_idx
            , &obj.f[2].v_idx , &obj.f[2].vt_idx , &obj.f[2].n_idx
            , &obj.f[3].v_idx , &obj.f[3].vt_idx , &obj.f[3].n_idx);

    if (ret != 9) {

    ret = sscanf(v_desc.c_str(),"f %d/%d %d/%d %d/%d %d/%d"
            , &obj.f[0].v_idx , &obj.f[0].vt_idx
            , &obj.f[1].v_idx , &obj.f[1].vt_idx
            , &obj.f[2].v_idx , &obj.f[2].vt_idx
            , &obj.f[3].v_idx , &obj.f[3].vt_idx );
    }
    return obj;
}

CObjV2 CObjMeshParser::ParseVt(const std::string &v_desc) {

    CObjV2 obj;
    float tmp;
    sscanf(v_desc.c_str(),"vt %f %f %f ", &obj.u, &obj.v, &tmp);
    return obj;
}

CObjV3 CObjMeshParser::ParseV(const std::string &v_desc) {

    CObjV3 obj;
    sscanf(v_desc.c_str(),"v %f %f %f", &obj.x, &obj.y, &obj.z);
    return obj;
}
CObjV3 CObjMeshParser::ParseVn(const std::string &v_desc) {

    CObjV3 obj;
    sscanf(v_desc.c_str(),"vn %f %f %f", &obj.x, &obj.y, &obj.z);
    return obj;
}

void CObjMeshParser::Reflect() {
    printf("Reflect for CObjMeshParser\n");
    printf("Total vertexes %lu\n",getVertexCount());
    for (auto it = d_sm.begin(); it != d_sm.end();++it) {
        printf("SubMesh name=%s m_name=%s c =%d \n", (*it)->name.c_str() ,  (*it)->m_name.c_str() ,(*it)->vn.size() );

           // for (std::vector<CObjVertex>::iterator it2 = (*it)->v.begin(); it2 != (*it)->v.end();++it2) {
           // auto &v = (*it2);
           // printf("%f %f %f : %f %f \n",  v.p.x, v.p.y, v.p.z, v.tc.u, v.tc.v);
            //}


    }
}


CObjMeshParser::CObjMeshParser(const std::string& fname)
 :d_vertex_count(0)
 {
    std::ifstream tst;
    unsigned int sm_id = 0 ;
    std::vector<CObjV3> d_glv; /*list of vectors*/
    std::vector<CObjV2> d_gltc; /*list of texture coordinates*/
    std::vector<CObjV3> d_gln; /*list of normals*/
    bool d_normals = false; /*is normals used */
    tst.open(fname);
    std::string dir;
    const size_t last_fslash_i = fname.rfind('/');
    //const size_t last_lslash_i = fname.rfind('\\');
    //const size_t last_slash = std::max(last_fslash_i,last_lslash_i);
    if (std::string::npos != last_fslash_i ) {
        dir = fname.substr(0,last_fslash_i) + '\\';
    }
    if (tst) {

        for( std::string line; getline( tst, line ); ) {

            auto &subm =  (d_sm.back());
            if (!line.find("vt") ){
                d_gltc.push_back(ParseVt(line));
            } else if (!line.find("vn")) {
                d_normals = true;
                d_gln.push_back(ParseVn(line));
            } else if (!line.find("o")) {
                    /*begin new submesh*/
                    std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                    s->name = ParseO(line);
                    s->id  = sm_id++;
                    s->m_dir = dir;
                    d_sm.push_back(s);

            } else if (!line.find("s")) {
                //TODO
            } else if (!line.find("usemtl")) {
                const std::string& mtl = ParseUSEMTL(line);
                /* sometimes can use different materials for single submesh*/
                if (d_sm.empty() || !subm->m_name.empty()) {
                    /*begin new submesh*/
                    std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                    if (d_sm.empty()) /*for files divieds complitly with materals and  without other dividers*/
                        s->name = std::string("no_submesh_mat_") + mtl;
                    else
                        s->name = subm->name + std::string("_mat_") + mtl;
                    s->m_name = mtl;
                    s->m_dir = dir;
                    s->id  = sm_id++;
                    d_sm.push_back(s);
                } else
                    subm->m_name = mtl;

            } else if (!line.find("mtllib")) {
                d_mtllibs.push_back(dir+ParseMTLLIB(line));
            } else if (!line.find("f")) {
                CObjFaceI fi = ParseF(line);
                if (d_normals){
                    std::vector<CObjVertexN> a_v = BuildVertsN(d_glv,d_gltc,d_gln,fi);
                    std::vector<CObjVertexN> &o_v = subm->vn;
                    d_vertex_count += a_v.size();
                    o_v.insert(o_v.end(), a_v.begin(), a_v.end());
                } else {
                    std::vector<CObjVertexN> a_v = BuildVerts(d_glv,d_gltc,fi);
                    d_vertex_count += a_v.size();
                    std::vector<CObjVertexN> &o_v = subm->vn;
                    o_v.insert(o_v.end(), a_v.begin(), a_v.end());
                }
            } else if (!line.find("v")) {
                d_glv.push_back(ParseV(line));
            } else if (!line.find("g")) {
                std::shared_ptr<CObjSubmesh> s(new CObjSubmesh());
                s->id  = sm_id++;
                s->name = ParseG(line);
                s->m_dir = dir;
                d_sm.push_back(s);

                //d_normals = true;
            } else if (!line.find("#")) {
                // comment
            } else if (!line.find(" ")) {
                // empty line with space
            } else if (line.empty()) {
                // empty line
            } else {
                std::cout << "Unrecogized OBJ tag: "<< line << " while parsing:"   << fname << std::endl;
            };
        }
    } else  {
        std::cout << "File Open failed!" << std::endl;
        return;
    }
    tst.close();
    SortByMaterial();
    IsReady = true;
}

