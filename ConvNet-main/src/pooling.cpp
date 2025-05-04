#include "pooling.h"

namespace convnet{  

  Pooling::Pooling(const std::array<int,3>& input_shape,const std::array<int,4>& pooling_shape):input_shape_(input_shape),pooling_shape_(pooling_shape){

    // Poolingのサイズとinputのサイズが,"整合性"があるか否かチェック。
    assert(isDivisible((input_shape_[0]-pooling_shape_[0]),pooling_shape_[2]) and isDivisible((input_shape_[1]-pooling_shape_[1]),pooling_shape_[3]));
      
    output_shape_ = {(input_shape_[0]-pooling_shape_[0])/pooling_shape_[2] + 1,
		     (input_shape_[1]-pooling_shape_[1])/pooling_shape_[3]+ 1,
		     input_shape_[2]};
    
  }

  std::vector<Eigen::MatrixXf> Pooling::Net(const std::vector<Eigen::MatrixXf>& input) const{
    int output_row,output_col;       // アウトプットのインデックスに対応するインプットのインデックスを保存
    int end_row,end_col;            // 限界のインデックスを保存(つまり,Pooling windowを"スライド"しているときに,inputからはみ出さないように) 
    Eigen::MatrixXf output,patch;
    std::vector<Eigen::MatrixXf> outputs;
    
    auto [pool_height,pool_width,pool_stride_height,pool_stride_width] = pooling_shape_;
    auto [input_height,input_width,input_depth] = input_shape_;
    auto [output_height,output_width,output_depth] = output_shape_;
    
    output = Eigen::MatrixXf(output_height,output_width);

    end_row = (input_height - pool_height) + 1;
    end_col = (input_width - pool_width) + 1;
    
    for(int d=0;d<output_depth;++d){
      output.setZero();  
      for(int i=0;i<end_row;i+=pool_stride_height){
	for(int j=0;j<end_col;j+=pool_stride_width){
	  patch = input[d].block(i,j,pool_height,pool_width); // pooling windowを取り出す

	  // インプットインデックスi,jに対応するアウトプットの行,列を計算
	  output_row = i/pool_stride_height;
	  output_col = j/pool_stride_width;

	  // Pooling windowから最大値を取り出して,アウトプットに保存
	  output(output_row,output_col) = patch.maxCoeff();	  
	}
      }            
      outputs.push_back(output);
    }  
    return outputs;
  }
  
  std::vector<Eigen::MatrixXf> Pooling::Forward(const std::vector<Eigen::MatrixXf>& input){    
    post_pooling_ = MaxPooling(input);    
    return post_pooling_;
  }

  std::vector<Eigen::MatrixXf> Pooling::Backward(const std::vector<Eigen::MatrixXf>& error){    
    errors_ = GetPoolingError(error,input_shape_,pooling_record_row_,pooling_record_col_);
    return errors_;
  }

  
  std::vector<Eigen::MatrixXf> Pooling::MaxPooling(const std::vector<Eigen::MatrixXf>& input){          
    int max_index_row,max_index_col; // Pooling window内において,最大値をもつ値のインデックスを保存
    int output_row,output_col;       // アウトプットのインデックスに対応するインプットのインデックスを保存
    int end_row,end_col;            // 限界のインデックスを保存(つまり,Pooling windowを"スライド"しているときに,inputからはみ出さないように) 
    Eigen::MatrixXf output,patch;
    Eigen::MatrixXi record_row,record_col;
    std::vector<Eigen::MatrixXf> outputs;
    
    auto [pool_height,pool_width,pool_stride_height,pool_stride_width] = pooling_shape_;
    auto [input_height,input_width,input_depth] = input_shape_;
    auto [output_height,output_width,output_depth] = output_shape_;

    // 新たなレコード用にメモリを解放
    std::vector<Eigen::MatrixXi>().swap(pooling_record_row_);
    std::vector<Eigen::MatrixXi>().swap(pooling_record_col_);
    
    output = Eigen::MatrixXf(output_height,output_width);
    record_row = Eigen::MatrixXi(output_height,output_width);
    record_col = Eigen::MatrixXi(output_height,output_width);

    // for loopがiterateする行,列を計算.(縦にend_row,横にend_col)
    end_row = (input_height - pool_height) + 1;
    end_col = (input_width - pool_width) + 1;
    
    for(int d=0;d<output_depth;++d){
      output.setZero(),record_row.setZero(),record_col.setZero();
  
      for(int i=0;i<end_row;i+=pool_stride_height){
	for(int j=0;j<end_col;j+=pool_stride_width){
	  patch = input[d].block(i,j,pool_height,pool_width); // pooling windowを取り出す

	  // インプットインデックスi,jに対応するアウトプットの行,列を計算
	  output_row = i/pool_stride_height;
	  output_col = j/pool_stride_width;

	  // Pooling windowから最大値を取り出して,アウトプットに保存
	  output(output_row,output_col) = patch.maxCoeff(&max_index_row,&max_index_col);

	  // アウトプットの値がどこから来たのかを保存(Backprop用)
	  record_row(output_row,output_col) = i + max_index_row;
	  record_col(output_row,output_col) = j + max_index_col;	
	}
      }
      
      pooling_record_row_.push_back(record_row);
      pooling_record_col_.push_back(record_col);
      outputs.push_back(output);
    }  
    return outputs;
  }

  std::vector<Eigen::MatrixXf> Pooling::GetPoolingError(const std::vector<Eigen::MatrixXf>& backprop_error,
					       const std::array<int,3>& before_pooling_shape,
					       const std::vector<Eigen::MatrixXi>& records_row,const std::vector<Eigen::MatrixXi>& records_col) const{
  
    int max_index_row,max_index_col;
    std::vector<Eigen::MatrixXf> errors;
    Eigen::MatrixXf error(before_pooling_shape[0],before_pooling_shape[1]);

    for(int d=0;d<before_pooling_shape[2];++d){
      error.setZero();  
      for(int i=0;i<backprop_error[d].rows();++i){
	for(int j=0;j<backprop_error[d].cols();++j){
	  // アウトプットの値がinputのどこの行,列から来たのかを示すインデックスを取り出す
	  max_index_row = records_row[d](i,j);
	  max_index_col = records_col[d](i,j);
      
	  error(max_index_row,max_index_col) += backprop_error[d](i,j); // エラーを"蓄積"
	}
      }
      errors.push_back(error);
    }
  
    return errors;
  }

  std::array<int,3> Pooling::getInputShape() const{
    return input_shape_;
  }
  
  std::array<int,3> Pooling::getOutputShape() const{
    return output_shape_;
  }

  std::array<int,4> Pooling::getPoolingShape() const{
    return pooling_shape_;
  }

  
};//namespace convnet
