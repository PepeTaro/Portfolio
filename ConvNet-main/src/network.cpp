#include "network.h"

namespace convnet{

  Network::Network(const std::array<int,3>& input_shape,convnet::Losses loss_type):loss_type_(loss_type),is_training(false){
    output_shapes_.push_back(input_shape);
  }
  
  Network::Network(const std::array<int,3>& input_shape,optimizers::Optimizer* optimizer,convnet::Losses loss_type):loss_type_(loss_type),is_training(true){
    output_shapes_.push_back(input_shape);
    optimizer_ = optimizer;        
  }
  
  void Network::addConv(const std::array<int,3>& kernel_shape,const std::array<int,2>& stride_shape,const std::array<int,2>& pad_shape,
			convnet::Activations activation_type){
    
    int output_height,output_width,output_depth;
    std::array<int,3> input_shape;
    std::array<int,6> filter_shape;
    
    input_shape = output_shapes_.back(); // 一つ前のレイヤーをシェイプを取り出す。

    // 一つ前のレイヤーの情報をもとにアウトプットのシェイプを計算
    output_height = (input_shape[0] - kernel_shape[0] + 2*pad_shape[0])/stride_shape[0] + 1;
    output_width  = (input_shape[1] - kernel_shape[1] + 2*pad_shape[1])/stride_shape[1] + 1;
    output_depth  = kernel_shape[2];
    
    // filter_shape = {ker_h,ker_w,input_channel,output_channel,stride_h,stride_w} であることに注意。
    // メンバ変数に情報を格納
    filter_shape = {kernel_shape[0],kernel_shape[1],input_shape[2],kernel_shape[2],stride_shape[0],stride_shape[1]};
    convs_.push_back(convnet::Conv(input_shape,filter_shape,pad_shape,activation_type));
    layer_types_.push_back(LayerType::Conv);
    output_shapes_.push_back({output_height,output_width,output_depth});
  }

  void Network::addPooling(const std::array<int,2>& pooling_shape,const std::array<int,2>& stride_shape){
    int output_height,output_width,output_depth;
    std::array<int,3> input_shape;
    std::array<int,4> pooling_stride_shape;
      
    input_shape = output_shapes_.back(); // 一つ前のレイヤーをシェイプを取り出す。

    // 一つ前のレイヤーの情報をもとにアウトプットのシェイプを計算
    output_height = (input_shape[0] - pooling_shape[0])/stride_shape[0] + 1;
    output_width  = (input_shape[1] - pooling_shape[1])/stride_shape[1] + 1;
    output_depth  = input_shape[2];

    // メンバ変数に情報を格納
    pooling_stride_shape = {pooling_shape[0],pooling_shape[1],stride_shape[0],stride_shape[1]};
    poolings_.push_back(convnet::Pooling(input_shape,pooling_stride_shape));
    layer_types_.push_back(LayerType::Pooling);
    output_shapes_.push_back({output_height,output_width,output_depth});
  }

  void Network::addDense(int output_size,convnet::Activations activation_type,float dropout_prob){
    std::array<int,3> input_shape;
    int input_size;
    
    input_shape = output_shapes_.back();// 一つ前のレイヤーをシェイプを取り出す。
    if(input_shape[1] == -1 and input_shape[2] == -1){ // ひとつ前のレイヤーがDenseだった場合
      input_size = input_shape[0];//一つ前のレイヤーがDenseの場合,input_shapeは,{input_size,-1,-1}の形をしていることに注意。
    }else{                                            // ひとつ前のレイヤーがConvまたはPoolingだった場合
      input_size = input_shape[0]*input_shape[1]*input_shape[2]; // vectorizeしているため,このinput_sizeとなる。
    }

    // メンバ変数に情報を格納
    denses_.push_back(convnet::Dense(input_size,output_size,activation_type,dropout_prob));
    layer_types_.push_back(LayerType::Dense);
    output_shapes_.push_back({output_size,-1,-1});
  }

