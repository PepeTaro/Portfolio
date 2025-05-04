#ifndef DENSE_H
#define DENSE_H

#include <cassert>
#include <Eigen/Dense>

#include "activations.h"
#include "losses.h"
#include "weight_init.h"
#include "optimizer.h"

namespace convnet{
  
  class Dense{
  protected:
    int input_size_;
    int output_size_;
    float learning_rate_;
    float weight_decay_;
    float dropout_prob_;
    Activations activation_type_;
  
    Eigen::MatrixXf input_;          // Forward関数のインプット行列を保存するための変数。
    Eigen::MatrixXf pre_activation_; // Activation関数を適用する前の状態を保存するための変数(Backprop時に使用する)
    Eigen::MatrixXf weight_;
    Eigen::VectorXf bias_;
    
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
    Dense(int input_size,int output_size,Activations activation_type=Activations::Linear,float dropout_prob=1.0);        
    Eigen::MatrixXf Activation(const Eigen::MatrixXf& input) const;
    Eigen::MatrixXf DerivativeOfActivation(const Eigen::MatrixXf& input) const;
    Eigen::MatrixXf Dropout(const Eigen::MatrixXf& input) const;
    Eigen::MatrixXf Net(const Eigen::MatrixXf& input) const;
    Eigen::MatrixXf Forward(const Eigen::MatrixXf& input);
    Eigen::MatrixXf Backward(const Eigen::MatrixXf& dout,optimizers::Optimizer* optimizer);
    void Update(const Eigen::MatrixXf& dw,const Eigen::MatrixXf& db,optimizers::Optimizer* optimizer);
    
    int getInputSize() const;
    int getOutputSize() const;
    float getDropoutProb() const;
    Activations getActivationType() const;
    Eigen::MatrixXf getWeight() const;
    Eigen::VectorXf getBias() const;
    
    void setWeight(const Eigen::MatrixXf& weight);
    void setBias(const Eigen::VectorXf& bias);
    
  };//class Dense

};//namespace convnet

#endif// DENSE_H
