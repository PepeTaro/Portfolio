#include <iostream>
#include <vector>
#include <array>
#include <Eigen/Dense>

#include "dense.h"
#include "pooling.h"
#include "conv.h"
#include "network.h"
#include "save_load_model.h"

#include "cifar10.hpp"

using namespace convnet;

int SplitDataset(const std::vector<std::vector<Eigen::MatrixXf>>& data,const std::vector<Eigen::MatrixXf>& labels,
		  std::vector<std::vector<Eigen::MatrixXf>>& training_data,std::vector<Eigen::MatrixXf>& training_labels,
		 std::vector<std::vector<Eigen::MatrixXf>>& validation_data,std::vector<Eigen::MatrixXf>& validation_labels,
		 float ratio=0.9){

  int data_size = data.size();  
  int training_data_size = floor(data_size*ratio);
  int data_index = 0;
    
  // indicesに0からnまでの整数を昇順に格納(ここで,nはdata_size)
  std::vector<int> indices(data_size);
  std::iota(std::begin(indices),std::end(indices),0);

  // indicesをシャッフル
  auto rng = std::default_random_engine{unsigned(time(NULL))};
  std::shuffle(std::begin(indices),std::end(indices),rng);
  
  for(int n : indices){ // データインデックスを格納しているindices配列から,順に取り出す(シャッフルされているため,ランダム)
    std::vector<Eigen::MatrixXf> rgb_image;
    
    if(data_index < training_data_size){ // training_dataを格納
      for(int c=0;c<3;++c){
	rgb_image.push_back(data[n][c]);
      }
      
      training_data.push_back(rgb_image);
      training_labels.push_back(labels[n]);
      data_index++;      
    }else{                              // validation_dataを格納
      for(int c=0;c<3;++c){
	rgb_image.push_back(data[n][c]);
      }
      
      validation_data.push_back(rgb_image);
      validation_labels.push_back(labels[n]);
      data_index++;
    }
    
  }

  std::cout << "データセットを以下の割合で分割しました:" << std::endl;
  std::cout << "(Training dataset size:" << training_data.size() << ",Validation dataset size:" << validation_data.size() << ")" << std::endl;

  return training_data.size();
}


// networkの精度,lossを測定
std::array<float,2> MeasureAccuracyAndLoss(const Network& network,
					   const std::vector<std::vector<Eigen::MatrixXf>>& data,const std::vector<Eigen::MatrixXf>& labels){
  int num_correct = 0;
  float accum_loss = 0.0;
  int max_idx;
  int data_size = data.size();
  Eigen::MatrixXf output;
  
  for(int n=0;n<data_size;++n){    
    output = network.Net({data[n][0],data[n][1],data[n][2]});  // インプットであるdata[n]をNetworkに"流し",outputを取得
    accum_loss += network.Loss(output,labels[n]); // lossを取得
    
    Softmax(output).maxCoeff(&max_idx); // 最大の確率を持つインデックスを取り出す 
    if(labels[n](max_idx) == 1) num_correct++; //正答数をカウント    
  }

  return {static_cast<float>(100.0*num_correct/data_size),(accum_loss/data_size)};
}

// training_dataを使用して,networkをトレーニング
void TrainNetwork(Network& network,
		  const std::vector<std::vector<Eigen::MatrixXf>>& training_data,const std::vector<Eigen::MatrixXf>& training_labels,
		  const std::vector<std::vector<Eigen::MatrixXf>>& validation_data,const std::vector<Eigen::MatrixXf>& validation_labels,
		  int epoch=1){

  Eigen::MatrixXf output;
  int progress;
  int training_data_size = training_data.size();
  int validation_data_size = validation_data.size();
  
  for(int e=0;e<epoch;++e){
    std::cout << "\n[Epoch:" << e+1 << "]" << std::endl;
    
    progress = 0;    
    for(int n=0;n<training_data_size;++n){      
      // Forward
      output = network.Forward({training_data[n][0],training_data[n][1],training_data[n][2]});      
      // Backward
      network.Backward(output,training_labels[n]);

      // Progressの表示
      if((progress%100) == 0) printf("\rProgress:%f",100.0*progress/training_data_size),fflush(stdout);      
      progress++;
      
    }
    printf("\rProgress:%f",100.0),fflush(stdout);
    /*
    {// Training loss
      auto [acc,loss] = MeasureAccuracyAndLoss(network,training_data,training_labels);
      std::cout << "\nTraining Accuracy:" << acc << "%" << std::endl;
      std::cout << "Training loss:" << loss << std::endl;
    };
    */
    {// Validation loss
      auto [acc,loss] = MeasureAccuracyAndLoss(network,validation_data,validation_labels);
      std::cout << "\nValidation Accuracy:" << acc << "%" << std::endl;
      std::cout << "Validation loss:" << loss << std::endl;
    };
    
  }
}

