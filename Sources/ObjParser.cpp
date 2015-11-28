#include "ObjParser.h"
#include "mat_math.h"
UVNVertex inline CObjMeshParser::BuildVert(const vec3 &p, const vec2 &tc, const vec3 &n) {
    UVNVertex  v = { p , n , tc};
    return v;
}

int CObjMeshParser::BuildVerts(const std::vector<vec3> &glv, const std::vector<vec2> &glt , const CObjFace& face,std::vector<UVNVertex>* res) {
    //std::vector<CObjVertex> res;
    vec3 normal = CalcNormal(glv[ face.f[0].v_idx - 1.0 ],glv[ face.f[1].v_idx - 1.0 ],glv[ face.f[2].v_idx - 1.0 ]);
    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
        vec2 empty_v2;

        if ( face.f[3].v_idx == 0 ) {
        // consider this is single triangle
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        } else {
        // consider this is face
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );

            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res->push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  , empty_v2 ,normal ) );
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  , empty_v2 ,normal ) );
        }
    } else {
                if ( face.f[3].v_idx == 0 ) {
        // consider this is single triangle
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] ,normal ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal) );
        } else {
        // consider this is face
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] ,normal ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  ,normal) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  ,normal) );

            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] ,normal ) );
            res->push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  ,normal) );
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  ,normal) );
        }
    }
    return 0;
}


int CObjMeshParser::BuildVertsN(const std::vector<vec3> &glv, const std::vector<vec2> &glt , const std::vector<vec3> &gln, const CObjFace& face,std::vector<UVNVertex>* res) {
    //std::vector<CObjVertex> res;

    if (face.f[0].vt_idx == 0) /* no texture coords case*/  {
            vec2 empty_v2;
        if ( face.f[3].v_idx == 0 ) {
            // consider this is single triangle
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2, gln[ face.f[0].n_idx - 1.0 ]  ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,empty_v2 , gln[ face.f[2].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  ,empty_v2   , gln[ face.f[3].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,empty_v2  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }

    } else {
        if ( face.f[3].v_idx == 0 ) {
            // consider this is single triangle
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ] , gln[ face.f[0].n_idx - 1.0 ]  ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ] , gln[ face.f[1].n_idx - 1.0 ]  ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ] , gln[ face.f[2].n_idx - 1.0 ]  ) );
        } else {
            // consider this is face
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[1].v_idx - 1.0 ]  ,glt[ face.f[1].vt_idx - 1.0 ]  , gln[ face.f[1].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );

            res->push_back(BuildVert(glv[ face.f[2].v_idx - 1.0 ]  ,glt[ face.f[2].vt_idx - 1.0 ]  , gln[ face.f[2].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[3].v_idx - 1.0 ]  ,glt[ face.f[3].vt_idx - 1.0 ]  , gln[ face.f[3].n_idx - 1.0 ] ) );
            res->push_back(BuildVert(glv[ face.f[0].v_idx - 1.0 ]  ,glt[ face.f[0].vt_idx - 1.0 ]  , gln[ face.f[0].n_idx - 1.0 ] ) );
        }
    }
    return 0;
}

bool CObjMeshParser::SanitizeFace(const CObjFace &face, int vertexCount, int normalCount, int txCount)
{
    //less than 0
    for (int i = 0 ; i < 3 ; i++ ) {
        if (face.f[i].n_idx < 0) return false;
        if (face.f[i].vt_idx < 0) return false;
        if (face.f[i].v_idx < 0) return false;
    }
    for (int i = 0 ; i < 3 ; i++ ) {
        if (face.f[i].n_idx > normalCount) return false;
        if (face.f[i].vt_idx > txCount) return false;
        if (face.f[i].v_idx > vertexCount) return false;
    }

    return true;
}

