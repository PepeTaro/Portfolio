#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <vector>
#include <array>
#include <Eigen/Dense>

#include "conv.h"

#include "dense.hpp"
#include "pooling.hpp"
#include "activations.hpp"
#include "losses.hpp"
#include "im2col.hpp"

namespace convnet{
  class Network{
  protected:
    enum class LayerType {Conv,Pooling,Dense};
  
    std::vector<convnet::Conv> convs_;            // Convレイヤーを格納している
    std::vector<convnet::Pooling> poolings_;      // Poolingレイヤーを格納している
    std::vector<convnet::Dense> denses_;          // Denseレイヤーを格納している
    std::vector<LayerType> layer_types_;          // 各レイヤーの種類を保存するvector
    std::vector<std::array<int,3>> output_shapes_;// Conv,Poolingは,{height,width,depth}の形だが,Denseの場合は{output_size,-1,-1}としている。
  
    float learning_rate_;
    float weight_decay_;
    float momentum_;
  
    convnet::Losses loss_type_;                   // Loss関数の種類
  
  public:
    Network(const std::array<int,3>& input_shape,convnet::Losses loss_type=convnet::Losses::CategoricalCrossEntropy,
	    float learning_rate=1e-3,float weight_decay=0.0,float momentum=0.0)
      :loss_type_(loss_type),learning_rate_(learning_rate),weight_decay_(weight_decay),momentum_(momentum){
      output_shapes_.push_back(input_shape);
    };
  
    void addConv(const std::array<int,3>& kernel_shape,const std::array<int,2>& stride_shape,const std::array<int,2>& pad_shape={0,0},
		 convnet::Activations activation_type=convnet::Activations::LeakyRelu){
    
      int output_height,output_width,output_depth;
      std::array<int,3> input_shape;
      std::array<int,6> filter_shape;
    
      input_shape = output_shapes_.back(); // 一つ前のレイヤーをシェイプを取り出す。

      // 一つ前のレイヤーの情報をもとにアウトプットのシェイプを計算
      output_height = (input_shape[0] - kernel_shape[0] + 2*pad_shape[0])/stride_shape[0] + 1;
      output_width  = (input_shape[1] - kernel_shape[1] + 2*pad_shape[1])/stride_shape[1] + 1;
      output_depth  = kernel_shape[2];
    
      // filter_shape = {ker_h,ker_w,input_channel,output_channel,stride_h,stride_w} であることに注意。
      filter_shape = {kernel_shape[0],kernel_shape[1],input_shape[2],kernel_shape[2],stride_shape[0],stride_shape[1]};
      convs_.push_back(convnet::Conv(input_shape,filter_shape,pad_shape,activation_type,learning_rate_,weight_decay_,momentum_));    
      layer_types_.push_back(LayerType::Conv);
      output_shapes_.push_back({output_height,output_width,output_depth});
    }

    void addPooling(const std::array<int,2>& pooling_shape,const std::array<int,2>& stride_shape){
      int output_height,output_width,output_depth;
      std::array<int,3> input_shape;
      std::array<int,4> pooling_stride_shape;
      
      input_shape = output_shapes_.back(); // 一つ前のレイヤーをシェイプを取り出す。

      // 一つ前のレイヤーの情報をもとにアウトプットのシェイプを計算
      output_height = (input_shape[0] - pooling_shape[0])/stride_shape[0] + 1;
      output_width  = (input_shape[1] - pooling_shape[1])/stride_shape[1] + 1;
      output_depth  = input_shape[2];

      pooling_stride_shape = {pooling_shape[0],pooling_shape[1],stride_shape[0],stride_shape[1]};
      poolings_.push_back(convnet::Pooling(input_shape,pooling_stride_shape));
      layer_types_.push_back(LayerType::Pooling);
      output_shapes_.push_back({output_height,output_width,output_depth});
    }

    void addDense(int output_size,convnet::Activations activation_type=convnet::Activations::Linear,float dropout_prob=1.0){
      std::array<int,3> input_shape;
      int input_size;
    
      input_shape = output_shapes_.back();// 一つ前のレイヤーをシェイプを取り出す。
      if(input_shape[1] == -1 and input_shape[2] == -1){ // ひとつ前のレイヤーがDenseだった場合
	input_size = input_shape[0];//一つ前のレイヤーがDenseの場合,input_shapeは,{input_size,-1,-1}の形をしていることに注意。
      }else{                                            // ひとつ前のレイヤーがConvまたはPoolingだった場合
	input_size = input_shape[0]*input_shape[1]*input_shape[2]; // vectorizeしているため,このinput_sizeとなる。
      }
    
      denses_.push_back(convnet::Dense(input_size,output_size,activation_type,learning_rate_,weight_decay_,momentum_,dropout_prob));
      layer_types_.push_back(LayerType::Dense);
      output_shapes_.push_back({output_size,-1,-1});
    }

