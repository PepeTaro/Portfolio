#include "save_load_model.h"

namespace convnet{
  void SaveNetwork(const Network& network,std::string file_name){
    json j;
        
    int conv_count = 0;
    int pooling_count = 0;
    int dense_count = 0;
    int layer_index = 0;
    
    std::vector<float> kernel_vec;
    std::vector<float> weight_vec;
    std::vector<float> bias_vec;
    Eigen::MatrixXf kernel_mat;
    Eigen::MatrixXf weight_mat;
    Eigen::VectorXf bias_mat;

    std::vector<LayerType> layer_types = network.getLayerTypes();
    std::vector<std::array<int,3>> output_shapes = network.getOutputShapes();    
    
    j["loss_type"]       = network.getLossType();
    j["layer_types"]     = layer_types;
    j["input_shape"]     = output_shapes[0];

    for(LayerType layer_type : layer_types){
      layer_index++;
      std::string layer_index_str = "layer" + std::to_string(layer_index); 
      switch(layer_type){
	
      case LayerType::Conv:{
	Conv conv = network.getConv(conv_count);
	
	j[layer_index_str.c_str()]["layer_type"] = "Conv";
	j[layer_index_str.c_str()]["kernel_shape"] = conv.getKernelShape();
	j[layer_index_str.c_str()]["pad_shape"] = conv.getPadShape();
	j[layer_index_str.c_str()]["activation"] = conv.getActivationType();
	
        kernel_mat = conv.getKernel();
	kernel_vec = std::vector<float>(kernel_mat.data(),kernel_mat.data() + kernel_mat.rows() * kernel_mat.cols());	
	j[layer_index_str.c_str()]["kernel"] = kernel_vec;
	j[layer_index_str.c_str()]["kernel_row"] = kernel_mat.rows();
	j[layer_index_str.c_str()]["kernel_col"] = kernel_mat.cols();
	
	bias_mat = conv.getBias();
	bias_vec = std::vector<float>(bias_mat.data(),bias_mat.data() + bias_mat.size());	
	j[layer_index_str.c_str()]["bias"] = bias_vec;

	conv_count++;
	break;
	
      }
      case LayerType::Dense:{
	Dense dense = network.getDense(dense_count);
	
	j[layer_index_str.c_str()]["layer_type"] = "Dense";
	j[layer_index_str.c_str()]["output_size"] = dense.getOutputSize();
	j[layer_index_str.c_str()]["activation"] = dense.getActivationType();

	weight_mat = dense.getWeight();
	weight_vec = std::vector<float>(weight_mat.data(),weight_mat.data() + weight_mat.rows() * weight_mat.cols());	
	j[layer_index_str.c_str()]["weight"] = weight_vec;
	j[layer_index_str.c_str()]["weight_row"] = weight_mat.rows();
	j[layer_index_str.c_str()]["weight_col"] = weight_mat.cols();

	bias_mat = dense.getBias();
	bias_vec = std::vector<float>(bias_mat.data(),bias_mat.data() + bias_mat.size());	
	j[layer_index_str.c_str()]["bias"] = bias_vec;
	
	dense_count++;
	break;
	
      }
      case LayerType::Pooling:{
	Pooling pooling = network.getPooling(pooling_count);
	
	j[layer_index_str.c_str()]["layer_type"] = "Pooling";
	j[layer_index_str.c_str()]["pooling_shape"] = pooling.getPoolingShape();
	pooling_count++;
	break;
	
      }
      default:{
	std::cerr << "Error(Network::Save) そのようなLayerTypeは存在しません" << std::endl;
	exit(-1);
      }
	
      }      
    }

    std::ofstream outfile(file_name.c_str());
    if(not outfile){
      std::cerr << "Error(Networ::Save) ファイルを開けませんでした(" << file_name.c_str() << ")" << std::endl;
      exit(-1);
    }

    outfile << std::setw(4) << j << std::endl;
    outfile.flush();
    outfile.close();
    std::cout << "モデルを" << file_name << "に保存しました。" << std::endl; 
  }

  
  