std::string CObjMeshParser::ParseG(const std::string &str) {
    //sscanf(str.c_str(),"g %90s",buf);
    const char * in_buf = str.c_str();
    const char * rw = in_buf+2;
    while (*in_buf != 0 && in_buf != rw ){in_buf++;}
    while (isblank(*in_buf) && *in_buf != 0 ){in_buf++;}
    return std::string(in_buf);
}

std::string CObjMeshParser::ParseO(const std::string &str) {
    //sscanf(str.c_str(),"o %90s",buf);
    const char * in_buf = str.c_str();
    const char * rw = in_buf+2;
    while (*in_buf != 0 && in_buf != rw ){in_buf++;}
    while (isblank(*in_buf) && *in_buf != 0 ){in_buf++;}
    return std::string(in_buf);
}

//void CObjMeshParser::SortByMaterial()
//{
//
//}

vec3 CObjMeshParser::CalcNormal(const vec3& v1, const vec3& v2, const vec3& v3)
{
    /*normal direction depends on R L coordinate system*/
    vec3 n_n = vec3::cross(v1 - v2 ,v1 - v3);
    if (vec3::length(n_n )== 0 ) {
        /* normal is broken*/
        return vec3(0.0,0.0,1.0);
    }
    return vec3::normalize( n_n );

}
std::string CObjMeshParser::ParseUSEMTL(const std::string &str) {
    const char *in_buf = str.c_str();
    const char * rw = in_buf+7;
    while (*in_buf != 0 && in_buf != rw ){in_buf++;}
    while (isblank(*in_buf) && *in_buf != 0 ){in_buf++;}
    return std::string(in_buf);
}
std::string CObjMeshParser::ParseMTLLIB(const std::string &str) {
    const char *in_buf = str.c_str();
    const char * rw = in_buf+7;
    while (*in_buf != 0 && in_buf != rw ){in_buf++;}
    while (isblank(*in_buf) && *in_buf != 0 ){in_buf++;}
    return std::string(in_buf);
}
// TODO: optimize
CObjFace CObjMeshParser::ParseF(const std::string &v_desc) {
    CObjFace obj ;

    for (int i = 0 ; i < 4; i++) {
        obj.f[i].n_idx = 0;
        obj.f[i].v_idx = 0;
        obj.f[i].vt_idx = 0;
    }
    const char * s = v_desc.c_str();
    const char * rw = s+2;
    while (*s != 0 && s != rw ) {s++;} //fast forward 2 chars
    int c = 0;
    while ( *s != 0 && c < 4) {

        while (*s != 0 && isblank(*s) ) {s++;} //forward spaces
        obj.f[c].v_idx = atoi(s);
        while (*s != 0 && isdigit(*s) ) {s++;} //forward numbers
        rw = s+1;
        while (*s != 0 && s != rw ) {s++;} //fast forward '/'
        if (isdigit(*s)) { // number or '/'
            obj.f[c].vt_idx = atoi(s);
            while (*s != 0 && isdigit(*s) ) {s++;} //forward numbers
        }
        if (*s == '/') {
            rw = s+1;
            while (*s != 0 && s != rw ) {s++;} //fast forward '/'
            if (isdigit(*s)) {
                obj.f[c].n_idx = atoi(s);
                while (*s != 0 && isdigit(*s) ) {s++;} //forward numbers
            }
        }
        c++;
    }
    return obj;
}


vec2 CObjMeshParser::ParseVt(const std::string &v_desc) {

    vec2 obj;
    //float tmp;
    //sscanf(v_desc.c_str(),"vt %f %f %f ", &obj.u, &obj.v, &tmp);
    const char * s = v_desc.c_str();
    const char * rw = s+3;
    while (*s != 0 && s != rw ) {s++;} //fast forward
    while (*s != 0 && isblank(*s) ) {s++;} //forward value
    obj.u = atof(s);
    while (*s != 0 && !isblank(*s) ) {s++;} //forward nonblank (value)
    while (*s != 0 && isblank(*s) ) {s++;} //forward spaces then
    obj.v = atof(s);
    return obj;
}