  // Netは,後にBackpropすると想定"していない"動作を行う(つまり評価用)
  Eigen::MatrixXf Network::Net(const std::vector<Eigen::MatrixXf>& input) const{
    
    // inputのサイズが適しているかチェック。
    assert(input[0].rows() == output_shapes_[0][0] and input[0].cols() == output_shapes_[0][1] and input.size() == output_shapes_[0][2]);
    assert(layer_types_.back() == LayerType::Dense); // レイヤーの最後は,Denseでなくてはいけない。

    bool is_dense=false; // Denseレイヤーの"領域"に入ったことを記録するための変数
    int conv_count=0;
    int pooling_count=0;
    int dense_count=0;
    std::vector<Eigen::MatrixXf> output;

    output = input;
    // Denseレイヤー領域に入った場合,即座にfor loopから抜け出す(not is_dense の部分に相当)ことに注意(テンソルをFlattenする必要があるため)
    for(int l=0;l<layer_types_.size() and not is_dense;++l){ // 各レイヤーを順に取り出し,レイヤーの種類に応じて,各々のForwardを呼び出す。
      switch(layer_types_[l]){
      case LayerType::Conv:
	output = convs_[conv_count++].Net(output);
	break;
      case LayerType::Pooling:
	output = poolings_[pooling_count++].Net(output);
	break;
      case LayerType::Dense: // Denseレイヤーの"領域"に入った場合,forループから抜ける。(outputをvectrizeしなくてはいけないため)
	is_dense = true;
	break;
      defaut:
	std::cerr << "[!]そのようなLayerTypeは存在しません" << std::endl;
	exit(-1);	
	break;
      }
    }
    
    Eigen::MatrixXf output_dense = convnet::Im2Dense(output); // ランク3テンソルであるoutputをFlattenする。
    for(int i=0;i<denses_.size();++i){
      output_dense = denses_[i].Net(output_dense);
    }
    
    return output_dense;    
  }

  // Forwardは,後にBackpropすると想定した動作を行う(つまり学習用)
  Eigen::MatrixXf Network::Forward(const std::vector<Eigen::MatrixXf>& input){

    if(is_training == false){
      std::cerr << "Error(Network::Forward)Modelが評価用のためForward関数は使用できません" << std::endl;
      exit(-1);
    }
    
    // inputのサイズが適しているかチェック。
    assert(input[0].rows() == output_shapes_[0][0] and input[0].cols() == output_shapes_[0][1] and input.size() == output_shapes_[0][2]);
    assert(layer_types_.back() == LayerType::Dense); // レイヤーの最後は,Denseでなくてはいけない。

    bool is_dense=false; // Denseレイヤーの"領域"に入ったことを記録するための変数
    int conv_count=0;
    int pooling_count=0;
    int dense_count=0;
    std::vector<Eigen::MatrixXf> output;
    
    output = input;
    // Denseレイヤー領域に入った場合,即座にfor loopから抜け出す(not is_dense の部分に相当)ことに注意(テンソルをFlattenする必要があるため)
    for(int l=0;l<layer_types_.size() and not is_dense;++l){ // 各レイヤーを順に取り出し,レイヤーの種類に応じて,各々のForwardを呼び出す。
      switch(layer_types_[l]){
      case LayerType::Conv:
	output = convs_[conv_count++].Forward(output);
	break;
      case LayerType::Pooling:
	output = poolings_[pooling_count++].Forward(output);
	break;
      case LayerType::Dense: // Denseレイヤーの"領域"に入った場合,forループから抜ける。(outputをvectrizeしなくてはいけないため)
	is_dense = true;
	break;
      defaut:
	std::cerr << "[!]そのようなLayerTypeは存在しません" << std::endl;
	exit(-1);
	break;
      }
    }
    
    Eigen::MatrixXf output_dense = convnet::Im2Dense(output); // ランク3テンソルであるoutputをflattenする。
    for(int i=0;i<denses_.size();++i){
      output_dense = denses_[i].Forward(output_dense);
    }
    
    return output_dense;    
  }

