#include <iostream>
#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "dense.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestDenseInputSize,DoesEqual){
  Dense dense1(1,1);
  EXPECT_EQ(dense1.getInputSize(),1);
  
  Dense dense2(3,4);
  EXPECT_EQ(dense2.getInputSize(),3);
}

TEST(TestDenseOutputSize,DoesEqual){
  Dense dense1(1,1);
  EXPECT_EQ(dense1.getOutputSize(),1);
  
  Dense dense2(3,4);
  EXPECT_EQ(dense2.getOutputSize(),4);
}

TEST(TestDenseDropoutProb,DoesEqual){
  Dense dense1(1,1,Activations::Linear,0.1);
  EXPECT_FLOAT_EQ(dense1.getDropoutProb(),0.1);
  
  Dense dense2(3,4,Activations::Sigmoid,1.0);
  EXPECT_FLOAT_EQ(dense2.getDropoutProb(),1.0);
  
  Dense dense3(3,2,Activations::Tanh,0.0);
  EXPECT_FLOAT_EQ(dense3.getDropoutProb(),0.0);
  
  Dense dense4(3,1,Activations::Relu,0.9);
  EXPECT_FLOAT_EQ(dense4.getDropoutProb(),0.9);
}

TEST(TestDenseActivationType,DoesEqual){
  Dense dense1(1,1,Activations::Linear,0.1);
  EXPECT_TRUE(dense1.getActivationType() == Activations::Linear);
  
  Dense dense2(3,4,Activations::Sigmoid,1.0);
  EXPECT_TRUE(dense2.getActivationType() == Activations::Sigmoid);
  
  Dense dense3(3,2,Activations::Tanh,0.0);
  EXPECT_TRUE(dense3.getActivationType() == Activations::Tanh);
  
  Dense dense4(3,1,Activations::Relu,0.9);
  EXPECT_TRUE(dense4.getActivationType() == Activations::Relu);
  
  Dense dense5(3,10,Activations::LeakyRelu,0.9);
  EXPECT_TRUE(dense5.getActivationType() == Activations::LeakyRelu);  
}
