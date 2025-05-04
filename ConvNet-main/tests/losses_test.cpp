#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "losses.h"
#include "utils_for_test.h"

#include <iostream>

using namespace convnet;

TEST(TestMse,DoesEqual){
  Eigen::VectorXf predicts(3);
  Eigen::VectorXf labels(3);
  double expectation;
  
  predicts << 0,0,0;
  labels << 0,0,0;
  expectation = 0.0;
  EXPECT_FLOAT_EQ(Mse(predicts,labels),expectation);

  predicts << 3,141592,-653589;
  labels << 3,141592,-653589;
  expectation = 0.0;
  EXPECT_FLOAT_EQ(Mse(predicts,labels),expectation);

  predicts << 1,2,3;
  labels << 1,1,1;
  expectation = 2.5;
  EXPECT_FLOAT_EQ(Mse(predicts,labels),expectation);

  predicts << -2,5,-8;
  labels << 3,-4,-1;
  expectation = 77.5;
  EXPECT_FLOAT_EQ(Mse(predicts,labels),expectation);
}

TEST(TestCategoricalCrossEntropy,DoesEqual){
  Eigen::VectorXf predicts(3);
  Eigen::VectorXf labels(3);
  double expectation;
  double factor;

  factor = 3*exp(0);
  expectation = log(3);
  predicts << 0,0,0;
  labels << 1,0,0;
  EXPECT_FLOAT_EQ(CategoricalCrossEntropy(predicts,labels),expectation);

  factor = exp(1)+2*exp(0);
  expectation = -log(exp(1)/factor);
  predicts << 1,0,0;
  labels << 1,0,0;
  EXPECT_FLOAT_EQ(CategoricalCrossEntropy(predicts,labels),expectation);

  factor = exp(1)+exp(-1)+exp(0);
  expectation = -log(exp(-1)/factor);
  predicts << 1,-1,0;
  labels << 0,1,0;
  EXPECT_FLOAT_EQ(CategoricalCrossEntropy(predicts,labels),expectation);

  factor = exp(3)+exp(-4)+exp(0);
  expectation = -log(exp(0)/factor);
  predicts << 3,-4,0;
  labels << 0,0,1;
  EXPECT_FLOAT_EQ(CategoricalCrossEntropy(predicts,labels),expectation);
}

TEST(TestGradientOfMse,DoesEqual){
  Eigen::VectorXf predicts(3);
  Eigen::VectorXf labels(3);
  Eigen::VectorXf expectation(3);
  
  predicts << 0,0,0;
  labels << 1,0,0;
  expectation << -1,0,0;
  EXPECT_TRUE(CompareMatrices(GradientOfMse(predicts,labels),expectation));

  predicts << 1,-1,0;
  labels << 1,-1,0;
  expectation << 0,0,0;
  EXPECT_TRUE(CompareMatrices(GradientOfMse(predicts,labels),expectation));

  predicts << 1,-1,0;
  labels << -1,1,0;
  expectation << 2,-2,0;
  EXPECT_TRUE(CompareMatrices(GradientOfMse(predicts,labels),expectation));
}

TEST(TestGradientOfCategoricalCrossEntropy,DoesEqual){
  Eigen::VectorXf predicts(3);
  Eigen::VectorXf labels(3);
  Eigen::VectorXf expectation(3);
  
  predicts << 0,0,0;
  labels << 1,0,0;
  expectation << -2.0/3,1.0/3,1.0/3;
  EXPECT_TRUE(CompareMatrices(GradientOfCategoricalCrossEntropy(predicts,labels),expectation));
}

TEST(TestSoftmax,DoesEqual){
  Eigen::VectorXf logits(3);
  Eigen::VectorXf expectation(3);
  
  logits << 0,0,0;
  expectation << 1./3,1./3,1./3;
  EXPECT_TRUE(CompareMatrices(Softmax(logits),expectation));

  logits << 1,0,-1;
  expectation << 0.6652409557748219,0.24472847105479767,0.09003057317038046;
  EXPECT_TRUE(CompareMatrices(Softmax(logits),expectation));  
}
