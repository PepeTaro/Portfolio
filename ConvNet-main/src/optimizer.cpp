#include "optimizer.h"

namespace convnet::optimizers{

  Optimizer::Optimizer(){}
  Optimizer::Optimizer(optimizer_type type,int dataset_size,int minibatch_size):
    type_(type),dataset_size_(dataset_size),minibatch_size_(minibatch_size),minibatch_update_(false),batch_count_(1),time_step_(0){}
  Optimizer::~Optimizer(){}    
  optimizer_type Optimizer::getType() const{ return type_;}
  int Optimizer::getTimeStep() const{return time_step_;}
  int Optimizer::getBatchCount() const{return batch_count_;}
  bool Optimizer::getMinibatchUpdate() const{return minibatch_update_;}
  void Optimizer::IncrementTimeStep(){ time_step_++;}
  void Optimizer::IncrementBatchCount(){
    
    if(batch_count_ == dataset_size_){ // batch_count_がdataset_size_に達した場合(つまり,各々のEpochの最後のデータの場合)
      minibatch_update_ = true;        // データサイズ数がminibatch_sizeで割り切れない場合,gradientの"余り"があるので強制的にパラメーター更新
      batch_count_ = 1; // カウンターをリセット
    }else if(batch_count_%minibatch_size_ == 0){ // batch_count_がminibatch_size_倍の場合
      minibatch_update_ = true;
      batch_count_++;
    }else{
      minibatch_update_ = false;
      batch_count_++;
    }
    
  }
  
