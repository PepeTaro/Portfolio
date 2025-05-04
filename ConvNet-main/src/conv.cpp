#include "conv.h"

namespace convnet{  
  
  Conv::Conv(const std::array<int,3>& input_shape,
	     const std::array<int,6>& kernel_shape,
	     const std::array<int,2>& pad_shape,
	     Activations activation_type):input_shape_(input_shape),kernel_shape_(kernel_shape),pad_shape_(pad_shape),
							 activation_type_(activation_type){

    // 引数に応じて,アウトプットの型を計算
    output_shape_[0] = (input_shape_[0] - kernel_shape_[0] + 2*pad_shape_[0])/kernel_shape_[4] + 1;
    output_shape_[1] = (input_shape_[1] - kernel_shape_[1] + 2*pad_shape_[1])/kernel_shape_[5] + 1;
    output_shape_[2] = kernel_shape_[3];

    // Assertチェック
    assert(isDivisible((input_shape_[0] - kernel_shape_[0] + 2*pad_shape_[0]),kernel_shape_[4]) and
	   isDivisible((input_shape_[1] - kernel_shape_[1] + 2*pad_shape_[1]),kernel_shape_[5]));    // outputの行数,列数は整数か
    assert(input_shape_[2] == kernel_shape_[2]);                                                     // inputのチャンネル数とkernelのチャンネル数は同じか

    // パラメーターを初期化
    input_ = Eigen::MatrixXf(kernel_shape_[0]*kernel_shape_[1]*kernel_shape_[2],output_shape_[0]*output_shape_[1]);
    kernel_ = init::InitKernel({kernel_shape_[0],kernel_shape_[1],kernel_shape_[2],kernel_shape_[3]},activation_type_);
    bias_ = init::InitBias(kernel_shape_[3]);

    input_.setZero();
    kernel_.setRandom();
    bias_.setZero();
    
    momentum_dw_ = Eigen::MatrixXf::Zero(kernel_shape_[0]*kernel_shape_[1]*kernel_shape_[2],kernel_shape_[3]);
    momentum_db_ = Eigen::VectorXf::Zero(kernel_shape_[3]);
    
    momentum_dw1_ = Eigen::MatrixXf::Zero(kernel_shape_[0]*kernel_shape_[1]*kernel_shape_[2],kernel_shape_[3]);
    momentum_db1_ = Eigen::VectorXf::Zero(kernel_shape_[3]);
    momentum_dw2_ = Eigen::MatrixXf::Zero(kernel_shape_[0]*kernel_shape_[1]*kernel_shape_[2],kernel_shape_[3]);
    momentum_db2_ = Eigen::VectorXf::Zero(kernel_shape_[3]);
    
    minibatch_dw_ = Eigen::MatrixXf::Zero(kernel_shape_[0]*kernel_shape_[1]*kernel_shape_[2],kernel_shape_[3]);
    minibatch_db_ = Eigen::VectorXf::Zero(kernel_shape_[3]);
  }
  
  // Local Response Normalizationを行うメソッド。
  Eigen::MatrixXf Conv::Normalize(const Eigen::MatrixXf& input) const{
    static float alpha = 1e-4;
    static float beta  = 0.75;        
    static float k = 2;    
    Eigen::ArrayXf denom;    
    Eigen::MatrixXf output;
    
    denom = pow(k + alpha*(pow(input.array(),2).colwise().sum()),beta);
    output = input.array().rowwise()/denom.transpose();
    
    return output;
  }
  
  Eigen::MatrixXf Conv::Activation(const Eigen::MatrixXf& input) const{
    return ApplyActivation(input,activation_type_); // activation_type_の種類に応じて,inputにActivation関数を適用
  }

  Eigen::MatrixXf Conv::DerivativeOfActivation(const Eigen::MatrixXf& input) const{
    return ApplyDerivativeOfActivation(input,activation_type_); // activation_type_の種類に応じて,点inputでのActivationの微分を計算
  }

