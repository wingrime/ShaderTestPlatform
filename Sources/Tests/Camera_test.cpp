#include "gtest/gtest.h"
#include "r_camera.h"
#include "mat_math.h"
TEST(Camera, ConstructorFromMatrices) {
    SCamera *cam = new SCamera(SMat4x4(), SMat4x4());
    SMat4x4 m = cam->getViewMatrix();
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(), cam->getProjMatrix()),true );
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(), cam->getViewMatrix()),true );
    delete cam;

}
TEST(Camera, ConstructorFromMatricesNeg) {
    SCamera *cam = new SCamera(SMat4x4(2.0), SMat4x4(2.0));
    SMat4x4 m = cam->getViewMatrix();
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(), cam->getProjMatrix()),false );
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(), cam->getViewMatrix()),false );
    delete cam;

}
TEST(Camera, MoveForward) {
    SCamera *cam = new SCamera(SMat4x4(),SMat4x4());
    /*
     * By default, indent matix should point to -z;
     * so move to 1.0 should give (2.0) in 4 row
    */
    cam->goForward(1.0);
    SMat4x4 m = cam->getViewMatrix();
    EXPECT_FLOAT_EQ(cam->getViewMatrix().a34 ,2.0 );
    delete cam;

}

TEST(Camera, RotateZ90) {
    SCamera *cam = new SCamera(SMat4x4(), SMat4x4());
    cam->rotEulerZ(toRad(90.0));
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(0.0,-1.0,0.0,0.0, \
                                   1.0,0.0,0.0,0.0, \
                                   0.0,0.0,1.0,0.0, \
                                   0.0,0.0,0.0,1.0), cam->getViewMatrix()),true );
    delete cam;
}

TEST(Camera, RotateX90) {
    SCamera *cam = new SCamera(SMat4x4(), SMat4x4());
    cam->rotEulerX(toRad(90.0));

    EXPECT_EQ( SMat4x4::Eq(SMat4x4(1.0,0.0,0.0,0.0, \
                                   0.0,0.0,-1.0,0.0, \
                                   0.0,1.0,0.0,0.0, \
                                   0.0,0.0,0.0,1.0), cam->getViewMatrix()),true );
    delete cam;

}

TEST(Camera, RotateY90) {
    SCamera *cam = new SCamera(SMat4x4(), SMat4x4());
    cam->rotEulerY(toRad(90.0));

    EXPECT_EQ( SMat4x4::Eq(SMat4x4(0.0,0.0,1.0,0.0, \
                                   0.0,1.0,0.0,0.0, \
                                   -1.0,0.0,0.0,0.0, \
                                   0.0,0.0,0.0,1.0), cam->getViewMatrix()),true );
    delete cam;

}

TEST(Camera, LookAtIndent) {
    SCamera *cam = new SCamera(SMat4x4(), SMat4x4());

    cam->LookAt(SVec4(0.0,0.0,-1.0,0.0),SVec4(0.0,0.0,0.0,0.0),SVec4(0.0,1.0,0.0,0.0));
    EXPECT_EQ( SMat4x4::Eq(SMat4x4(1.0,0.0,0.0,0.0, \
                                   0.0,1.0,0.0,0.0, \
                                   0.0,0.0,1.0,0.0, \
                                   0.0,0.0,0.0,1.0), cam->getViewMatrix()),true );
    delete cam;

}
