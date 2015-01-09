#include "gtest/gtest.h"
#include "mat_math.h"
#include <cmath>

TEST(SVec4, DefaultConstructor ) {

    SVec4 v;
    EXPECT_EQ(v.x == 0.0,true);
    EXPECT_EQ(v.y == 0.0,true);
    EXPECT_EQ(v.z == 0.0,true);
    EXPECT_EQ(v.w == 0.0,true);
    EXPECT_EQ(v.a == 0.0,true);
    EXPECT_EQ(v.r == 0.0,true);
    EXPECT_EQ(v.g == 0.0,true);
    EXPECT_EQ(v.b == 0.0,true);
}
TEST(SVec4, NormalConstructor ) {

    SVec4 v(1.0,2.0,3.0,4.0);
    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.z , 3.0);
    EXPECT_FLOAT_EQ(v.w , 4.0);
    EXPECT_FLOAT_EQ(v.r , 1.0);
    EXPECT_FLOAT_EQ(v.g , 2.0);
    EXPECT_FLOAT_EQ(v.b , 3.0);
    EXPECT_FLOAT_EQ(v.a , 4.0);
}

TEST(SVec4, CopyConstructor ) {

    SVec4 v(SVec4(SVec4(1.0,2.0,3.0,4.0)));
    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.z , 3.0);
    EXPECT_FLOAT_EQ(v.w , 4.0);

}

TEST(SVec4, Eq ) {

    EXPECT_EQ(SVec4::Eq(SVec4(),SVec4()),true);
    EXPECT_EQ(SVec4::Eq(SVec4(1,2,3,4),SVec4(1,2,3,4)),true);

}


TEST(SVec4, CrossProduct3_operator ) {

    EXPECT_EQ(SVec4::Eq(SVec4::Cross3(SVec4(1,2,3,0),SVec4(3,2,1,0)),SVec4(-4,8,-4,0)),true);
    EXPECT_EQ(SVec4::Eq(SVec4::Cross3(SVec4(3,2,1,0),SVec4(1,2,3,0)),SVec4(4,-8,4,0)),true);

}


TEST(SVec4, Add_operator ) {

    EXPECT_EQ(SVec4::Eq(SVec4(1,2,3,0)+SVec4(3,2,1,0),SVec4(4,4,4,0)),true);
    EXPECT_EQ(SVec4::Eq(SVec4(3,2,1,0)+SVec4(1,2,3,0),SVec4(4,4,4,0)),true);

}
TEST(SVec4, Sub_operator ) {

    EXPECT_EQ(SVec4::Eq(SVec4(1,2,3,0)-SVec4(3,2,1,0),SVec4(-2,0,2,0)),true);
    EXPECT_EQ(SVec4::Eq(SVec4(3,2,1,0)-SVec4(1,2,3,0),SVec4(2,0,-2,0)),true);

}

TEST(SVec4, Length ) {

    EXPECT_FLOAT_EQ(SVec4(1.0,1.0,1.0,1.0).Length(),2.0);
    EXPECT_FLOAT_EQ(SVec4(2.0,4.0,4.0,0.0).Length(),6.0);
    EXPECT_FLOAT_EQ(SVec4(4.0,4.0,2.0,0.0).Length(),6.0);
    EXPECT_FLOAT_EQ(SVec4(4.0,4.0,2.0,0.0).Length(),6.0);
}

TEST(SVec4, Dot ) {

    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(2,4,4,0),SVec4(4,4,1,0)),28.0);
    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(0,0,0,0),SVec4(0,0,0,0)),0.0);
    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(1,0,0,0),SVec4(1,0,0,0)),1.0);
    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(0,1,0,0),SVec4(0,1,0,0)),1.0);
    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(0,0,1,0),SVec4(0,0,1,0)),1.0);
    EXPECT_FLOAT_EQ(SVec4::Dot(SVec4(0,0,0,1),SVec4(0,0,0,1)),1.0);
}

TEST(SVec4, Normalize ) {
    EXPECT_EQ(SVec4::Eq(SVec4(3,1,2,0).Normalize(),SVec4(0.801784,0.267261,0.534522,0)),true);

}

