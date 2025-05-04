#ifndef VECTOR4_H
#define VECTOR4_H

#include <cmath>
#include <iostream>
#include <ostream>

#include "type.h"

class Vector4{  
public:
  real x,y,z,w;
public:
  Vector4();
  Vector4(real x,real y,real z,real w);
  Vector4& operator=(const Vector4& v);
  Vector4 operator+(const Vector4& v) const;
  Vector4& operator+=(const Vector4& v);
  Vector4 operator-(const Vector4& v) const;
  Vector4& operator-=(const Vector4& v);
  bool operator==(const Vector4& v) const;
  bool operator!=(const Vector4& v) const;
  Vector4 operator*(real t) const;
  Vector4& operator*=(real t);
  Vector4 operator/(real t) const;
  Vector4& operator/=(real t);
  Vector4 operator-() const;
  real operator[](int i) const;  
  real& operator[](int i);

  friend Vector4 operator*(real t,const Vector4& v);
  friend std::ostream& operator<<(std::ostream& os, const Vector4& v);
  
};//class Vector4

// 以下のユーティリティ関数はすべて,引数が３次元空間ベクトルであると仮定している
// つまり,w成分は0であると仮定,戻り値もw成分が0であるベクトルを返す
real dot(const Vector4& a,const Vector4& b);
real norm(const Vector4& a);
Vector4 cross(const Vector4& a,const Vector4& b);
Vector4 normalize(const Vector4& v);
Vector4 normal(const Vector4& p1,const Vector4& p2,const Vector4& p3);

#endif// VECTOR4_H