  // メンバ変数pad_shape_の情報を元に,引数inputにゼロをPaddingする
  std::vector<Eigen::MatrixXf> Conv::Padding(const std::vector<Eigen::MatrixXf>& input) const{      
    std::vector<Eigen::MatrixXf> output;      
    Eigen::MatrixXf padded_input(input_shape_[0]+2*pad_shape_[0],input_shape_[1]+2*pad_shape_[1]);      
    for(int i=0;i<input_shape_[2];++i){
      padded_input.setZero();                                                                                                                                       
      padded_input.block(pad_shape_[0],pad_shape_[1],input_shape_[0],input_shape_[1]) = input[i];
      output.push_back(padded_input);
    }
      
    return output;
  }

  //メンバ変数pad_shape_の情報を元に,引数padded_inputからPaddingされた部分を取り除く(つまり,Paddingメソッドの逆操作をする)
  std::vector<Eigen::MatrixXf> Conv::UnPadding(const std::vector<Eigen::MatrixXf>& padded_input) const{
    std::vector<Eigen::MatrixXf> output;      
    Eigen::MatrixXf unpadded_input(input_shape_[0],input_shape_[1]);
      
    for(int i=0;i<input_shape_[2];++i){                                                                                                
      unpadded_input = padded_input[i].block(pad_shape_[0],pad_shape_[1],input_shape_[0],input_shape_[1]); 
      output.push_back(unpadded_input);	
    }
    return output;
  }

  std::vector<Eigen::MatrixXf> Conv::Net(const std::vector<Eigen::MatrixXf>& input) const{
    // Assertチェック(input内のすべての行列が同じ形をしていると仮定していることに注意。)
    assert(input[0].rows() == input_shape_[0] and input[0].cols() == input_shape_[1] and input.size() == input_shape_[2]);
      
    std::vector<Eigen::MatrixXf> padded_input = Padding(input); // Paddingが必要な場合,inputに0をpaddingする
    std::array<int,5> kernel_shape = {kernel_shape_[0],kernel_shape_[1],kernel_shape_[2],kernel_shape_[4],kernel_shape_[5]};
    Eigen::MatrixXf input_mat = Im2Col(padded_input,kernel_shape); // GEMMを行うためにinputを行列に変換。
      
    Eigen::MatrixXf pre_activation = ((kernel_.transpose())*input_mat);//GEMMを行う。  
    pre_activation.colwise() += bias_;// biasの影響を追加。

    Eigen::MatrixXf output_mat = Normalize(Activation(pre_activation));//Activation関数を適用した後にNormalizationを行う。
    return Col2Im(output_mat,output_shape_); // 計算結果は行列のままなので,テンソルの形に戻す。
  }

  std::vector<Eigen::MatrixXf> Conv::Forward(const std::vector<Eigen::MatrixXf>& input){
    // Assertチェック(input内のすべての行列が同じ形をしていると仮定していることに注意。)
    assert(input[0].rows() == input_shape_[0] and input[0].cols() == input_shape_[1] and input.size() == input_shape_[2]);

    std::vector<Eigen::MatrixXf> padded_input = Padding(input); // Paddingが必要な場合,inputに0をpaddingする
    input_ = Im2Col(padded_input,{kernel_shape_[0],kernel_shape_[1],kernel_shape_[2],kernel_shape_[4],kernel_shape_[5]}); // GEMMを行うためにinputを行列に変換。
      
    pre_activation_ = ((kernel_.transpose())*input_); // GEMMを行う。  
    pre_activation_.colwise() += bias_;               // biasの影響を追加。

    Eigen::MatrixXf output_mat = Normalize(Activation(pre_activation_));           // Activation関数を適用した後にNormalizationを行う。
    return Col2Im(output_mat,{output_shape_[0],output_shape_[1],output_shape_[2]}); // 計算結果は行列のままなので,テンソルの形に戻す。
  }

