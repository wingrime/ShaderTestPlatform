#include "gtest/gtest.h"
#include "mat_math.h"
#include <cmath>

TEST(vec3, DefaultConstructor ) {

    vec3 v;
    EXPECT_EQ(v.x == 0.0,true);
    EXPECT_EQ(v.y == 0.0,true);
    EXPECT_EQ(v.z == 0.0,true);
}
TEST(vec3, NormalConstructor ) {

    vec3 v(1.0,2.0,3.0);
    EXPECT_FLOAT_EQ(v.x , 1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.z , 3.0);
    EXPECT_FLOAT_EQ(v.r , 1.0);
    EXPECT_FLOAT_EQ(v.g , 2.0);
    EXPECT_FLOAT_EQ(v.b , 3.0);
 
}
TEST(vec3, CopyConstructor ) {

    vec3 v(vec3(vec3(1.0,2.0,3.0)));

    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y ,2.0);
    EXPECT_FLOAT_EQ(v.z ,3.0);

}
TEST(vec3, Add ) {

    vec3 v1(2.1 ,3.3,1.2);
    vec3 v2(3.1 ,3.5,2.2);
    vec3 v3 = v1+v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1+3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3+3.5);
    EXPECT_FLOAT_EQ(v3.z ,1.2+2.2);
}
TEST(vec3, Sub ) {

    vec3 v1(2.1 ,3.3,2.0);
    vec3 v2(3.1 ,3.5,1.0);
    vec3 v3 = v1-v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1-3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3-3.5);
    EXPECT_FLOAT_EQ(v3.z ,2.0-1.0);
}
TEST(vec3, Neg ) {

    vec3 v1(2.1 ,3.3,3.0);
    EXPECT_FLOAT_EQ(-v1.x ,-2.1);
    EXPECT_FLOAT_EQ(-v1.y ,-3.3);
    EXPECT_FLOAT_EQ(-v1.z ,-3.0);
}
TEST(vec3, FloatMul ) {

    vec3 v1(1.0 ,2.0,3.0);
    EXPECT_FLOAT_EQ((v1*2.0).x ,2.0);
    EXPECT_FLOAT_EQ((v1*2.0).y ,4.0);
    EXPECT_FLOAT_EQ((v1*2.0).z ,6.0);
}
TEST(vec3, FloatDiv ) {

    vec3 v1(1.0 ,2.0,4.0);
    EXPECT_FLOAT_EQ((v1/2.0).x ,0.5);
    EXPECT_FLOAT_EQ((v1/2.0).y ,1.0);
    EXPECT_FLOAT_EQ((v1/2.0).z ,2.0);
}

TEST(vec3, FloatDivZero ) {

    vec3 v1(1.0 ,2.0,4.0);
    EXPECT_FLOAT_EQ((v1/0.0).x ,0.0);
    EXPECT_FLOAT_EQ((v1/0.0).y ,0.0);
    EXPECT_FLOAT_EQ((v1/0.0).z ,0.0);
}
TEST(vec3, Length ) {

    vec3 v1(2.0 ,2.0,2.0);
    EXPECT_FLOAT_EQ( vec3::length(v1) , sqrt(2.0*2.0+2.0*2.0+2.0*2.0) );
}

TEST(vec3, Norm ) {

    vec3 v1(2.0 ,2.0,2.0);
    vec3 v2 = vec3::normalize(v1);
    EXPECT_FLOAT_EQ( v2.x , 2.0/sqrt(2.0*2.0+2.0*2.0+2.0*2.0) );
    EXPECT_FLOAT_EQ( v2.y , 2.0/sqrt(2.0*2.0+2.0*2.0+2.0*2.0) );
    EXPECT_FLOAT_EQ( v2.z , 2.0/sqrt(2.0*2.0+2.0*2.0+2.0*2.0) );
       
}
TEST(vec3, Equal ) {

    vec3 v1(2.1 ,3.3,-4.0);
    vec3 v2(2.1,3.3,-4.0);
    vec3 v3(0.0,0.0,0.0);
    vec3 v4(0.0,0.0,0.0);
    EXPECT_TRUE(v1 == v2);
    EXPECT_TRUE(v3 == v4);
    EXPECT_FALSE(v1 == v3);
}
