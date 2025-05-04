#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>
#include <array>

#include <Eigen/Dense>

#include "conv.h"
#include "dense.h"
#include "pooling.h"
#include "losses.h"
#include "activations.h"
#include "im2col.h"
#include "optimizer.h"

namespace convnet{
  enum class LayerType {Conv,Pooling,Dense};
  
  class Network{
  protected:
    bool is_training; // trueの場合は,Forward,Backwardが使用できる,flaseの場合はNetのみ
    
    std::vector<convnet::Conv> convs_;            // Convレイヤーを格納している
    std::vector<convnet::Pooling> poolings_;      // Poolingレイヤーを格納している
    std::vector<convnet::Dense> denses_;          // Denseレイヤーを格納している
    std::vector<LayerType> layer_types_;          // 各レイヤーの種類を保存するvector
    std::vector<std::array<int,3>> output_shapes_;// Conv,Poolingは,{height,width,depth}の形だが,Denseの場合は{output_size,-1,-1}としている。
    
    convnet::Losses loss_type_;// Loss関数の種類
    optimizers::Optimizer* optimizer_;
    
  public:
    Network(const std::array<int,3>& input_shape,convnet::Losses loss_type);
    Network(const std::array<int,3>& input_shape,optimizers::Optimizer* optimizer,convnet::Losses loss_type=convnet::Losses::CategoricalCrossEntropy);
    
    void addConv(const std::array<int,3>& kernel_shape,const std::array<int,2>& stride_shape,const std::array<int,2>& pad_shape={0,0},
		 convnet::Activations activation_type=convnet::Activations::LeakyRelu);
    
    void addPooling(const std::array<int,2>& pooling_shape,const std::array<int,2>& stride_shape);
    void addDense(int output_size,convnet::Activations activation_type=convnet::Activations::Linear,float dropout_prob=1.0);
    
    Eigen::MatrixXf Net(const std::vector<Eigen::MatrixXf>& input) const;
    Eigen::MatrixXf Forward(const std::vector<Eigen::MatrixXf>& input);
    void Backward(const Eigen::VectorXf& output,const Eigen::VectorXf& label);
    
    float Loss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const;  
    Eigen::MatrixXf GradientOfLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const;
    
    void setDenseWeight(int index,const Eigen::MatrixXf& weight);
    void setDenseBias(int index,const Eigen::MatrixXf& bias);
    void setConvKernel(int index,const Eigen::MatrixXf& kernel);
    void setConvBias(int index,const Eigen::MatrixXf& bias);

    Dense getDense(int index) const;
    Pooling getPooling(int index) const;
    Conv getConv(int index) const;

    Losses getLossType() const;
    std::vector<LayerType> getLayerTypes() const;
    std::vector<std::array<int,3>> getOutputShapes() const;    
    
  };//class Network
  
};//namespace convnet

#endif// NETWORK_H