  std::vector<Eigen::MatrixXf> Conv::Backward(const std::vector<Eigen::MatrixXf>& dout,optimizers::Optimizer* optimizer){
    // Assertチェック(dout内のすべての行列が同じ形をしていると仮定していることに注意。)
    assert(dout[0].rows() == output_shape_[0] and dout[0].cols() == output_shape_[1] and dout.size() == output_shape_[2]);
    
    Eigen::MatrixXf dout_mat = Im2Col(dout,{1,1,(int)dout.size(),1,1}).transpose();                // doutをテンソルから行列に変換
    dout_mat = (dout_mat.array()*(DerivativeOfRelu(pre_activation_).transpose().array())).matrix();//doutはActivation後のエラーなので,Activation前のエラーに変換
    
    //以下の２行は,メンバ変数input_に対応するエラーを計算している。
    Eigen::MatrixXf dout_w = dout_mat*(kernel_.transpose());            
    std::vector<Eigen::MatrixXf> dx = Col2ImBack(dout_w,{input_shape_[0]+2*pad_shape_[0],input_shape_[1]+2*pad_shape_[1],input_shape_[2]},output_shape_,
						 {kernel_shape_[0],kernel_shape_[1],kernel_shape_[2],kernel_shape_[4],kernel_shape_[5]});
    
    dx = UnPadding(dx); // 元のエラーdxは,paddingされた状態のエラーなので,不必要な部分を取り除く。
      
    Eigen::MatrixXf dw = input_*dout_mat;// weight_のgradientを計算
    Eigen::VectorXf db = dout_mat.colwise().sum();// bias_のgradientを計算
        
    Update(dw,db,optimizer);
    
    return dx;
  }
  
  void Conv::Update(const Eigen::MatrixXf& dw,const Eigen::MatrixXf& db,optimizers::Optimizer* optimizer){    
    if(optimizer->getType() == optimizers::optimizer_type::Sgd){
      optimizer->SgdUpdate(dw,db,minibatch_dw_,minibatch_db_,kernel_,bias_,momentum_dw_,momentum_db_);
    }else if(optimizer->getType() == optimizers::optimizer_type::Adam){
      optimizer->AdamUpdate(dw,db,minibatch_dw_,minibatch_db_,kernel_,bias_,momentum_dw1_,momentum_db1_,momentum_dw2_,momentum_db2_);
    }else if(optimizer->getType() == optimizers::optimizer_type::AdamW){
      optimizer->AdamWUpdate(dw,db,minibatch_dw_,minibatch_db_,kernel_,bias_,momentum_dw1_,momentum_db1_,momentum_dw2_,momentum_db2_);
    }else{
      std::cerr << "Error:(Conv::Update) そのようなOptimizerは存在しません" << std::endl;
      exit(-1);
    }
  }

  Activations Conv::getActivationType() const{
    return activation_type_;
  }
  
  std::array<int,3> Conv::getInputShape() const{
    return input_shape_;
  }
  
  std::array<int,6> Conv::getKernelShape() const{
    return kernel_shape_;
  }
  
  std::array<int,3> Conv::getOutputShape() const{
    return output_shape_;
  }
  
  std::array<int,2> Conv::getPadShape() const{
    return pad_shape_;
  }
  
  Eigen::MatrixXf Conv::getKernel() const{
    return kernel_;
  }
  
  Eigen::VectorXf Conv::getBias() const{
    return bias_;
  }
  
  void Conv::setKernel(const Eigen::MatrixXf& kernel){
    assert(kernel_.rows() == kernel.rows());
    assert(kernel_.cols() == kernel.cols());
    
    kernel_ = kernel;
  }
  
  void Conv::setBias(const Eigen::VectorXf& bias){
    assert(bias_.size() == bias.size());
    
    bias_ = bias;
  }
  
};//namespace convnet

