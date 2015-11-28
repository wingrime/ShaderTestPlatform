#include "gtest/gtest.h"
#include "mat_math.h"
#include <cmath>

TEST(vec4, DefaultConstructor ) {

  vec4 v;
    EXPECT_EQ(v.x == 0.0,true);
    EXPECT_EQ(v.y == 0.0,true);
    EXPECT_EQ(v.z == 0.0,true);
    EXPECT_EQ(v.w == 0.0,true);
    EXPECT_EQ(v.a == 0.0,true);
    EXPECT_EQ(v.r == 0.0,true);
    EXPECT_EQ(v.g == 0.0,true);
    EXPECT_EQ(v.b == 0.0,true);
}
TEST(vec4, NormalConstructor ) {

    vec4 v(1.0,2.0,3.0,4.0);
    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.z , 3.0);
    EXPECT_FLOAT_EQ(v.w , 4.0);
    EXPECT_FLOAT_EQ(v.r , 1.0);
    EXPECT_FLOAT_EQ(v.g , 2.0);
    EXPECT_FLOAT_EQ(v.b , 3.0);
    EXPECT_FLOAT_EQ(v.a , 4.0);
}

TEST(vec4, CopyConstructor ) {

    vec4 v(vec4(vec4(1.0,2.0,3.0,4.0)));
    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.z , 3.0);
    EXPECT_FLOAT_EQ(v.w , 4.0);

}

TEST(vec4, Eq ) {

    EXPECT_EQ(vec4::Eq(vec4(),vec4()),true);
    EXPECT_EQ(vec4::Eq(vec4(1,2,3,4),vec4(1,2,3,4)),true);
    EXPECT_EQ(vec4::Eq(vec4(-1,2,-3,4),vec4(-1,2,-3,4)),true);

}

TEST(vec4, Eq_operator ) {

    EXPECT_EQ(vec4()==vec4(),true);
    EXPECT_EQ((vec4(1,2,3,4)==vec4(1,2,3,4)),true);
    EXPECT_EQ((vec4(-1,2,3,-4)==vec4(-1,2,3,-4)),true);

}

TEST(vec4, CrossProduct3_operator ) {

    EXPECT_EQ(vec4::Eq(vec4::Cross3(vec4(1,2,3,0),vec4(3,2,1,0)),vec4(-4,8,-4,0)),true);
    EXPECT_EQ(vec4::Eq(vec4::Cross3(vec4(3,2,1,0),vec4(1,2,3,0)),vec4(4,-8,4,0)),true);

}


TEST(vec4, Add_operator ) {

    EXPECT_EQ(vec4::Eq(vec4(1,2,3,0)+vec4(3,2,1,0),vec4(4,4,4,0)),true);
    EXPECT_EQ(vec4::Eq(vec4(3,2,1,0)+vec4(1,2,3,0),vec4(4,4,4,0)),true);

}
TEST(vec4, Sub_operator ) {

    EXPECT_EQ(vec4::Eq(vec4(1,2,3,0)-vec4(3,2,1,0),vec4(-2,0,2,0)),true);
    EXPECT_EQ(vec4::Eq(vec4(3,2,1,0)-vec4(1,2,3,0),vec4(2,0,-2,0)),true);

}

TEST(vec4, Length ) {

    EXPECT_FLOAT_EQ(vec4(1.0,1.0,1.0,1.0).Length(),2.0);
    EXPECT_FLOAT_EQ(vec4(2.0,4.0,4.0,0.0).Length(),6.0);
    EXPECT_FLOAT_EQ(vec4(4.0,4.0,2.0,0.0).Length(),6.0);
    EXPECT_FLOAT_EQ(vec4(4.0,4.0,2.0,0.0).Length(),6.0);
}

TEST(vec4, Dot ) {

    EXPECT_FLOAT_EQ(vec4::Dot(vec4(2,4,4,0),vec4(4,4,1,0)),28.0);
    EXPECT_FLOAT_EQ(vec4::Dot(vec4(0,0,0,0),vec4(0,0,0,0)),0.0);
    EXPECT_FLOAT_EQ(vec4::Dot(vec4(1,0,0,0),vec4(1,0,0,0)),1.0);
    EXPECT_FLOAT_EQ(vec4::Dot(vec4(0,1,0,0),vec4(0,1,0,0)),1.0);
    EXPECT_FLOAT_EQ(vec4::Dot(vec4(0,0,1,0),vec4(0,0,1,0)),1.0);
    EXPECT_FLOAT_EQ(vec4::Dot(vec4(0,0,0,1),vec4(0,0,0,1)),1.0);
}

TEST(vec4, Normalize ) {
    EXPECT_EQ(vec4::Eq(vec4(3,1,2,0).Normalize(),vec4(0.801784,0.267261,0.534522,0)),true);
    EXPECT_EQ(vec4::Eq(vec4::Normalize(vec4(3,1,2,0)),vec4(0.801784,0.267261,0.534522,0)),true);
}

