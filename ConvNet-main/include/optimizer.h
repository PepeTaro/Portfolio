#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <iostream>
#include <Eigen/Dense>

namespace convnet::optimizers{
  
  enum class optimizer_type{Sgd,Adam,AdamW};
  
  class Optimizer{
  protected:
    optimizer_type type_; // どのOptimizerを使用しているかを示す変数
    int batch_count_;     // 各Epochにおいて,学習に使用したデータ数をカウント
    int minibatch_size_; 
    int dataset_size_;   // 学習に使用するデータセット全体のサイズ
    int time_step_;      // Backprop用のタイムステップ
    bool minibatch_update_; // この変数がtrueであるときのみ,パラメーターを更新
    
  public:
    Optimizer();
    Optimizer(optimizer_type type,int dataset_size,int minibatch_size);
    virtual ~Optimizer();
    
    virtual void SgdUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			    Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			    Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			    Eigen::MatrixXf& momentum_dw,Eigen::VectorXf& momentum_db) const;
    
    virtual void AdamUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			     Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			     Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			     Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			     Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const;
    
    virtual void AdamWUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			      Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			      Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			      Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			      Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const;    

    optimizer_type getType() const;
    int getTimeStep() const;
    int getBatchCount() const;
    bool getMinibatchUpdate() const;
    void IncrementTimeStep();
    void IncrementBatchCount();

  };//class Optimizer
  
  class Sgd : public Optimizer{
  protected:
    float learning_rate_;
    float weight_decay_;
    float momentum_; 
    
  public:
    Sgd(int dataset_size,int minibatch_size=32,float learning_rate=1e-2,float weight_decay=1e-1,float momentum=0.9);
    virtual ~Sgd();
    
    float getLearningRate() const;
    float getWeightDecay() const;
    float getMomentum() const;

    void SgdUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
		    Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
		    Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
		    Eigen::MatrixXf& momentum_dw,Eigen::VectorXf& momentum_db) const;

  };//class Sgd

  class Adam : public Optimizer{
  protected:
    float learning_rate_;
    float weight_decay_;
    float beta1_;
    float beta2_;  
    float epsilon_ = 1e-8;
  public:
    
    Adam(int dataset_size,int minibatch_size=32,float learning_rate=1e-3,float weight_decay=0.0,float beta1=0.9,float beta2=0.999);    
    virtual ~Adam();
    
    float getLearningRate() const;
    float getBeta1() const;
    float getBeta2() const;
    float getEpsilon() const;

    void AdamUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
		     Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
		     Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
		     Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
		     Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const;    
  
  };//class Adam

  class AdamW : public Adam{    
  public:
    
    AdamW(int dataset_size,int minibatch_size=32,float learning_rate=1e-3,float weight_decay=0.1,float beta1=0.9,float beta2=0.999);    
    virtual ~AdamW();
    
    float getWeightDecay() const;
    
    void AdamWUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
		      Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
		      Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
		      Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
		      Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const;    
  
  };//class AdamW

};//namespace convnet::optimizers

#endif// OPTIMIZER_H
