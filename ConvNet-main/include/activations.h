#ifndef ACTIVATIONS_H
#define ACTIVATIONS_H

#include <iostream>
#include <Eigen/Dense>
#include <cmath>

namespace convnet{
  
  enum class Activations{LeakyRelu,Relu,Sigmoid,Tanh,Linear};
  const float leaky_relu_param=0.1;

  float ScalarRelu(float x);
  float ScalarDerivativeOfRelu(float x);
  float ScalarLeakyRelu(float x);
  float ScalarDerivativeOfLeakyRelu(float x);
  float ScalarSigmoid(float z);
  float ScalarDerivativeOfSigmoid(float z);
  float ScalarTanh(float x);
  float ScalarDerivativeOfTanh(float x);
  float ScalarLinear(float x);
  float ScalarDerivativeOfLinear(float x);

  Eigen::MatrixXf Relu(const Eigen::MatrixXf& input);
  Eigen::MatrixXf DerivativeOfRelu(const Eigen::MatrixXf& input);
  Eigen::MatrixXf LeakyRelu(const Eigen::MatrixXf& input);
  Eigen::MatrixXf DerivativeOfLeakyRelu(const Eigen::MatrixXf& input);
  Eigen::MatrixXf Sigmoid(const Eigen::MatrixXf& input);
  Eigen::MatrixXf DerivativeOfSigmoid(const Eigen::MatrixXf& input);
  Eigen::MatrixXf Tanh(const Eigen::MatrixXf& input);
  Eigen::MatrixXf DerivativeOfTanh(const Eigen::MatrixXf& input);
  Eigen::MatrixXf Linear(const Eigen::MatrixXf& input);
  Eigen::MatrixXf DerivativeOfLinear(const Eigen::MatrixXf& input);
  Eigen::MatrixXf ApplyActivation(const Eigen::MatrixXf& input,Activations activation_type);
  Eigen::MatrixXf ApplyDerivativeOfActivation(const Eigen::MatrixXf& input,Activations activation_type);
  
};//namespace convnet

#endif// ACTIVATIONS_H
