#include "gtest/gtest.h"

#include "utils.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestIsInteger,DoesEqual){
  EXPECT_EQ(isInteger(1),true);
  EXPECT_EQ(isInteger(1.0),true);
  EXPECT_EQ(isInteger(0),true);
  EXPECT_EQ(isInteger(-1.0),true);
  
  EXPECT_EQ(isInteger(1.1),false);
  EXPECT_EQ(isInteger(0.1),false);
  EXPECT_EQ(isInteger(-0.1),false);
  EXPECT_EQ(isInteger(3.141592),false);
}

TEST(TestIsDivisible,DoesEqual){
  EXPECT_EQ(isDivisible(1,1),true);
  EXPECT_EQ(isDivisible(0,1),true);
  EXPECT_EQ(isDivisible(4,2),true);
  EXPECT_EQ(isDivisible(4,-2),true);

  EXPECT_EQ(isDivisible(3,2),false);
  EXPECT_EQ(isDivisible(-4,3),false);  
}
