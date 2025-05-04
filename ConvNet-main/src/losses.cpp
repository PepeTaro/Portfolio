#include "losses.h"

namespace convnet{

  float LogSumExp(const Eigen::VectorXf& logits){
    Eigen::VectorXf logits_minus_max;
    Eigen::VectorXf exp_logits_minus_max;
    Eigen::VectorXf max_vector;
    float sum_of_exps;
    float return_value;
    float max;
  
    max_vector = Eigen::VectorXf(logits.size());
    max = logits.maxCoeff();
    max_vector.setConstant(max);

    logits_minus_max = logits - max_vector;
    exp_logits_minus_max = logits_minus_max.array().exp();
  
    sum_of_exps  = exp_logits_minus_max.sum();
    return_value = max + log(sum_of_exps);

    return return_value;
  }

  Eigen::VectorXf Softmax(const Eigen::VectorXf& logits){
    Eigen::VectorXf softmax;
    Eigen::VectorXf lse_vector;
    float lse;

    lse = LogSumExp(logits);
    lse_vector = Eigen::VectorXf(logits.size());  
    lse_vector.setConstant(lse);

    softmax = (logits - lse_vector).array().exp();
    return softmax;
  }

  float CategoricalCrossEntropy(const Eigen::VectorXf& logits,const Eigen::VectorXf& labels){
    static float epsilon = 1e-12;
    Eigen::VectorXf softmax;
    Eigen::VectorXf log_of_softmax;
    Eigen::VectorXf labels_times_log_of_softmax;
    Eigen::VectorXf epsilon_vec = Eigen::VectorXf::Constant(logits.size(),epsilon);
    float loss;
    
    softmax = Softmax(logits);
    log_of_softmax = (epsilon_vec + softmax).array().log(); // logを使用するため,softmaxの確率が0だと困るので非常に小さい数epsilonを加えている
    labels_times_log_of_softmax = (labels.array())*(log_of_softmax.array());// 成分ごとに掛け合わせていることに注意。
    loss = -1.0 * labels_times_log_of_softmax.sum();
  
    return loss;
  }

  float Mse(const Eigen::VectorXf& input,const Eigen::VectorXf& labels){
    return 0.5*(input - labels).array().pow(2).sum();
  }

  Eigen::VectorXf GradientOfMse(const Eigen::VectorXf& input,const Eigen::VectorXf& labels){
    return (input - labels);
  }

  Eigen::VectorXf GradientOfCategoricalCrossEntropy(const Eigen::VectorXf& logits,const Eigen::VectorXf& labels){
    Eigen::VectorXf softmax;
  
    softmax = Softmax(logits);  
    return (softmax - labels);
  }

  float GetLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label,const Losses loss_type){
    switch(loss_type){
    case Losses::CategoricalCrossEntropy:
      return CategoricalCrossEntropy(output,label);
      break;
    case Losses::Mse:
      return Mse(output,label);
      break;
    default:
      std::cerr << "[!] Error(GetLoss): そのようなLoss関数は存在しません。" << std::endl;
      exit(-1);
      break;
    }
  }

  Eigen::VectorXf GetGradientOfLoss(const Eigen::VectorXf& output,const Eigen::VectorXf& label,const Losses loss_type){
    switch(loss_type){
    case Losses::CategoricalCrossEntropy:
      return GradientOfCategoricalCrossEntropy(output,label);
      break;
    case Losses::Mse:
      return GradientOfMse(output,label);
      break;    
    default:
      std::cerr << "[!] Error(GetGradientOfLoss): そのようなLoss関数は存在しません。" << std::endl;
      exit(-1);
      break;
    }
  }

};//namespace convnet
