#include <iostream>
#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "dense.h"
#include "pooling.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestPoolingInputShape,DoesEqual){
  Pooling pooling1({28,28,1},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling1.getInputShape(),{28,28,1}));

  Pooling pooling2({28,28,3},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling2.getInputShape(),{28,28,3}));

  Pooling pooling3({1,1,1},{1,1,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling3.getInputShape(),{1,1,1}));

  Pooling pooling4({32,32,3},{6,6,2,2});
  EXPECT_TRUE(ComapreArrays3(pooling4.getInputShape(),{32,32,3}));
}

TEST(TestPoolingOutputShape,DoesEqual){
  Pooling pooling1({28,28,1},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling1.getOutputShape(),{27,27,1}));

  Pooling pooling2({28,28,3},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling2.getOutputShape(),{27,27,3}));

  Pooling pooling3({1,1,1},{1,1,1,1});
  EXPECT_TRUE(ComapreArrays3(pooling3.getOutputShape(),{1,1,1}));

  Pooling pooling4({32,32,3},{6,6,2,2});
  EXPECT_TRUE(ComapreArrays3(pooling4.getOutputShape(),{14,14,3}));
}

TEST(TestPoolingShape,DoesEqual){
  Pooling pooling1({28,28,1},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays4(pooling1.getPoolingShape(),{2,2,1,1}));

  Pooling pooling2({28,28,3},{2,2,1,1});
  EXPECT_TRUE(ComapreArrays4(pooling2.getPoolingShape(),{2,2,1,1}));

  Pooling pooling3({1,1,1},{1,1,1,1});
  EXPECT_TRUE(ComapreArrays4(pooling3.getPoolingShape(),{1,1,1,1}));

  Pooling pooling4({32,32,3},{6,6,2,2});
  EXPECT_TRUE(ComapreArrays4(pooling4.getPoolingShape(),{6,6,2,2}));
}
