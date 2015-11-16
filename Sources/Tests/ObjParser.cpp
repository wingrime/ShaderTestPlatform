#include "gtest/gtest.h"
#include "ObjParser.h"
#include "Log.h"
#include <stdio.h>
TEST(OBJParser, TestFile1) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\test1.obj");
    //ObjCtx *ctx = p.ParseFromFile("G:\\models\\san-miguel.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    if (!res)
    	FAIL();
    CObjMeshParser::ReleaseContext(ctx);
    EXPECT_EQ((bool)res, true);

}

TEST(OBJParser, TestFile2) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\test2.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    if (!res)
    	FAIL();
    CObjMeshParser::ReleaseContext(ctx);
    EXPECT_EQ((bool)res, true);

}

TEST(OBJParser, TestFile3) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\test3.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    if (!res)
    	FAIL();
    CObjMeshParser::ReleaseContext(ctx);
    EXPECT_EQ((bool)res, true);

}

TEST(OBJParser, TestFile4) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\test4.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
    	FAIL();
    CObjMeshParser::ReleaseContext(ctx);
    
}

TEST(OBJParser, MtlReflist) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\mtllist.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
    	FAIL();

    std::vector<std::string> v = ctx->refMaterialFiles;
    EXPECT_EQ(v.size(),5);
    if (v.size() != 5)
    	FAIL();
    EXPECT_EQ( v[0] , std::string("castle.mtl") );
    EXPECT_EQ( v[1] , std::string("castle2.mtl") );
    EXPECT_EQ( v[2] , std::string("castle3.mtl") );
    EXPECT_EQ( v[3] , std::string("castle4.mtl") );
    EXPECT_EQ( v[4] , std::string("castle5.mtl") );
    CObjMeshParser::ReleaseContext(ctx);
}

TEST(OBJParser, OneFace) {
	CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\oneface.obj");
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
    	FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),6);
    if (s->vn.size() != 6)
        FAIL();
    EXPECT_EQ(s->vn[0].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].p, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].p, vec3(0.0,1.0,0.0));
    EXPECT_EQ(s->vn[3].p, vec3(0.0,1.0,0.0));
    EXPECT_EQ(s->vn[4].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[5].p, vec3(1.0,1.0,0.0));
    for (int i = 0; i < 6 ; i++) {
        EXPECT_TRUE(vec2(s->vn[i].tc) == vec2(0.0,0.0));
    }
    CObjMeshParser::ReleaseContext(ctx);
}
TEST(OBJParser, OneTriangle) {
    CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\onetriangle.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
        FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),3);
    if (s->vn.size() != 3)
        FAIL();
    EXPECT_EQ(s->vn[0].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].p, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].p, vec3(0.0,1.0,0.0));
    for (int i = 0; i < 3 ; i++) {
        EXPECT_TRUE(vec2(s->vn[i].tc) == vec2(0.0,0.0));
    }
    CObjMeshParser::ReleaseContext(ctx);
}
TEST(OBJParser, OneTriangleNormal) {
    CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\onetriangle_normal.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
        FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),3);
    if (s->vn.size() != 3)
        FAIL();
    EXPECT_EQ(s->vn[0].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].p, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].p, vec3(0.0,1.0,0.0));
    EXPECT_EQ(s->vn[0].n, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].n, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].n, vec3(0.0,1.0,0.0));
    for (int i = 0; i < 3 ; i++) {
        EXPECT_TRUE(vec2(s->vn[i].tc) == vec2(0.0,0.0));
    }
    CObjMeshParser::ReleaseContext(ctx);
}
TEST(OBJParser, OneTriangleNormalUV) {
    CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\onetriangle_normal_uv.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
        FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),3);
    if (s->vn.size() != 3)
        FAIL();
    EXPECT_EQ(s->vn[0].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].p, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].p, vec3(0.0,1.0,0.0));
    EXPECT_EQ(s->vn[0].n, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].n, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].n, vec3(0.0,1.0,0.0));

    EXPECT_EQ(s->vn[0].tc, vec2(0.2,0.4));
    EXPECT_EQ(s->vn[1].tc, vec2(0.3,0.2));
    EXPECT_EQ(s->vn[2].tc, vec2(0.2,0.1));


    CObjMeshParser::ReleaseContext(ctx);
}
TEST(OBJParser, OneTriangleNormalUVInvIdx) {
    CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\onetriangle_normal_uv_invalidx.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
        FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),0);
    //should not crash

    CObjMeshParser::ReleaseContext(ctx);
}
TEST(OBJParser, OneTriangleUV) {
    CObjMeshParser p;
    ObjCtx *ctx = p.ParseFromFile("..\\TestCaseAssert\\onetriangle_uv.obj");
    //CObjMeshParser::Reflect(ctx);
    void *res  = ctx;
    EXPECT_EQ((bool)res, true);
    if (!res)
        FAIL();
    EXPECT_EQ(ctx->subMeshSet.size(),1);
    if (ctx->subMeshSet.size() != 1)
        FAIL();
    NonIndexedMesh * s = ctx->subMeshSet[0];
    EXPECT_EQ(s->vn.size(),3);
    if (s->vn.size() != 3)
        FAIL();
    EXPECT_EQ(s->vn[0].p, vec3(1.0,1.0,0.0));
    EXPECT_EQ(s->vn[1].p, vec3(1.0,0.0,0.0));
    EXPECT_EQ(s->vn[2].p, vec3(0.0,1.0,0.0));

    EXPECT_EQ(s->vn[0].tc, vec2(0.2,0.4));
    EXPECT_EQ(s->vn[1].tc, vec2(0.3,0.2));
    EXPECT_EQ(s->vn[2].tc, vec2(0.2,0.1));


    CObjMeshParser::ReleaseContext(ctx);
}