vec3 CObjMeshParser::ParseV(const std::string &v_desc) {

    vec3 obj;
    //sscanf(v_desc.c_str(),"v %f %f %f", &obj.x, &obj.y, &obj.z);
    const char * s = v_desc.c_str();
    const char * rw = s+2;
    while (*s != 0 && s != rw ) {s++;} //fast forward
    while (*s != 0 && isblank(*s) ) {s++;} //forward value
    obj.x = atof(s);
    while (*s != 0 && !isblank(*s) ) {s++;} //forward nonblank (value)
    while (*s != 0 && isblank(*s) ) {s++;} //forward spaces then
    obj.y = atof(s);
    while (*s != 0 && !isblank(*s) ) {s++;} //forward nonblank (value)
    while (*s != 0 && isblank(*s) ) {s++;} //forward spaces then
    obj.z = atof(s);



    return obj;
}
vec3 CObjMeshParser::ParseVn(const std::string &v_desc) {

    vec3 obj;
    //sscanf(v_desc.c_str(),"vn %f %f %f", &obj.x, &obj.y, &obj.z);
    const char * s = v_desc.c_str();
    const char * rw = s+3;
    while (*s != 0 && s != rw ) {s++;} //fast forward
    while (*s != 0 && isblank(*s) ) {s++;} //forward value
    obj.x = atof(s);
    while (*s != 0 && !isblank(*s) ) {s++;} //forward nonblank (value)
    while (*s != 0 && isblank(*s) ) {s++;} //forward spaces then
    obj.y = atof(s);
    while (*s != 0 && !isblank(*s) ) {s++;} //forward nonblank (value)
    while (*s != 0 && isblank(*s) ) {s++;} //forward spaces then
    obj.z = atof(s);
    return obj;
}



int CObjMeshParser::Reflect(ObjCtx *ctx) {
    printf("Reflect CObjMeshParser\n");
    //.printf("Total vertexes %lu\n",getVertexCount());
    for (auto it = ctx->subMeshSet.begin(); it != ctx->subMeshSet.end();++it) {
        printf("SubMeshName=%s MaterialName=%s VertexCount=%d \n v:x      v:y      v:z      n:x       n:y      n:z     v:u     v:v \n", (*it)->name.c_str() ,  (*it)->m_name.c_str() ,(*it)->vn.size() );
           for (std::vector<UVNVertex>::iterator it2 = (*it)->vn.begin(); it2 != (*it)->vn.end();++it2) {
                auto &v = (*it2);
               printf("% 6.4f % 6.4f % 6.4f % 6.4f % 6.4f % 6.4f : % 6.4f % 6.4f \n",  v.p.x, v.p.y, v.p.z,v.n.x, v.n.y, v.n.z, v.tc.u, v.tc.v);
           }
    }
    return 0;
}

void CObjMeshParser::ReleaseContext(ObjCtx *ctx)
{

    for (auto it = ctx->subMeshSet.begin(); it != ctx->subMeshSet.end();it++) {
        delete (*it);
    }
    delete ctx;
}

void CObjMeshParser::SortByMaterial(ObjCtx *ctx)
{
std::sort(ctx->subMeshSet.begin(), ctx->subMeshSet.end(), [](NonIndexedMesh * a , NonIndexedMesh *  b) -> bool  { return (a->m_name >  b->m_name);} );
}

