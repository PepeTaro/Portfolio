#ifndef MNIST_HPP
#define MNIST_HPP

#include <iostream>
#include <cassert>
#include <numeric>

// https://github.com/wichtounet/mnist/
#include "../thirdparty/mnist/include/mnist/mnist_reader.hpp"

void PreprocessTrainingData(const auto& dataset,std::vector<Eigen::MatrixXf>& data,std::vector<Eigen::MatrixXf>& labels,int data_size=60000,int feature_size=784){
  Eigen::MatrixXf image(28,28);    
  Eigen::MatrixXf label(10,1);
  int h,w;

  for(int i=0;i<data_size;++i){
    image.setZero();
    label.setZero();
    for(int j=0;j<feature_size;++j){
      h = j/28;
      w = j%28;
      image(h,w) = (float)dataset.training_images[i][j]/255.0;
    }
    data.push_back(image);
    label(dataset.training_labels[i]) = 1;
    labels.push_back(label);
  }
}

void PreprocessTestData(const auto& dataset,std::vector<Eigen::MatrixXf>& data,std::vector<Eigen::MatrixXf>& labels,int data_size=10000,int feature_size=784){
  Eigen::MatrixXf image(28,28);    
  Eigen::MatrixXf label(10,1);
  int h,w;

  for(int i=0;i<data_size;++i){
    image.setZero();
    label.setZero();
    for(int j=0;j<feature_size;++j){
      h = j/28;
      w = j%28;
      image(h,w) = (float)dataset.test_images[i][j]/255.0;
    }
    data.push_back(image);
    label(dataset.test_labels[i]) = 1;
    labels.push_back(label);
  }  
}

#endif// MNIST_HPP
