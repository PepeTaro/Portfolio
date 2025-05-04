#include "color.h"

Color::Color(){

}

Color::Color(real r,real g,real b):r(r),g(g),b(b){

}

Color Color::operator+(const Color& c) const{
  return Color(r + c.r,g + c.g,b + c.b);
}

Color& Color::operator+=(const Color& c){
  r += c.r; g += c.g; b += c.b;
  return *this;
}

Color Color::operator*(real t) const{
  return Color(r*t,g*t,b*t);
}

Color Color::operator*(const Color& c) const{
  return Color(r*c.r,g*c.g,b*c.b);
}

Color& Color::operator*=(real t){
  r *= t; g *= t; b *= t;
  return *this;
}

Color& Color::operator*=(const Color& c){
  r *= c.r; g *= c.g; b *= c.b;
  return *this;
}

Color Color::Clamp(const Color& c){
  return Color(clamp(c.r),clamp(c.g),clamp(c.b));
}

Color Color::Interpolate(const Color& c1,const Color& c2,const Color& c3,real alpha,real beta,real gamma){
  Color color;
    
  color.r = alpha*c1.r + beta*c2.r + gamma*c3.r;
  color.g = alpha*c1.g + beta*c2.g + gamma*c3.g;
  color.b = alpha*c1.b + beta*c2.b + gamma*c3.b;
  
  return color;
}

Color Color::Interpolate(const Color& c1,const Color& c2,real t){
  Color color;
  real s = 1.0 - t;
  color.r = s*c1.r + t*c2.r;
  color.g = s*c1.g + t*c2.g;
  color.b = s*c1.b + t*c2.b;
    
  return color;
}

Color operator*(real t,const Color& c){
  return Color(c.r*t,c.g*t,c.b*t);
}
