#include "gtest/gtest.h"
#include "ImageBuffer.h"
TEST(ImageBuffer, NonExistsImage) {
    ImageBuffer img("NonExistsImage.png");
    EXPECT_EQ(img.IsReady, false);
}
TEST(ImageBuffer, PNGImage) {
    ImageBuffer img("TestCaseAssert\\empty_normal.png");
    EXPECT_EQ(img.IsReady, true);
}
