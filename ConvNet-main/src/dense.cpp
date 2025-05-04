#include "dense.h"

namespace convnet{
  
  Dense::Dense(int input_size,int output_size,Activations activation_type,float dropout_prob)
    :input_size_(input_size),output_size_(output_size),activation_type_(activation_type),dropout_prob_(dropout_prob){
    
    // Assertチェック
    assert(input_size_ > 0 and output_size_ > 0);
    assert(dropout_prob_ >= 0.0 and dropout_prob_ <= 1.0);//dropout_prob_は確率なので,0.0から1.0
      
    // パラメーターを初期化
    weight_ = init::InitWeight(input_size_,output_size_,activation_type_);
    bias_   = init::InitBias(output_size_);

    momentum_dw_   = Eigen::MatrixXf::Zero(output_size_,input_size_);
    momentum_db_   = Eigen::VectorXf::Zero(output_size_);
    
    momentum_dw1_ = Eigen::MatrixXf::Zero(output_size_,input_size_);
    momentum_db1_ = Eigen::VectorXf::Zero(output_size_);
    momentum_dw2_ = Eigen::MatrixXf::Zero(output_size_,input_size_);;
    momentum_db2_ = Eigen::VectorXf::Zero(output_size_);

    minibatch_dw_   = Eigen::MatrixXf::Zero(output_size_,input_size_);
    minibatch_db_   = Eigen::VectorXf::Zero(output_size_);

  }
  
  Eigen::MatrixXf Dense::Activation(const Eigen::MatrixXf& input) const{
    return ApplyActivation(input,activation_type_);        
  }

  Eigen::MatrixXf Dense::DerivativeOfActivation(const Eigen::MatrixXf& input) const{
    return ApplyDerivativeOfActivation(input,activation_type_);        
  }

  Eigen::MatrixXf Dense::Dropout(const Eigen::MatrixXf& input) const{
    Eigen::ArrayXf prob_0to1;
    Eigen::ArrayXf mask;
    Eigen::ArrayXf factor;

    // 以下のコードは,[0.0,1.0]から一様に,input_size_個サンプリングし,prob_0to1にサンプリングした値を(ベクトルの形で)格納。
    prob_0to1 = 0.5*(Eigen::ArrayXf::Random(input_size_,1) + Eigen::ArrayXf::Constant(input_size_,1,1.0));
    mask = (prob_0to1 <= dropout_prob_).cast<float>();      
    return (input.array()*mask).matrix();      
  }

  Eigen::MatrixXf Dense::Net(const Eigen::MatrixXf& input) const{
    // Assertチェック
    assert(input.cols() == 1 and input.rows() == input_size_);   
    return Activation((dropout_prob_*weight_)*input + bias_); // Dropoutの仕様のため,weight_にdropout_prob_を掛けていることに注意
  }

  Eigen::MatrixXf Dense::Forward(const Eigen::MatrixXf& input){
    // Assertチェック
    assert(input.cols() == 1 and input.rows() == input_size_);
    
    input_ = Dropout(input); // Forward関数が学習時に使用するため,Dropoutを行う
    pre_activation_ = weight_*input_ + bias_;        
    return Activation(pre_activation_);
  }

  // doutは,メンバ変数output_に対応するエラー。  
  Eigen::MatrixXf Dense::Backward(const Eigen::MatrixXf& dout,optimizers::Optimizer* optimizer){
    // Assertチェック
    assert(dout.cols() == 1 and dout.rows() == output_size_);
    
    // エラーを計算
    Eigen::MatrixXf activation_error = DerivativeOfActivation(pre_activation_).asDiagonal();
    Eigen::MatrixXf dx  = activation_error*dout;       // メンバ変数input_に対応するエラーを計算
    Eigen::MatrixXf dw = dx*(input_.transpose());      // weight_のgradientを計算
    Eigen::MatrixXf error = (weight_.transpose())*dx;  // 一つ前のレイヤーに渡すエラーを計算。

    Update(dw,dx,optimizer);
    
    return error;
  }

  void Dense::Update(const Eigen::MatrixXf& dw,const Eigen::MatrixXf& db,optimizers::Optimizer* optimizer){
    
    if(optimizer->getType() == optimizers::optimizer_type::Sgd){
      optimizer->SgdUpdate(dw,db,minibatch_dw_,minibatch_db_,weight_,bias_,momentum_dw_,momentum_db_);
    }else if(optimizer->getType() == optimizers::optimizer_type::Adam){
      optimizer->AdamUpdate(dw,db,minibatch_dw_,minibatch_db_,weight_,bias_,momentum_dw1_,momentum_db1_,momentum_dw2_,momentum_db2_);
    }else if(optimizer->getType() == optimizers::optimizer_type::AdamW){
      optimizer->AdamWUpdate(dw,db,minibatch_dw_,minibatch_db_,weight_,bias_,momentum_dw1_,momentum_db1_,momentum_dw2_,momentum_db2_);
    }else{
      std::cerr << "Error:(Dense::Update) そのようなOptimizerは存在しません" << std::endl;
      exit(-1);
    }
  }
  
  int Dense::getInputSize() const{
    return input_size_;
  }
  
  int Dense::getOutputSize() const{
    return output_size_;
  }
  
  float Dense::getDropoutProb() const{
    return dropout_prob_;
  }
  
  Activations Dense::getActivationType() const{
    return activation_type_;
  }
  
  Eigen::MatrixXf Dense::getWeight() const{
    return weight_;
  }
  
  Eigen::VectorXf Dense::getBias() const{
    return bias_;
  }

  void Dense::setWeight(const Eigen::MatrixXf& weight){
    assert(weight_.rows() == weight.rows());
    assert(weight_.cols() == weight.cols());
    
    weight_ = weight;
  }
  
  void Dense::setBias(const Eigen::VectorXf& bias){
    assert(bias_.size() == bias.size());
    
    bias_ = bias;
  }

};//namespace convnet