  void Network::Backward(const Eigen::VectorXf& output,const Eigen::VectorXf& label){

    if(is_training == false){
      std::cerr << "Error(Network::Backward)Modelが評価用のためForward関数は使用できません" << std::endl;
      exit(-1);
    }

    assert(output.size() == label.size()); // もちろん,outputとlabelの次元が一致しなくてはいけない。

    // OptimizerのタイムステップとBactchCount(処理したデータ数)をインクリメント
    optimizer_->IncrementTimeStep();
    optimizer_->IncrementBatchCount();
    
    Eigen::MatrixXf error = GradientOfLoss(output,label); // Loss関数のエラーを取得。    
    
    for(int i=denses_.size()-1;i>=0;--i){  // まずDenseレイヤーの"領域"のみをBackwardする。(Conv,Poolingの"領域"ではエラーがテンソルである必要があるため)
      error = denses_[i].Backward(error,optimizer_);
    }
    
    int conv_pooling_idx = output_shapes_.size() - denses_.size() - 1; // Denseレイヤー"領域"の一つ前のレイヤー(つまり,Conv or Pooling)のインデックスを計算。
    std::vector<Eigen::MatrixXf> errors = convnet::Dense2Im(error,output_shapes_[conv_pooling_idx]);// Denseレイヤーのエラーをランク3テンソルに変換。
    
    int conv_idx=convs_.size()-1;
    int pooling_idx=poolings_.size()-1;
    for(int l=conv_pooling_idx;l >=0;--l){ // Conv,Poolingレイヤー"領域"をBackwardしていく。
      switch(layer_types_[l]){
      case LayerType::Conv:
	errors = convs_[conv_idx--].Backward(errors,optimizer_);
	break;
      case LayerType::Pooling:
	errors = poolings_[pooling_idx--].Backward(errors);
	break;	
      defaut:	
	std::cerr << "[!]そのようなLayerTypeは存在しないか,あるいはLayerの順序が正しくありません" << std::endl;
	exit(-1);
	break;
      }
    }
    
  }

  float Network::Loss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const{
    return GetLoss(output,label,loss_type_); // loss_type_の種類に応じた,Loss関数の値を返す。
  }
  
  Eigen::MatrixXf Network::GradientOfLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const{
    return GetGradientOfLoss(output,label,loss_type_); // loss_type_の種類に応じた,Loss関数のGradientを返す。
  }

  void Network::setDenseWeight(int index,const Eigen::MatrixXf& weight){
    if(index == -1){
      denses_[denses_.size()-1].setWeight(weight);
    }else{
      assert(index < denses_.size());
      denses_[index].setWeight(weight);
    }    
  }
  
  void Network::setDenseBias(int index,const Eigen::MatrixXf& bias){
    if(index == -1){
      denses_[denses_.size()-1].setBias(bias);
    }else{
      assert(index < denses_.size());
      denses_[index].setBias(bias);
    }
  }
  
  void Network::setConvKernel(int index,const Eigen::MatrixXf& kernel){
    if(index == -1){
      convs_[convs_.size()-1].setKernel(kernel);
    }else{
      assert(index < convs_.size());
      convs_[index].setKernel(kernel);
    }    
  }
  
  void Network::setConvBias(int index,const Eigen::MatrixXf& bias){
    if(index == -1){
      convs_[convs_.size()-1].setBias(bias);
    }else{
      assert(index < convs_.size());
      convs_[index].setBias(bias);
    }
  }

  Dense Network::getDense(int index) const{
    assert(index >= 0 and index < denses_.size());
    return denses_[index];    
  }
  
  Pooling Network::getPooling(int index) const{
    assert(index >= 0 and index < poolings_.size());
    return poolings_[index];
  }
  
  Conv Network::getConv(int index) const{
    assert(index >= 0 and index < convs_.size());
    return convs_[index];
  }

  Losses Network::getLossType() const{
    return loss_type_;
  }
  
  std::vector<LayerType> Network::getLayerTypes() const{
    return layer_types_;
  }
  
  std::vector<std::array<int,3>> Network::getOutputShapes() const{
    return output_shapes_;
  }  

};//namespace convnet