  void Optimizer::SgdUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			     Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			     Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			     Eigen::MatrixXf& momentum_dw,Eigen::VectorXf& momentum_db) const{    
    std::cerr << "Error:(Optimizer::SgdUpdate) Sgd以外のOptimizerはこのメソッドを使用できません" << std::endl;
    exit(-1);    
  }
  
  void Optimizer::AdamUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			      Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			      Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			      Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			      Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const{
    std::cerr << "Error:(Optimizer::AdamUpdate) Adam以外のOptimizerはこのメソッドを使用できません" << std::endl;
    exit(-1);
  }
  
  void Optimizer::AdamWUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			      Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			      Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			      Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			      Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const{
    std::cerr << "Error:(Optimizer::AdamWUpdate) AdamW以外のOptimizerはこのメソッドを使用できません" << std::endl;
    exit(-1);
  }

  
  Sgd::Sgd(int dataset_size,int minibatch_size,float learning_rate,float weight_decay,float momentum):
    Optimizer(optimizer_type::Sgd,dataset_size,minibatch_size),learning_rate_(learning_rate),weight_decay_(weight_decay),momentum_(momentum){}
  
  Sgd::~Sgd(){}    
  float Sgd::getLearningRate() const{return learning_rate_;}
  float Sgd::getWeightDecay() const{return weight_decay_;}
  float Sgd::getMomentum() const{return momentum_;}    
  
  void Sgd::SgdUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
		       Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
		       Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
		       Eigen::MatrixXf& momentum_dw,Eigen::VectorXf& momentum_db) const{
    
    // minibatch分をgradientを蓄積
    minibatch_dw += dw;
    minibatch_db += db;
    
    if(minibatch_update_){
      
      // Momentumを計算
      momentum_dw = momentum_*momentum_dw + (1.0/minibatch_size_)*minibatch_dw + weight_decay_*weight;
      momentum_db = momentum_*momentum_db + (1.0/minibatch_size_)*minibatch_db;

      // SGDを実行
      weight -= learning_rate_*momentum_dw;
      bias   -= learning_rate_*momentum_db;  

      // minibatchの蓄積をクリア
      minibatch_dw.setZero();
      minibatch_db.setZero();     
      
    }            
  }

  Adam::Adam(int dataset_size,int minibatch_size,float learning_rate,float weight_decay,float beta1,float beta2):
    Optimizer(optimizer_type::Adam,dataset_size,minibatch_size),learning_rate_(learning_rate),weight_decay_(weight_decay),beta1_(beta1),beta2_(beta2){}
  Adam::~Adam(){}
    
  float Adam::getLearningRate() const{return learning_rate_;}
  float Adam::getBeta1() const{return beta1_;}
  float Adam::getBeta2() const{return beta2_;}
  float Adam::getEpsilon() const{return epsilon_;}    
    
  void Adam::AdamUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			 Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			 Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			 Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			 Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const{
    
    // minibatch分をgradientを蓄積
    minibatch_dw += dw;
    minibatch_db += db;

    if(minibatch_update_){ 

      Eigen::MatrixXf g_dw = (1.0/minibatch_size_)*minibatch_dw + weight_decay_*weight;
      Eigen::VectorXf g_db = (1.0/minibatch_size_)*minibatch_db;
      
      momentum_dw1 = beta1_*momentum_dw1 + (1 - beta1_)*g_dw;
      momentum_db1 = beta1_*momentum_db1 + (1 - beta1_)*g_db;
      
      momentum_dw2 = beta2_*momentum_dw2 + (1 - beta2_)*g_dw.array().pow(2).matrix();
      momentum_db2 = beta2_*momentum_db2 + (1 - beta2_)*g_db.array().pow(2).matrix();
	
      Eigen::MatrixXf m_dw = momentum_dw1/(1 - powf(beta1_,time_step_));
      Eigen::MatrixXf m_db = momentum_db1/(1 - powf(beta1_,time_step_));
      
      Eigen::MatrixXf v_dw = momentum_dw2/(1 - powf(beta2_,time_step_));
      Eigen::MatrixXf v_db = momentum_db2/(1 - powf(beta2_,time_step_));

      weight  -=  learning_rate_*(m_dw.array()/(v_dw.array().sqrt() + epsilon_)).matrix();
      bias    -=  learning_rate_*(m_db.array()/(v_db.array().sqrt() + epsilon_)).matrix();
      
      // minibatchの蓄積をクリア
      minibatch_dw.setZero();
      minibatch_db.setZero();
    }
  }


  AdamW::AdamW(int dataset_size,int minibatch_size,float learning_rate,float weight_decay,float beta1,float beta2):
    Adam(dataset_size,minibatch_size,learning_rate,weight_decay,beta1,beta2){
    type_ = optimizer_type::AdamW;
  }
  AdamW::~AdamW(){}
    
  float AdamW::getWeightDecay() const{return weight_decay_;}
    
  void AdamW::AdamWUpdate(const Eigen::MatrixXf& dw,const Eigen::VectorXf& db,
			   Eigen::MatrixXf& minibatch_dw,Eigen::VectorXf& minibatch_db,
			   Eigen::MatrixXf& weight,Eigen::VectorXf& bias,
			   Eigen::MatrixXf& momentum_dw1,Eigen::VectorXf& momentum_db1,
			   Eigen::MatrixXf& momentum_dw2,Eigen::VectorXf& momentum_db2) const{
    
    // minibatch分をgradientを蓄積
    minibatch_dw += dw;
    minibatch_db += db;
    
    if(minibatch_update_){ 

      Eigen::MatrixXf g_dw = (1.0/minibatch_size_)*minibatch_dw;
      Eigen::VectorXf g_db = (1.0/minibatch_size_)*minibatch_db;
      
      momentum_dw1 = beta1_*momentum_dw1 + (1 - beta1_)*g_dw;
      momentum_db1 = beta1_*momentum_db1 + (1 - beta1_)*g_db;
      
      momentum_dw2 = beta2_*momentum_dw2 + (1 - beta2_)*g_dw.array().pow(2).matrix();
      momentum_db2 = beta2_*momentum_db2 + (1 - beta2_)*g_db.array().pow(2).matrix();
	
      Eigen::MatrixXf m_dw = momentum_dw1/(1 - powf(beta1_,time_step_));
      Eigen::MatrixXf m_db = momentum_db1/(1 - powf(beta1_,time_step_));
      
      Eigen::MatrixXf v_dw = momentum_dw2/(1 - powf(beta2_,time_step_));
      Eigen::MatrixXf v_db = momentum_db2/(1 - powf(beta2_,time_step_));

      weight  -=  learning_rate_*(m_dw.array()/(v_dw.array().sqrt() + epsilon_) + weight_decay_*weight.array()).matrix();
      bias    -=  learning_rate_*(m_db.array()/(v_db.array().sqrt() + epsilon_)).matrix();
          
      // minibatchの蓄積をクリア
      minibatch_dw.setZero();      
      minibatch_db.setZero();
    }
  }

};//namespace convnet::optimizers;
