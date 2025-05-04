#include "utils_for_test.h"

bool CompareMatrices(const Eigen::MatrixXf& m1,const Eigen::MatrixXf& m2){
  if(m1.rows() != m2.rows() or m1.cols() != m2.cols()) return false;

  for(int i=0;i<m1.rows();++i){
    for(int j=0;j<m1.cols();++j){
      if(fabs( m1(i,j) - m2(i,j) ) > tolerance) return false;
    }
  }
  return true;
}

Eigen::MatrixXf ArangeRow(const std::array<int,2>& output_size,const std::array<int,3>& info){
  int size,low,high;
  
  if(info[0] == -1){ // デフォルトの動作。
    int tmp = output_size[0]*output_size[1];
    size = high = tmp;
    low = 1;
  }else{    
    size = info[0];
    low = info[1];
    high = info[2];
  }
  
  Eigen::VectorXf vec = Eigen::VectorXf::LinSpaced(size,low,high);// numpyで言う,arangeの様な関数。
  return Eigen::Map<Eigen::MatrixXf>(vec.data(),output_size[1],output_size[0]).transpose();
}

Eigen::MatrixXf ArangeCol(const std::array<int,2>& output_size,const std::array<int,3>& info){
  int size,low,high;
  
  if(info[0] == -1){
    int tmp = output_size[0]*output_size[1];
    size = high = tmp;
    low = 1;
  }else{    
    size = info[0];
    low = info[1];
    high = info[2];
  }
    
  Eigen::VectorXf vec = Eigen::VectorXf::LinSpaced(size,low,high);
  return Eigen::Map<Eigen::MatrixXf>(vec.data(),output_size[0],output_size[1]);
}
