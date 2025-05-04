#ifndef IM2COL_H
#define IM2COL_H

#include <iostream>
#include <vector>
#include <array>
#include <Eigen/Dense>

#include "utils.h"

namespace convnet{
  
  Eigen::MatrixXf Im2Col(const std::vector<Eigen::MatrixXf>& input,const std::array<int,5>& kernel_shape);
  std::vector<Eigen::MatrixXf> Col2Im(const Eigen::MatrixXf& matrix,const std::array<int,3>& output_shape);  
  Eigen::MatrixXf Ker2Col(const std::vector<std::vector<Eigen::MatrixXf>>& kernel); 
  std::vector<Eigen::MatrixXf> Col2ImBack(const Eigen::MatrixXf& error,const std::array<int,3>& input_shape,const std::array<int,3>& output_shape,
					  const std::array<int,5>& kernel_shape);
  Eigen::MatrixXf Im2Dense(const std::vector<Eigen::MatrixXf>& input);
  std::vector<Eigen::MatrixXf> Dense2Im(const Eigen::MatrixXf& input,const std::array<int,3>& shape);
  
};//namespace convnet

#endif// IM2COL_H
