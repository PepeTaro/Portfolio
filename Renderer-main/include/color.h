#ifndef COLOR_H
#define COLOR_H

#include "type.h"
#include "utils.h"

class Color{  
public:
  real r,g,b;
public:
  Color();
  Color(real r,real g,real b);  
  Color operator+(const Color& c) const;
  Color& operator+=(const Color& c);
  Color operator*(real t) const;
  Color operator*(const Color& c) const;
  Color& operator*=(real t);
  Color& operator*=(const Color& c);

  static Color Clamp(const Color& c);
  static Color Interpolate(const Color& c1,const Color& c2,const Color& c3,real alpha,real beta,real gamma);
  static Color Interpolate(const Color& c1,const Color& c2,real t);
  friend Color operator*(real t,const Color& c);
  
};//class Color


#endif// COLOR_H
