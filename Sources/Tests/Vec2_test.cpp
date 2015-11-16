#include "gtest/gtest.h"
#include "mat_math.h"
#include <cmath>

TEST(vec2, DefaultConstructor ) {

    vec2 v;
    EXPECT_EQ(v.x == 0.0,true);
    EXPECT_EQ(v.y == 0.0,true);

    EXPECT_EQ(v.r == 0.0,true);
    EXPECT_EQ(v.g == 0.0,true);
    
    EXPECT_EQ(v.w == 0.0,true);
    EXPECT_EQ(v.h == 0.0,true);
}
TEST(vec2, NormalConstructor ) {

    vec2 v(1.0,2.0);
    EXPECT_FLOAT_EQ(v.x , 1.0);
    EXPECT_FLOAT_EQ(v.y , 2.0);
    EXPECT_FLOAT_EQ(v.r , 1.0);
    EXPECT_FLOAT_EQ(v.g , 2.0);
    EXPECT_FLOAT_EQ(v.w , 1.0);
    EXPECT_FLOAT_EQ(v.h , 2.0);
 
}
TEST(vec2, CopyConstructor ) {

    vec2 v(vec2(vec2(1.0,2.0)));

    EXPECT_FLOAT_EQ(v.x ,1.0);
    EXPECT_FLOAT_EQ(v.y ,2.0);

}
TEST(vec2, Add ) {

    vec2 v1(2.1 ,3.3);
    vec2 v2(3.1 ,3.5);
    vec2 v3 = v1+v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1+3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3+3.5);
}
TEST(vec2, Sub ) {

    vec2 v1(2.1 ,3.3);
    vec2 v2(3.1 ,3.5);
    vec2 v3 = v1-v2;
    EXPECT_FLOAT_EQ(v3.x ,2.1-3.1);
    EXPECT_FLOAT_EQ(v3.y ,3.3-3.5);
}
TEST(vec2, Neg ) {

    vec2 v1(2.1 ,3.3);
    EXPECT_FLOAT_EQ(-v1.x ,-2.1);
    EXPECT_FLOAT_EQ(-v1.y ,-3.3);
}
TEST(vec2, FloatMul ) {

    vec2 v1(1.0 ,2.0);
    EXPECT_FLOAT_EQ((v1*2.0).x ,2.0);
    EXPECT_FLOAT_EQ((v1*2.0).y ,4.0);
}
TEST(vec2, FloatDiv ) {

    vec2 v1(1.0 ,2.0);
    EXPECT_FLOAT_EQ((v1/2.0).x ,0.5);
    EXPECT_FLOAT_EQ((v1/2.0).y ,1.0);
}
TEST(vec2, Equal ) {

    vec2 v1(2.1 ,3.3);
    vec2 v2(2.1,3.3);
    vec2 v3(0.0,0.0);
    vec2 v4(0.0,0.0);
    EXPECT_TRUE(v1 == v2);
    EXPECT_TRUE(v3 == v4);
    EXPECT_FALSE(v1 == v3);


}