ObjCtx *CObjMeshParser::ParseFromFile(const std::string &fname)
{
    std::ifstream tst;
    unsigned int sm_id = 0 ;
    ObjCtx *ctx = new ObjCtx();
    ctx->srcFileName = fname;

    std::vector<vec3> d_glv; /*list of vectors*/
    std::vector<vec2> d_gltc; /*list of texture coordinates*/
    std::vector<vec3> d_gln; /*list of normals*/
    bool d_normals = false; /*is normals used */
    tst.open(fname);
    std::string dir;
    const size_t last_fslash_i = fname.rfind('/');
    if (std::string::npos != last_fslash_i ) {
        dir = fname.substr(0,last_fslash_i) + '\\';
    }
    if (tst) {

        for( std::string li; getline( tst, li ); ) {
            const char *in_buf = li.c_str();
            /*remove leading spaces*/
            while (isblank(*in_buf) && *in_buf != 0 ){in_buf++;}
            std::string line(in_buf);
            NonIndexedMesh *subm = 0 ;
            if (!ctx->subMeshSet.empty()) {
                 subm = ctx->subMeshSet.back();
            }

            if (!line.find("vt")){
                d_gltc.push_back(ParseVt(line));
            } else if (!line.find("vn")) {
                d_normals = true;
                d_gln.push_back(ParseVn(line));
            } else if (!line.find("o")) {
                    /*begin new submesh*/
                    NonIndexedMesh * s = new NonIndexedMesh();
                    s->name = ParseO(line);
                    s->id  = sm_id++;
                    s->m_dir = dir;
                    ctx->subMeshSet.push_back(s);

            } else if (!line.find("s")) {
                //TODO
            } else if (line.find("usemtl")!=  std::string::npos) {
                const std::string& mtl = ParseUSEMTL(line);
                /* sometimes can use different materials for single submesh*/
                if (ctx->subMeshSet.empty() || !subm->m_name.empty()) {
                    /*begin new submesh*/
                    NonIndexedMesh * s = new NonIndexedMesh();
                    if (ctx->subMeshSet.empty()) /*for files divieds complitly with materals and  without other dividers*/
                        s->name = std::string("no_submesh_mat_") + mtl;
                    else
                        s->name = subm->name + std::string("_mat_") + mtl;
                    s->m_name = mtl;
                    s->m_dir = dir;
                    s->id  = sm_id++;
                    ctx->subMeshSet.push_back(s);
                } else
                    subm->m_name = mtl;

            } else if (line.find("mtllib") !=  std::string::npos) {
                ctx->refMaterialFiles.push_back(dir+ParseMTLLIB(line));
            } else if (!line.find("f") ) {
                CObjFace fi = ParseF(line);
                    /*create default object in case where we not meet o tag*/
                if (!subm) {
                    subm = new NonIndexedMesh();
                    subm->name = fname;
                    subm->id  = sm_id++;
                    subm->m_dir = dir;
                    ctx->subMeshSet.push_back(subm);
                }
                // TODO Fixup negative indexes
                if (SanitizeFace(fi,d_glv.size(),d_gln.size(),d_gltc.size()))
                {
                    if (d_normals){
                        std::vector<UVNVertex> &o_v = subm->vn;
                        BuildVertsN(d_glv,d_gltc,d_gln,fi,&o_v);
                    } else {
                        std::vector<UVNVertex> &o_v = subm->vn;
                        BuildVerts(d_glv,d_gltc,fi,&o_v);
                    }
                }
            } else if (!line.find("v")) {
                d_glv.push_back(ParseV(line));
            } else if (!line.find("g")) {
                NonIndexedMesh* s = new NonIndexedMesh();
                s->id  = sm_id++;
                s->name = ParseG(line);
                s->m_dir = dir;
                ctx->subMeshSet.push_back(s);
                //d_normals = true;
            } else if (!line.find("#")) {
                // comment
            } else if (!line.find(" ")) {
                // empty line with space
            } else if (line.empty()) {
                // empty line
            } else {
                //std::cout << "Unrecogized OBJ tag: "<< line << " while parsing:"   << fname << std::endl;
            };
        }
    } else  {
        //std::cout << "File Open failed!" << std::endl;
        return 0;
    }
    tst.close();
    return ctx ;
}
CObjMeshParser::CObjMeshParser()
 {
    IsReady = true;
}

