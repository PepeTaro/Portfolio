#include "weight_init.h"

namespace convnet::init{

  Eigen::MatrixXf InitKernel(const std::array<int,4>& kernel_shape,Activations activation_type){
    Eigen::MatrixXf kernel;
    int num_input,num_output;
    num_input = kernel_shape[0]*kernel_shape[1]*kernel_shape[2];
    num_output = kernel_shape[3];
    
    switch(activation_type){
    case Activations::LeakyRelu:
      kernel = Kaiming({num_input,num_output},num_input);
      break;
    case Activations::Relu:
      kernel = Kaiming({num_input,num_output},num_input);
      break;
    case Activations::Sigmoid:
      kernel = Xavier({num_input,num_output},num_input,num_output);
      break;
    case Activations::Tanh:
      kernel = Xavier({num_input,num_output},num_input,num_output);
      break;
    case Activations::Linear:
      kernel = Xavier({num_input,num_output},num_input,num_output);
      break;
    default:
      std::cerr << "[!] Error(InitKernel): そのようなActivation関数は存在しません。" << std::endl;
      exit(-1);
      break;
    }  
    return kernel;    

  }
  
  Eigen::MatrixXf InitWeight(int num_input,int num_output,Activations activation_type){
    Eigen::MatrixXf weight;
    
    switch(activation_type){
    case Activations::LeakyRelu:
      weight = Kaiming({num_output,num_input},num_input);
      break;
    case Activations::Relu:
      weight = Kaiming({num_output,num_input},num_input);
      break;
    case Activations::Sigmoid:
      weight = Xavier({num_output,num_input},num_input,num_output);
      break;
    case Activations::Tanh:
      weight = Xavier({num_output,num_input},num_input,num_output);
      break;
    case Activations::Linear:
      weight = Xavier({num_output,num_input},num_input,num_output);
      break;
    default:
      std::cerr << "[!] Error(InitWeight): そのようなActivation関数は存在しません。" << std::endl;
      exit(-1);
      break;
    }  
    return weight;
    
  }
  
  Eigen::VectorXf InitBias(int num_output,float epsilon){
    Eigen::VectorXf bias;
    bias = Eigen::VectorXf::Constant(num_output,epsilon);

    return bias;
  }
  
  Eigen::MatrixXf Kaiming(const std::array<int,2>& mat_shape,int num_input){
    Eigen::MatrixXf output(mat_shape[0],mat_shape[1]);
    float mean = 0;
    float stddev = sqrt(2.0/num_input);

    for(int i=0;i<mat_shape[0];++i){
      for(int j=0;j<mat_shape[1];++j){	
	output(i,j) = Gaussian(mean,stddev); 
      }
    }
    
    return output;
  }
  
  Eigen::MatrixXf Xavier(const std::array<int,2>& mat_shape,int num_input,int num_output){
    Eigen::MatrixXf output(mat_shape[0],mat_shape[1]);
    float bound = sqrt(6.0/(num_input+num_output));
	
    for(int i=0;i<mat_shape[0];++i){
      for(int j=0;j<mat_shape[1];++j){
	output(i,j) = Uniform(-bound,+bound);
      }
    }
    
    return output;
  }

  float Uniform(float lower_bound,float upper_bound){
    std::uniform_real_distribution<float> distribution(lower_bound,upper_bound);
    return distribution(generator);
  }
  
  float Gaussian(float mean,float stddev){
    std::normal_distribution<float> distribution(mean,stddev);
    return distribution(generator);
  }
  
};//namespace convnet