  Network LoadNetwork(std::string file_name){
    std::ifstream infile(file_name.c_str());
    if(not infile){
      std::cerr << "Error(Load) そのようなファイルは存在しません" << std::endl;
      exit(-1);
    }
    std::cout << "モデルを" << file_name << "から読み込みました。" << std::endl;
    
    // jsonファイルからロード
    json j;
    infile >> j;  

    std::array<int,3> input_shape      = j["input_shape"];        
    Losses loss_type                   = j["loss_type"];
    std::vector<LayerType> layer_types = j["layer_types"];

    std::vector<float> kernel_vec;
    std::vector<float> weight_vec;
    std::vector<float> bias_vec;
    Eigen::MatrixXf kernel_mat;
    Eigen::MatrixXf weight_mat;
    Eigen::VectorXf bias_mat;
    
    std::array<int,6> kernel_shape;
    std::array<int,2> pad_shape;
    std::array<int,4> pooling_shape;
    Activations activation_type;

    int layer_index = 0;
    int output_size;
    int weight_row,weight_col;
    int kernel_row,kernel_col;

    Network network(input_shape,loss_type);
    
    for(LayerType layer_type : layer_types){
      layer_index++;
      std::string layer_index_str = "layer" + std::to_string(layer_index);
      
      switch(layer_type){
      case LayerType::Conv:
	
	kernel_shape    = j[layer_index_str.c_str()]["kernel_shape"];
	pad_shape       = j[layer_index_str.c_str()]["pad_shape"];
	activation_type = j[layer_index_str.c_str()]["activation"];
	
	network.addConv({kernel_shape[0],kernel_shape[1],kernel_shape[3]},{kernel_shape[4],kernel_shape[5]},
			{pad_shape[0],pad_shape[1]},activation_type);
	
	kernel_vec = static_cast<std::vector<float>>(j[layer_index_str.c_str()]["kernel"]);
	kernel_row = j[layer_index_str.c_str()]["kernel_row"];
	kernel_col = j[layer_index_str.c_str()]["kernel_col"];
	bias_vec   = static_cast<std::vector<float>>(j[layer_index_str.c_str()]["bias"]);

	kernel_mat = Eigen::Map<Eigen::MatrixXf>(&kernel_vec[0],kernel_row,kernel_col);
	bias_mat = Eigen::Map<Eigen::VectorXf>(&bias_vec[0],bias_vec.size());

	network.setConvKernel(-1,kernel_mat);
	network.setConvBias(-1,bias_mat);      

	break;
	
      case LayerType::Dense:
        output_size    = j[layer_index_str.c_str()]["output_size"];
	activation_type = j[layer_index_str.c_str()]["activation"];
	
	network.addDense(output_size,activation_type);
		
	weight_vec = static_cast<std::vector<float>>(j[layer_index_str.c_str()]["weight"]);
	weight_row = j[layer_index_str.c_str()]["weight_row"];
	weight_col = j[layer_index_str.c_str()]["weight_col"];
	bias_vec   = static_cast<std::vector<float>>(j[layer_index_str.c_str()]["bias"]);

	weight_mat = Eigen::Map<Eigen::MatrixXf>(&weight_vec[0],weight_row,weight_col);
	bias_mat = Eigen::Map<Eigen::VectorXf>(&bias_vec[0],bias_vec.size());

	network.setDenseWeight(-1,weight_mat);
	network.setDenseBias(-1,bias_mat);

	break;
	
      case LayerType::Pooling:
	pooling_shape = j[layer_index_str.c_str()]["pooling_shape"];
	network.addPooling({pooling_shape[0],pooling_shape[1]},{pooling_shape[2],pooling_shape[3]});		
	break;

      default:
	std::cerr << "Error(LoadNetwork) そのようなLayerTypeは存在しません" << std::endl;
	exit(-1);
      }      
    }
    
    return network;
  }

   
};//namespace convnet
