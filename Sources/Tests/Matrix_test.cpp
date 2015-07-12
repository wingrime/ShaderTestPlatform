#include "gtest/gtest.h"
#include "mat_math.h"
TEST(Matrix, NaiveCompareDefault) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(),SMat4x4()),true);
}

TEST(Matrix, NaiveCompareNeg) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(),SMat4x4(2.0)),false);
}
TEST(Matrix, AddDefault) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4()+SMat4x4(),SMat4x4(2.0)),true);
}
TEST(Matrix, SimpleAdd) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(2.0)+SMat4x4(2.0),SMat4x4(4.0)),true);
}
TEST(Matrix, MulIndent) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4()*SMat4x4(),SMat4x4()),true);
}

TEST(Matrix, DefaultIsIndent) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1.0),SMat4x4()),true);
}
TEST(Matrix, Transpose) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1.0).Transpose(),SMat4x4(1.0)),true);
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16).Transpose().Transpose(),SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16)),true);
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16).Transpose(), \
                          SMat4x4(1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16)),true);
}
TEST(Matrix, FullConstructor) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0),SMat4x4(1.0)),true);
}

TEST(Matrix,Mul) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16)*SMat4x4(1,2,3,4, 5,6,7,8,9, 10,11,12, 13,14,15,16),\
                          SMat4x4(90,100,110,120,202,228,254,280,314,356,398,440,426,484,542,600)),true);
}
TEST(Matrix,Translate) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1,2,3,4, \
                                  5,6,7,8, \
                                  9,10,11,12,\
                                  13,14,15,16).Translate(SVec4(10,20,30,1.0)),
                          SMat4x4(1,2,3,4+10, \
                                  5,6,7,8+20, \
                                  9,10,11,12+30,\
                                  13,14,15,16+1.0)),true);
}
TEST(Matrix, LookAtIndent) {

    SMat4x4 m = LookAtMatrix(SVec4(0.0,0.0,-1.0,0.0),SVec4(0.0,0.0,0.0,0.0),SVec4(0.0,1.0,0.0,0.0));
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(1.0,0.0,0.0,0.0, \
                                   0.0,1.0,0.0,0.0, \
                                   0.0,0.0,1.0,0.0, \
                                   0.0,0.0,0.0,1.0),m ),true );
}

TEST(Matrix,Move) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(1,2,3,4, \
                                  5,6,7,8, \
                                  9,10,11,12,\
                                  13,14,15,16).Move(1.0,2.0,3.0),
                          SMat4x4(1,2,3,4+1.0, \
                                  5,6,7,8+2.0, \
                                  9,10,11,12+3.0,\
                                  13,14,15,16)),true);
}


TEST(Matrix,Inverse) {
    EXPECT_EQ(SMat4x4::Eq(SMat4x4(0.0,0.0,0.0,-1.0, \
                                  1.0,0.0,0.0,0.0, \
                                  0.0,0.0,2.0/3.0,0.0,\
                                  0.0,2.0,0.0,0.0).Inverse(),
                          SMat4x4(0.0,1.0,0.0,0.0, \
                                  0.0,0.0,0.0,1.0/2.0, \
                                  0.0,0.0,3.0/2.0,0.0,\
                                 -1.0,0.0,0.0,0.0)),true);
}
/*
TODO:
Rotation Tests
Scale test
*/
