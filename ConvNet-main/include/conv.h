#ifndef CONV_H
#define CONV_H

#include <vector>
#include <array>
#include <cassert>
#include <Eigen/Dense>

#include "im2col.h"
#include "activations.h"
#include "losses.h"
#include "utils.h"
#include "weight_init.h"
#include "optimizer.h"

namespace convnet{
  
  class Conv{
  protected:
    float learning_rate_;
    float weight_decay_;
    Activations activation_type_;
    
    Eigen::MatrixXf input_;  // Forward関数のインプット行列を保存するための変数。
    Eigen::MatrixXf kernel_; // kernel_は行列の形で保存していることに注意。(素朴な実装だとテンソル)
    Eigen::VectorXf bias_;
  
    std::array<int,3> input_shape_;  // inputの形を保存した配列{input_height,input_width,input_depth}
    std::array<int,6> kernel_shape_; // kernel(厳密にはfilter)の形を保存した配列{kernel_height,kernel_width,kernel_depth,num_of_filters,stride_height,stride_width}
    std::array<int,3> output_shape_; // outputの形を保存した配列{output_height,output_width,output_depth}
    std::array<int,2> pad_shape_;    // paddingの形を保存{pad_height,pad_width}
    Eigen::MatrixXf pre_activation_; // Activation関数を適用する前の状態を保存するための変数(Backprop時に使用する)

    // 一つ前のタイムステップのmomentumを保存する変数
    Eigen::MatrixXf momentum_dw_;
    Eigen::VectorXf momentum_db_;
    float momentum_;

    // Adam
    Eigen::MatrixXf momentum_dw1_;
    Eigen::VectorXf momentum_db1_;    
    Eigen::MatrixXf momentum_dw2_;
    Eigen::VectorXf momentum_db2_;

    // minibatch gradient descentの際に,gradientを蓄積するための変数
    Eigen::MatrixXf minibatch_dw_;
    Eigen::VectorXf minibatch_db_;
    
  public:
    Conv(const std::array<int,3>& input_shape,
	 const std::array<int,6>& kernel_shape,
	 const std::array<int,2>& pad_shape={0,0},
	 Activations activation_type=Activations::LeakyRelu);
    
    // Local Response Normalizationを行うメソッド。
    Eigen::MatrixXf Normalize(const Eigen::MatrixXf& input) const;   
    Eigen::MatrixXf Activation(const Eigen::MatrixXf& input) const;
    Eigen::MatrixXf DerivativeOfActivation(const Eigen::MatrixXf& input) const;
    
    std::vector<Eigen::MatrixXf> Padding(const std::vector<Eigen::MatrixXf>& input) const;
    std::vector<Eigen::MatrixXf> UnPadding(const std::vector<Eigen::MatrixXf>& padded_input) const;
    
    std::vector<Eigen::MatrixXf> Net(const std::vector<Eigen::MatrixXf>& input) const;
    std::vector<Eigen::MatrixXf> Forward(const std::vector<Eigen::MatrixXf>& input);
    std::vector<Eigen::MatrixXf> Backward(const std::vector<Eigen::MatrixXf>& dout,optimizers::Optimizer* optimizer);
    void Update(const Eigen::MatrixXf& dw,const Eigen::MatrixXf& db,optimizers::Optimizer* optimizer);
      
    Activations getActivationType() const;
    std::array<int,3> getInputShape() const;
    std::array<int,6> getKernelShape() const;
    std::array<int,3> getOutputShape() const;
    std::array<int,2> getPadShape() const;
    Eigen::MatrixXf getKernel() const;
    Eigen::VectorXf getBias() const;

    void setKernel(const Eigen::MatrixXf& kernel);
    void setBias(const Eigen::VectorXf& bias);
    
  };//class Conv

};//namespace convnet

#endif// CONV_H