    // Netは,後にBackpropすると想定"していない"動作を行う(つまり評価用)
    Eigen::MatrixXf Net(const std::vector<Eigen::MatrixXf>& input){
      // inputのサイズが適しているかチェック。
      assert(input[0].rows() == output_shapes_[0][0] and input[0].cols() == output_shapes_[0][1] and input.size() == output_shapes_[0][2]);
      assert(layer_types_.back() == LayerType::Dense); // レイヤーの最後は,Denseでなくてはいけない。

      bool is_dense=false; // Denseレイヤーの"領域"に入ったことを記録するための変数
      int conv_count=0;
      int pooling_count=0;
      int dense_count=0;
      std::vector<Eigen::MatrixXf> output;

      output = input;
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
	  assert("[!]そのようなLayerTypeは存在しません\n");
	  break;
	}
      }
    
      Eigen::MatrixXf output_dense = convnet::Im2Dense(output); // ランク3テンソルであるoutputをvectorizeする。
      for(int i=0;i<denses_.size();++i){
	output_dense = denses_[i].Net(output_dense);
      }
    
      return output_dense;    
    }

    // Forwardは,後にBackpropすると想定した動作を行う(つまり学習用)
    Eigen::MatrixXf Forward(const std::vector<Eigen::MatrixXf>& input){
      // inputのサイズが適しているかチェック。
      assert(input[0].rows() == output_shapes_[0][0] and input[0].cols() == output_shapes_[0][1] and input.size() == output_shapes_[0][2]);
      assert(layer_types_.back() == LayerType::Dense); // レイヤーの最後は,Denseでなくてはいけない。

      bool is_dense=false; // Denseレイヤーの"領域"に入ったことを記録するための変数
      int conv_count=0;
      int pooling_count=0;
      int dense_count=0;
      std::vector<Eigen::MatrixXf> output;

      output = input;
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
	  assert("[!]そのようなLayerTypeは存在しません\n");
	  break;
	}
      }
    
      Eigen::MatrixXf output_dense = convnet::Im2Dense(output); // ランク3テンソルであるoutputをvectorizeする。
      for(int i=0;i<denses_.size();++i){
	output_dense = denses_[i].Forward(output_dense);
      }
    
      return output_dense;    
    }

    void Backward(const Eigen::VectorXf& output,const Eigen::VectorXf& label){    
      assert(output.size() == label.size()); // もちろん,outputとlabelの次元が一致しなくてはいけない。
      Eigen::MatrixXf error = GradientOfLoss(output,label); // Loss関数のエラーを取得。
    
      for(int i=denses_.size()-1;i>=0;--i){  // まずDenseレイヤーの"領域"のみをBackwardする。(Conv,Poolingの"領域"ではエラーがテンソルである必要があるため)
	error = denses_[i].Backward(error);
      }
    
      int conv_pooling_idx = output_shapes_.size() - denses_.size() - 1; // Denseレイヤー"領域"の一つ前のレイヤー(つまり,Conv or Pooling)のインデックスを計算。
      std::vector<Eigen::MatrixXf> errors = convnet::Dense2Im(error,output_shapes_[conv_pooling_idx]);// Denseレイヤーのエラーをランク3テンソルに変換。
    
      int conv_idx=convs_.size()-1;
      int pooling_idx=poolings_.size()-1;
      for(int l=conv_pooling_idx;l >=0;--l){ // Conv,Poolingレイヤー"領域"をBackwardしていく。
	switch(layer_types_[l]){
	case LayerType::Conv:
	  errors = convs_[conv_idx--].Backward(errors);
	  break;
	case LayerType::Pooling:
	  errors = poolings_[pooling_idx--].Backward(errors);
	  break;	
	defaut:
	  assert("[!]そのようなLayerTypeは存在しないか,あるいはLayerの順序が正しくありません\n");
	  break;
	}
      }
    
    }

    float Loss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const{
      return GetLoss(output,label,loss_type_); // loss_type_の種類に応じた,Loss関数の値を返す。
    }
  
    Eigen::MatrixXf GradientOfLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label) const{
      return GetGradientOfLoss(output,label,loss_type_); // loss_type_の種類に応じた,Loss関数のGradientを返す。
    }

    void setLearningRate(float learning_rate){    
      learning_rate_ = learning_rate;
    }
  
    void setWeightDecay(float weight_decay){    
      weight_decay_ = weight_decay;
    }

    void setMomentum(float momentum){    
      momentum_ = momentum;
    }
  
  };//class Network

};//namespace convnet

#endif// NETWORK_HPP