// Testデータセットの精度,lossを測定
void MeasureTestAccuracy(const Network& network,const std::vector<std::vector<Eigen::MatrixXf>>& test_data,const std::vector<Eigen::MatrixXf>& test_labels){  
  auto [acc,loss] = MeasureAccuracyAndLoss(network,test_data,test_labels);
  std::cout << "\nTest Accuracy:" << acc << "%" << std::endl;
  std::cout << "Test loss:" << loss << std::endl;
}

int TestNetwork(){

  int training_data_size;
  int data_size = 50000;
  int test_data_size = 10000;
  // 外部モジュールを使用して、CIFARデータセットを読み込み。
  auto dataset = cifar::read_dataset<std::vector, std::vector, uint8_t, uint8_t>();

  std::vector<std::vector<Eigen::MatrixXf>> data;
  std::vector<Eigen::MatrixXf> labels;
  std::vector<std::vector<Eigen::MatrixXf>> training_data;
  std::vector<Eigen::MatrixXf> training_labels;
  std::vector<std::vector<Eigen::MatrixXf>> validation_data;
  std::vector<Eigen::MatrixXf> validation_labels;
  std::vector<std::vector<Eigen::MatrixXf>> test_data;
  std::vector<Eigen::MatrixXf> test_labels;
  
  // データのプリプロセス。
  PreprocessTrainingData(dataset,data,labels,data_size);
  PreprocessTestData(dataset,test_data,test_labels,test_data_size);
  training_data_size = SplitDataset(data,labels,training_data,training_labels,validation_data,validation_labels,0.9);

  //Network network({32,32,3},new optimizers::Sgd(training_data_size,32,1e-4,1e-2,0.99),Losses::CategoricalCrossEntropy);
  //Network network({32,32,3},new optimizers::Adam(training_data_size,32,1e-4),Losses::CategoricalCrossEntropy)(Test Accuracy:52.66%)
  Network network({32,32,3},new optimizers::AdamW(training_data_size,32,1e-3,1e-1),Losses::CategoricalCrossEntropy);
  
  
  network.addConv({5,5,12},{1,1},{0,0},Activations::LeakyRelu);  
  network.addPooling({2,2},{2,2});
  network.addConv({5,5,32},{1,1},{0,0},Activations::LeakyRelu);
  network.addPooling({2,2},{2,2});  
  network.addDense(256,Activations::LeakyRelu);
  network.addDense(128,Activations::LeakyRelu);
  network.addDense(10);
  
  /*
  network.addConv({5,5,6},{1,1},{0,0},Activations::LeakyRelu);  
  network.addPooling({2,2},{2,2});
  network.addConv({5,5,16},{1,1},{0,0},Activations::LeakyRelu);
  network.addPooling({2,2},{2,2});
  network.addDense(120,Activations::LeakyRelu);
  network.addDense(84,Activations::LeakyRelu);
  network.addDense(10);
  */
  
  TrainNetwork(network,training_data,training_labels,validation_data,validation_labels,40);// Train
  MeasureTestAccuracy(network,test_data,test_labels);   // Test

  //SaveNetwork(network,"../models/cifar10.json");    
}

int main(){    
  TestNetwork();
}
 
   
