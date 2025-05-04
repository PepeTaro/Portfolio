#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "activations.h"
#include "utils_for_test.h"

#include <iostream>

using namespace convnet;

TEST(TestScalerRelu,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarRelu(0.0),0.0);
  EXPECT_FLOAT_EQ(ScalarRelu(1.1),1.1);
  EXPECT_FLOAT_EQ(ScalarRelu(-1.1),0.0);
}

TEST(TestScalarDerivativeOfRelu,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarDerivativeOfRelu(0.0),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfRelu(1.1),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfRelu(-1.1),0.0);
}

TEST(TestScalerLeakyRelu,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarLeakyRelu(0.0),0.0);
  EXPECT_FLOAT_EQ(ScalarLeakyRelu(1.1),1.1);
  EXPECT_FLOAT_EQ(ScalarLeakyRelu(-1.1),convnet::leaky_relu_param*(-1.1));
}

TEST(TestScalarDerivativeOfLeakyRelu,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLeakyRelu(0.0),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLeakyRelu(1.1),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLeakyRelu(-1.1),convnet::leaky_relu_param);
}

TEST(TestScalerSigmoid,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarSigmoid(0.0),0.5);
  EXPECT_FLOAT_EQ(ScalarSigmoid(1.1),0.7502601055951176);
  EXPECT_FLOAT_EQ(ScalarSigmoid(-1.1),0.24973989440488237);
}

TEST(TestScalarDerivativeOfSigmoid,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarDerivativeOfSigmoid(0.0),0.25);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfSigmoid(1.1),0.1873698795475206);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfSigmoid(-1.1),0.18736987954752057);
}

TEST(TestScalerTanh,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarTanh(0.0),0.0);
  EXPECT_FLOAT_EQ(ScalarTanh(1.1),0.8004990217606297);
  EXPECT_FLOAT_EQ(ScalarTanh(-1.1),-0.8004990217606297);
}

TEST(TestScalarDerivativeOfTanh,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarDerivativeOfTanh(0.0),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfTanh(1.1),0.35920131616027484);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfTanh(-1.1),0.35920131616027484);
}

TEST(TestScalerLinear,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarLinear(0.0),0.0);
  EXPECT_FLOAT_EQ(ScalarLinear(1.1),1.1);
  EXPECT_FLOAT_EQ(ScalarLinear(-1.1),-1.1);
}

TEST(TestScalarDerivativeOfLinear,DoesEqual){
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLinear(0.0),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLinear(1.1),1.0);
  EXPECT_FLOAT_EQ(ScalarDerivativeOfLinear(-1.1),1.0);
}

TEST(TestRelu,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1.0,1.1,
    0.0,0.0,
    0.0,2;

  EXPECT_TRUE(CompareMatrices(Relu(input),expected));  
}

TEST(TestDerivativeOfRelu,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1,1,
    1,0,
    0,1;

  EXPECT_TRUE(CompareMatrices(DerivativeOfRelu(input),expected));  
}

TEST(TestLeakyRelu,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1.0,1.1,
    0.0,(-1.0*convnet::leaky_relu_param),
    (-1.1*convnet::leaky_relu_param),2;
  
  EXPECT_TRUE(CompareMatrices(LeakyRelu(input),expected));  
}

TEST(TestDerivativeOfLeakyRelu,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1,1,
    1,(convnet::leaky_relu_param),
    (convnet::leaky_relu_param),1;

  EXPECT_TRUE(CompareMatrices(DerivativeOfLeakyRelu(input),expected));  
}

TEST(TestSigmoid,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    0.7310585786300049,0.7502601055951176,
    0.5,0.2689414213699951,
    0.24973989440488237,0.8807970779778824;

  EXPECT_TRUE(CompareMatrices(Sigmoid(input),expected));  
}

TEST(TestDerivativeOfSigmoid,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    0.19661193324148185,0.1873698795475206,
    0.25,0.19661193324148185,
    0.18736987954752057,0.10499358540350653;

  EXPECT_TRUE(CompareMatrices(DerivativeOfSigmoid(input),expected));  
}

TEST(TestLinear,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;
    
  EXPECT_TRUE(CompareMatrices(Linear(input),expected));  
}


TEST(TestDerivativeOfLinear,DoesEqual){
  Eigen::MatrixXf input(3,2);
  Eigen::MatrixXf expected(3,2);

  input <<
    1.0,1.1,
    0.0,-1.0,
    -1.1,2;

  expected <<
    1,1,
    1,1,
    1,1;
  
  EXPECT_TRUE(CompareMatrices(DerivativeOfLinear(input),expected));  
}
