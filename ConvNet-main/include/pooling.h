#ifndef POOLING_H
#define POOLING_H

#include <cassert>
#include <vector>
#include <array>
#include <Eigen/Dense>

#include "utils.h"

namespace convnet{
  
  class Pooling{
  protected:
    std::array<int,3> input_shape_;
    std::array<int,3> output_shape_;
    std::array<int,4> pooling_shape_;
    std::vector<Eigen::MatrixXi> pooling_record_row_,pooling_record_col_;
    std::vector<Eigen::MatrixXf> post_pooling_,errors_;
  
  public:
    Pooling(const std::array<int,3>& input_shape,const std::array<int,4>& pooling_shape);
    std::vector<Eigen::MatrixXf> Net(const std::vector<Eigen::MatrixXf>& input) const;
    std::vector<Eigen::MatrixXf> Forward(const std::vector<Eigen::MatrixXf>& input);
    std::vector<Eigen::MatrixXf> Backward(const std::vector<Eigen::MatrixXf>& error);
    std::vector<Eigen::MatrixXf> MaxPooling(const std::vector<Eigen::MatrixXf>& input);
    std::vector<Eigen::MatrixXf> GetPoolingError(const std::vector<Eigen::MatrixXf>& backprop_error,
						 const std::array<int,3>& before_pooling_shape,
						 const std::vector<Eigen::MatrixXi>& records_row,
						 const std::vector<Eigen::MatrixXi>& records_col) const;

    std::array<int,3> getInputShape() const;
    std::array<int,3> getOutputShape() const;    
    std::array<int,4> getPoolingShape() const;        
  };//class Pooling

};//namespace convnet

#endif// POOLING_H
