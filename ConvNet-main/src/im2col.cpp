#include "im2col.h"

namespace convnet{
  
  /**
   *  ランク3テンソルinputに対して,kernel_shapeに対応するIm2Col変換を行う。
   *  @param ランク３テンソルinput,kernelの情報を格納した配列
   *  @return Im2Col変換をした結果となる行列
   *
   * [注意]
   *  inputのvectorインデックスがチャンネルを表す,つまりinput[0],input[1],input[2]がそれぞれ,１番目,2番目,３番目のチェンネルに対応する
   *  kernel_shape={kernel_height,kernel_widt,kernel_depth,stride_height,stride_width}
   **/
  Eigen::MatrixXf Im2Col(const std::vector<Eigen::MatrixXf>& input,
			 const std::array<int,5>& kernel_shape){  

    int input_height,input_width,input_channel;  
    int output_height,output_width,output_depth;
    int num_patch_row,num_patch_col; // この変数は,それぞれ縦,横のパッチの数を保存。
    int patch_size; // kernelの高さと横幅を掛けた数を保存。
    Eigen::MatrixXf output,patch;
  
    auto [ker_height,ker_width,ker_depth,ker_stride_height,ker_stride_width] = kernel_shape;
  
    // input内のすべての行列が同じサイズと仮定している。(故に,一般性を欠くことなく,input[0]から行列の形を取り出している)
    input_height = input[0].rows();
    input_width = input[0].cols();
    input_channel = input.size();

    num_patch_row = (input_height - ker_height)/ker_stride_height + 1; // 縦方向のパッチ数
    num_patch_col = (input_width - ker_width)/ker_stride_width + 1;    // 横方向のパッチ数
    output_height = ker_height*ker_width*ker_depth;
    output_width  = num_patch_row*num_patch_col;
    patch_size    = ker_height*ker_width;

    output = Eigen::MatrixXf(output_height,output_width);
    output.setZero();

    // Assertチェック
    assert(input_height >= ker_height and input_width >= ker_width); // kernelのサイズはinputのサイズより小さいか。
    assert(ker_stride_height >= 1 and ker_stride_width >= 1); // Strideは縦,横,共に1以上でなければならない。
    assert(input_channel == ker_depth);                      // inputのチャンネル数とkernelのチャンネル数(depth)は一致しなくてはいけない。
    assert(isDivisible((input_height - ker_height),ker_stride_height)
	   and isDivisible((input_width - ker_width),ker_stride_width));//パッチの数は整数である必要がある。

    // 以下の２つの整数は,各パッチの左上の位置を保存する。
    int patch_start_row,patch_start_col;
    for(int i=0;i<num_patch_row;++i){
      for(int j=0;j<num_patch_col;++j){
	for(int c=0;c<input_channel;++c){	
	  //パッチをstride分ずらす。
	  patch_start_row = i*ker_stride_height; 
	  patch_start_col = j*ker_stride_width;
	
	  patch = input[c].block(patch_start_row,patch_start_col,ker_height,ker_width);//パッチを取り出す。
	  Eigen::Map<Eigen::VectorXf> vectorized_patch(patch.data(),patch.size());     // パッチを列ベクトルにreshape.
	  output.block(c*patch_size,i + j*num_patch_col,patch_size,1) = vectorized_patch;//そのベクトルを,output行列内の対応する位置に配置。
	}
      }
    }

    return output;
  }

  /**
   *  引数matrixの各行を,(output_shape[0],output_shape[1])行列に変換して,その変換した行列をvectorに,output_shape[2]個"積む"
   *  @param 変換したい行列,outputに期待する形を格納した配列。(もちろん,matrixとの整合性が必要)
   *  @return ランク3テンソル
   *
   * [注意]
   *  関数名からして,Im2Colの逆変換と勘違いされるかもしれないが,実際は逆変換"ではない"ことに注意。
   *  TODO:適切な名前に変更
   **/
  std::vector<Eigen::MatrixXf> Col2Im(const Eigen::MatrixXf& matrix,const std::array<int,3>& output_shape){
    std::vector<Eigen::MatrixXf> outputs;
    Eigen::MatrixXf output,patch;

    // Assertチェック
    assert(output_shape[2] == matrix.rows()); // 引数matrixの各行が,戻り値のチャンネルに対応するため,必要。
    assert(output_shape[0]*output_shape[1] == matrix.cols()); // 各行をoutput_shapeの形をしたパッチにreshapeするため,必要。
	 
    for(int d=0;d<output_shape[2];++d){
      patch = matrix.block(d,0,1,matrix.cols()); // 引数matrixの各行を一つずつ取り出す
      output = Eigen::Map<Eigen::MatrixXf>(patch.data(),output_shape[0],output_shape[1]);//取り出した行ベクトルをパッチにreshape
      outputs.push_back(output);// output_shape[2]個分"積む"
    }
    return outputs;
  }

