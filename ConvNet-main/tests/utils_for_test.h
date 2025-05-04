#ifndef UTILS_FOR_TEST_H
#define UTILS_FOR_TEST_H

#include <cmath>
#include <array>
#include <Eigen/Dense>

static const float tolerance = 1e-4;

template <int N>
bool ComapreArrays(const std::array<int,N>& a,const std::array<int,N>& b){
  for(int i=0;i<N;++i){
    if(a[i] != b[i]) return false;
  }
  return true;
}

auto const ComapreArrays2 = &ComapreArrays<2>;
auto const ComapreArrays3 = &ComapreArrays<3>;
auto const ComapreArrays4 = &ComapreArrays<4>;
auto const ComapreArrays6 = &ComapreArrays<6>;

bool CompareMatrices(const Eigen::MatrixXf& m1,const Eigen::MatrixXf& m2);
Eigen::MatrixXf ArangeRow(const std::array<int,2>& output_size,const std::array<int,3>& info={-1,-1,-1});
Eigen::MatrixXf ArangeCol(const std::array<int,2>& output_size,const std::array<int,3>& info={-1,-1,-1});

#endif// UTILS_FOR_TEST_H
