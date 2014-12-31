#include "gtest/gtest.h"
#include "MTLParser.h"
#include "Log.h"
TEST(MTLParser, NonExistsMTLFile) {
    MainLog l;
    MTLParser mp("not_exists.mtl");
   // EXPECT_EQ(mp., false);
   /*should be only default matrial*/
}

