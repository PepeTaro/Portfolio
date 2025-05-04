#include "activations.h"

namespace convnet{

  float ScalarRelu(float x){
    if(x >= 0)
      return x;
    else
      return 0.0;
  }

  float ScalarDerivativeOfRelu(float x){
    if(x >= 0)
      return 1.0;
    else
      return 0.0;
  }

  float ScalarLeakyRelu(float x){
    if(x >= 0)
      return x;
    else
      return leaky_relu_param*x;
  }

  float ScalarDerivativeOfLeakyRelu(float x){
    if(x >= 0)
      return 1.0;
    else
      return leaky_relu_param;
  }

  float ScalarSigmoid(float z){
    return 1.0/(1.0+exp(-z));
  }

  float ScalarDerivativeOfSigmoid(float z){
    return ScalarSigmoid(z)*(1.0 - ScalarSigmoid(z));  
  }

  float ScalarTanh(float z){
    return tanhf(z);
  }

  float ScalarDerivativeOfTanh(float z){
    return (1.0 - powf(tanhf(z),2));
  }

  float ScalarLinear(float x){
    return x;
  }

  float ScalarDerivativeOfLinear(float x){
    return 1;  
  }

  Eigen::MatrixXf Relu(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarRelu);
  }

  Eigen::MatrixXf DerivativeOfRelu(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarDerivativeOfRelu);
  }

  Eigen::MatrixXf LeakyRelu(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarLeakyRelu);
  }

  Eigen::MatrixXf DerivativeOfLeakyRelu(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarDerivativeOfLeakyRelu);
  }

  Eigen::MatrixXf Sigmoid(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarSigmoid);
  }

  Eigen::MatrixXf DerivativeOfSigmoid(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarDerivativeOfSigmoid);
  }

  Eigen::MatrixXf Tanh(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarTanh);
  }

  Eigen::MatrixXf DerivativeOfTanh(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarDerivativeOfTanh);
  }

  Eigen::MatrixXf Linear(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarLinear);
  }

  Eigen::MatrixXf DerivativeOfLinear(const Eigen::MatrixXf& input){
    return input.unaryExpr(&ScalarDerivativeOfLinear);
  }

  Eigen::MatrixXf ApplyActivation(const Eigen::MatrixXf& input,const Activations activation_type){
    switch(activation_type){
    case Activations::LeakyRelu:
      return LeakyRelu(input);
      break;
    case Activations::Relu:
      return Relu(input);
      break;
    case Activations::Sigmoid:
      return Sigmoid(input);
      break;
    case Activations::Tanh:
      return Tanh(input);
      break;
    case Activations::Linear:
      return Linear(input);
      break;
    default:
      std::cerr << "[!] Error(ApplyActivation): そのようなActivation関数は存在しません。" << std::endl;
      exit(-1);
      break;
    }
  }

  Eigen::MatrixXf ApplyDerivativeOfActivation(const Eigen::MatrixXf& input,const Activations activation_type){
    switch(activation_type){
    case Activations::LeakyRelu:
      return DerivativeOfLeakyRelu(input);
      break;
    case Activations::Relu:
      return DerivativeOfRelu(input);
      break;
    case Activations::Sigmoid:
      return DerivativeOfSigmoid(input);
      break;
    case Activations::Tanh:
      return DerivativeOfTanh(input);
      break;
    case Activations::Linear:
      return DerivativeOfLinear(input);
      break;
    default:
      std::cerr << "[!] Error(ApplyDerivativeOfActivation): そのようなActivation関数は存在しません。" << std::endl;
      exit(-1);    
      break;
    }
  }

};//namespace convnet
