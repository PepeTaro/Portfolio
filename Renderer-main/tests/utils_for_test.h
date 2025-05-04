#ifndef UTILS_H
#define UTILS_H

#include <cmath>

#include "vector4.h"
#include "matrix4.h"
#include "type.h"

static const real random_max = 1000.0;
static const real random_min = -1000.0;
static const int num_test_random = 100;
static const real epsilon = 1e-8;

inline real GenerateRandom(real min,real max){
  real r = static_cast<real>(rand()) / static_cast<real>(RAND_MAX);
  return r*(max - min) + min;
}

inline real GenerateRandom(){
  return GenerateRandom(random_min,random_max);
}

inline Matrix4 RandomMatrix4(){
  Matrix4 m = Matrix4::Zero();
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      m(i,j) = GenerateRandom(random_min,random_max);
    }
  }
  return m;
}

inline Vector4 RandomVector4(){
  Vector4 v({0,0,0,0});
  for(int i=0;i<4;++i){
    v[i] = GenerateRandom(random_min,random_max);    
  }
  return v;
}

inline bool AlmostEqual(real a,real b){
  if(fabs(a-b) < epsilon) return true;
  return false;
}

inline bool EXPECT_VEC4_EQ(const Vector4& a,const Vector4& b){
  for(int i=0;i<4;++i){
    if(a[i] != b[i]) return false;
  }
  return true;
}

inline bool EXPECT_MAT4_EQ(const Matrix4& a,const Matrix4& b){
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      if(a(i,j) != b(i,j)) return false;
    }
  }
  return true;
}

#endif// UTILS_H
