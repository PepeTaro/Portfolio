#ifndef LIGHT_H
#define LIGHT_H

#include "vector4.h"
#include "color.h"

class Light{
public:
  Vector4 v; // 光源の位置
  Color c; // 光源の色
  real i; // 光源の強さ
public:
  Light();
  Light(const Vector4& v,const Color& c,real i);
  
};//class Light

#endif// LIGHT_H
