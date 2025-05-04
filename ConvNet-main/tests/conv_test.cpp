#include <iostream>
#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "conv.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestConvInputShape,DoesEqual){
  Conv conv1({28,28,1},{2,2,1,3,1,1},{0,0},Activations::Linear);
  EXPECT_TRUE(ComapreArrays3(conv1.getInputShape(),{28,28,1}));

  Conv conv2({28,28,3},{5,5,3,16,1,1},{2,2},Activations::Sigmoid);
  EXPECT_TRUE(ComapreArrays3(conv2.getInputShape(),{28,28,3}));

  Conv conv3({1,1,1},{1,1,1,1,1,1},{0,0},Activations::Tanh);
  EXPECT_TRUE(ComapreArrays3(conv3.getInputShape(),{1,1,1}));

  Conv conv4({32,32,2},{4,4,2,1,2,4},{0,0},Activations::Relu);
  EXPECT_TRUE(ComapreArrays3(conv4.getInputShape(),{32,32,2}));
}

TEST(TestConvOutputShape,DoesEqual){
  Conv conv1({28,28,1},{2,2,1,3,1,1},{0,0},Activations::Linear);
  EXPECT_TRUE(ComapreArrays3(conv1.getOutputShape(),{27,27,3}));

  Conv conv2({28,28,3},{5,5,3,16,1,1},{2,2},Activations::Sigmoid);
  EXPECT_TRUE(ComapreArrays3(conv2.getOutputShape(),{28,28,16}));

  Conv conv3({1,1,1},{1,1,1,1,1,1},{0,0},Activations::Tanh);
  EXPECT_TRUE(ComapreArrays3(conv3.getOutputShape(),{1,1,1}));

  Conv conv4({32,32,2},{4,4,2,1,2,4},{0,0},Activations::Relu);
  EXPECT_TRUE(ComapreArrays3(conv4.getOutputShape(),{15,8,1}));
}

TEST(TestConvKernelShape,DoesEqual){
  Conv conv1({28,28,1},{2,2,1,3,1,1},{0,0},Activations::Linear);
  EXPECT_TRUE(ComapreArrays6(conv1.getKernelShape(),{2,2,1,3,1,1}));

  Conv conv2({28,28,3},{5,5,3,16,1,1},{2,2},Activations::Sigmoid);
  EXPECT_TRUE(ComapreArrays6(conv2.getKernelShape(),{5,5,3,16,1,1}));

  Conv conv3({1,1,1},{1,1,1,1,1,1},{0,0},Activations::Tanh);
  EXPECT_TRUE(ComapreArrays6(conv3.getKernelShape(),{1,1,1,1,1,1}));

  Conv conv4({32,32,2},{4,4,2,1,2,4},{0,0},Activations::Relu);
  EXPECT_TRUE(ComapreArrays6(conv4.getKernelShape(),{4,4,2,1,2,4}));
}

TEST(TestConvPadShape,DoesEqual){
  Conv conv1({28,28,1},{2,2,1,3,1,1},{0,0},Activations::Linear);
  EXPECT_TRUE(ComapreArrays2(conv1.getPadShape(),{0,0}));

  Conv conv2({28,28,3},{5,5,3,16,1,1},{2,2},Activations::Sigmoid);
  EXPECT_TRUE(ComapreArrays2(conv2.getPadShape(),{2,2}));

  Conv conv3({1,1,1},{1,1,1,1,1,1},{0,0},Activations::Tanh);
  EXPECT_TRUE(ComapreArrays2(conv3.getPadShape(),{0,0}));

  Conv conv4({32,32,2},{4,4,2,1,2,4},{0,0},Activations::Relu);
  EXPECT_TRUE(ComapreArrays2(conv4.getPadShape(),{0,0}));
}
