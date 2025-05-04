#ifndef CIFAR10_HPP
#define CIFAR10_HPP

#include <iostream>
#include <cassert>
#include <numeric>

// https://github.com/wichtounet/cifar-10
#include "../thirdparty/cifar-10/include/cifar/cifar10_reader.hpp"

void PreprocessTrainingData(const auto& dataset,
			    std::vector<std::vector<Eigen::MatrixXf>>& data,std::vector<Eigen::MatrixXf>& labels,
			    int data_size=50000,int feature_size=1024){
  
  std::vector<Eigen::MatrixXf> image;
  Eigen::MatrixXf image_channel(32,32);    
  Eigen::MatrixXf label(10,1);
  int h,w;

  for(int i=0;i<data_size;++i){
    image.clear();
    label.setZero();
    for(int c=0;c<3;++c){
      image_channel.setZero();
      for(int j=0;j<feature_size;++j){
	h = j/32;
	w = j%32;
	image_channel(h,w) = (float)dataset.training_images[i][c*feature_size + j]/255.0;
      }
      image.push_back(image_channel);
    }
    
    data.push_back(image);
    label(dataset.training_labels[i]) = 1;
    labels.push_back(label);
  }
}

void PreprocessTestData(const auto& dataset,
			    std::vector<std::vector<Eigen::MatrixXf>>& data,std::vector<Eigen::MatrixXf>& labels,
			    int data_size=10000,int feature_size=1024){
  
  std::vector<Eigen::MatrixXf> image;
  Eigen::MatrixXf image_channel(32,32);    
  Eigen::MatrixXf label(10,1);
  int h,w;

  for(int i=0;i<data_size;++i){
    image.clear();
    label.setZero();
    for(int c=0;c<3;++c){
      image_channel.setZero();
      for(int j=0;j<feature_size;++j){
	h = j/32;
	w = j%32;
	image_channel(h,w) = (float)dataset.test_images[i][c*feature_size + j]/255.0;
      }
      image.push_back(image_channel);
    }
    
    data.push_back(image);
    label(dataset.test_labels[i]) = 1;
    labels.push_back(label);
  }
}

#endif// CIFAR10_HPP
