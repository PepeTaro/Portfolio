#ifndef LOSSES_H
#define LOSSES_H

#include <iostream>
#include <cmath>
#include <Eigen/Dense>

namespace convnet{
  
  enum class Losses{CategoricalCrossEntropy,Mse};

  float LogSumExp(const Eigen::VectorXf& logits);
  Eigen::VectorXf Softmax(const Eigen::VectorXf& logits);

  float Mse(const Eigen::VectorXf& input,const Eigen::VectorXf& labels);
  float CategoricalCrossEntropy(const Eigen::VectorXf& logits,const Eigen::VectorXf& labels);
  float GetLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label,Losses loss_type);
  
  Eigen::VectorXf GradientOfMse(const Eigen::VectorXf& input,const Eigen::VectorXf& labels);
  Eigen::VectorXf GradientOfCategoricalCrossEntropy(const Eigen::VectorXf& logits,const Eigen::VectorXf& labels);
  Eigen::VectorXf GetGradientOfLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label,Losses loss_type);
  
};//namespace convnet

#endif// LOSSES_H