  /** 引数kernelをGEMM(GEneralized Matrix Multiplication)の計算に適した形の行列に変換
   *  @param ランク4テンソル(kernel[f][c]は,f番目のフィルターのc番目のチャンネルに対応する行列)
   *  @return 変換後の行列
   **/
  Eigen::MatrixXf Ker2Col(const std::vector<std::vector<Eigen::MatrixXf>>& kernel){  
    int ker_height,ker_width,ker_depth,num_of_filters,ker_size;  
    int output_height,output_width,output_depth;  
    int patch_size; // kernelの高さと横幅を掛けた数を保存。
    Eigen::MatrixXf output;

    // kernel内のすべての行列が同じサイズと仮定している。
    ker_height = kernel[0][0].rows();
    ker_width  = kernel[0][0].cols();
    ker_depth  = kernel[0].size();
    ker_size   = ker_height*ker_width;  
    num_of_filters = kernel.size();
  
    output_height = ker_depth*ker_size;
    output_width  = num_of_filters;    
    output = Eigen::MatrixXf(output_height,output_width);
    output.setZero();

    for(int f=0;f<num_of_filters;++f){
      // f番目のフィルターを,行列に変換して,output行列内の"正しい"位置に配置
      output.block(0,f,output_height,1) = Im2Col(kernel[f],{ker_height,ker_width,ker_depth,1,1});
    }
  
    return output;
  }

  /** ConvNetにおけるBackprop時のエラーを伝播させるのに必要。戻り値は,Convolutionする前のテンソルに対応したエラー。
   *  @param 変換する行列,Convolutionする前のテンソルの形を保存した配列,Convolutionした後のテンソルの形を保存した配列,kernelの情報を保存した配列
   *  @return ランク3テンソル
   **/
  std::vector<Eigen::MatrixXf> Col2ImBack(const Eigen::MatrixXf& error,const std::array<int,3>& input_shape,const std::array<int,3>& output_shape,const std::array<int,5>& kernel_shape){
    Eigen::MatrixXf vec;
    Eigen::MatrixXf patch,output,block_per_depth;
    std::vector<Eigen::MatrixXf> outputs;
    int patch_row_idx,patch_col_idx,patch_size;

    auto [ker_height,ker_width,ker_depth,ker_stride_height,ker_stride_width] = kernel_shape;

    patch_size = ker_height*ker_width;
    output = Eigen::MatrixXf(input_shape[0],input_shape[1]);

    // Assertチェック
    assert(input_shape[2] == ker_depth); // inputのチャンネル数とkernelのチャンネル数が等しいか
    assert(isDivisible(error.cols(),patch_size) and
	   error.cols()/patch_size == input_shape[2]);// 引数errorの各行をpatchに変換するため,割り切れるか否かチェック。
    // errroの列は,patch_size*inputのチャンネル数であると期待している。
  
    for(int c=0;c<input_shape[2];++c){ // inputのチャンネル
      block_per_depth = error.block(0,c*patch_size,error.rows(),patch_size);//inputの各チャンネルに対応するブロックを取り出す。
      output.setZero();
    
      for(int i=0;i<error.rows();++i){
	vec = block_per_depth.block(i,0,1,block_per_depth.cols()); // 各行を取り出す。
	patch = Eigen::Map<Eigen::MatrixXf>(vec.data(),ker_height,ker_width); // その行を,kernelと同じ形をしたパッチに変換。            
	patch_row_idx = (i%output_shape[1])*ker_stride_height;                // パッチを"正しい"位置に配置するためのオフセットを計算。
	patch_col_idx = (i/output_shape[1])*ker_stride_width;
	output.block(patch_row_idx,patch_col_idx,ker_height,ker_width) += patch; // パッチを配置
      }

      outputs.push_back(output);  //　チャンネル数分積む
    }

    return outputs;
  }

  /**
   *  ランク3テンソルをベクトルにFlattenする。(Denseレイヤーのインプットはベクトルしか受け付けないため,Denseレイヤー前にこの関数を使用する)
   *  @param ランク3テンソル
   *  @return ベクトル
   **/
  Eigen::MatrixXf Im2Dense(const std::vector<Eigen::MatrixXf>& input){
    Eigen::MatrixXf im2mat = Im2Col(input,{input[0].rows(),input[0].cols(),(int)input.size(),1,1});           // ランク3テンソルを行列に変換
    Eigen::MatrixXf mat2vec = Eigen::Map<Eigen::MatrixXf>(im2mat.data(),(input[0].rows()*input[1].cols()*input.size()),1); // ベクトルに変換
  
    return mat2vec;
  }

  /**
   *  ベクトルである引数inputを引数shapeの情報に基づいて,ランク3テンソルへ変換
   *  (Backprop時に,Denseレイヤーから,Conv或いはPoolingレイヤーにエラーを"流す"際に使用)
   *  @param ベクトル,アウトプットとして期待するシェイプの情報を格納した配列
   *  @return ランク3テンソル
   **/
  std::vector<Eigen::MatrixXf> Dense2Im(const Eigen::MatrixXf& input,const std::array<int,3>& shape){
    std::vector<Eigen::MatrixXf> outputs;  
    Eigen::MatrixXf patch,output;
    int patch_size = shape[0]*shape[1];//ランク3テンソルの縦幅と横幅をかけ合わせた数値
  
    // ベクトルinputの次元が,shapeと整合性があるか否かチェック
    assert(isDivisible(input.rows(),shape[2]) and isDivisible(input.rows(),patch_size) and (input.rows()/patch_size) == shape[2]);
  
    for(int i=0;i<shape[2];++i){ // チャンネル数ループ
      patch = input.block(i*patch_size,0,patch_size,1); // ベクトルinputからpatch_size分とフェッチ
      output = Eigen::Map<Eigen::MatrixXf>(patch.data(),shape[0],shape[1]); // patchを行列に変換
      outputs.push_back(output);
    }
  
    return outputs;
  }

};//namespace convnet
