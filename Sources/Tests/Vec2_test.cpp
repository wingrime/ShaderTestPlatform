#include "gtest/gtest.h"
#include "mat_math.h"
#include <cmath>

TEST(SVec2, DefaultConstructor ) {

    SVec2 v;
    EXPECT_EQ(v.x == 0.0,true);
    EXPECT_EQ(v.y == 0.0,true);

    EXPECT_EQ(v.r == 0.0,true);
    EXPECT_EQ(v.g == 0.0,true);
    
    EXPECT_EQ(v.w == 0.0,true);
    EXPECT_EQ(v.h == 0.0,true);
}
TEST(SVec2, NormalConstructor ) {

    SVec2 v(1.0,2.0);
    EXPECT_FLOAT_EQ(v.x , 1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.r , 1.0);
    EXPECT_FLOAT_EQ(v.g , 2.0);
    EXPECT_FLOAT_EQ(v.w , 1.0);
    EXPECT_FLOAT_EQ(v.h , 2.0);
 
}
TEST(SVec2, CopyConstructor ) {

    SVec2 v(SVec2(SVec2(1.0,2.0)));

    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y ,2.0);

}
TEST(SVec2, Add ) {

    SVec2 v1(2.1 ,3.3);
    SVec2 v2(3.1 ,3.5);
    SVec2 v3 = v1+v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1+3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3+3.5);
}
TEST(SVec2, Sub ) {

    SVec2 v1(2.1 ,3.3);
    SVec2 v2(3.1 ,3.5);
    SVec2 v3 = v1-v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1-3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3-3.5);
}
