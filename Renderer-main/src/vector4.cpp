#include "vector4.h"

Vector4::Vector4(){
  x = y = z = w = 0.0;
}

Vector4::Vector4(real x,real y,real z,real w):x(x),y(y),z(z),w(w){

}

Vector4& Vector4::operator=(const Vector4& v){
  x = v.x; y = v.y; z = v.z; w = v.w;
  return *this;
}

Vector4 Vector4::operator+(const Vector4& v) const{    
  return Vector4(x + v.x,y + v.y,z + v.z,w + v.w);  
}

Vector4& Vector4::operator+=(const Vector4& v){
  x += v.x; y += v.y; z += v.z; w += v.w;
  return *this;
}

Vector4 Vector4::operator-(const Vector4& v) const{
  return Vector4(x - v.x,y - v.y,z - v.z,w - v.w);  
}

Vector4& Vector4::operator-=(const Vector4& v){
  x -= v.x; y -= v.y; z -= v.z; w -= v.w;
  return *this;
}

bool Vector4::operator==(const Vector4& v) const{
  return (x == v.x and y == v.y and z == v.z and w == v.w);
}

bool Vector4::operator!=(const Vector4& v) const{
  return (x != v.x or y != v.y or z != v.z or w == v.w);
}

Vector4 Vector4::operator*(real t) const{
  return Vector4(x*t,y*t,z*t,w*t);  
}

Vector4& Vector4::operator*=(real t){
  x *= t; y *= t; z *= t; w *= t;
  return *this;
}

Vector4 Vector4::operator/(real t) const{
  assert(t != 0);
  return Vector4(x/t,y/t,z/t,w/t);  
}

Vector4& Vector4::operator/=(real t){
  assert(t != 0);
  x /= t; y /= t; z /= t; w /= t;
  return *this;
}

Vector4 Vector4::operator-() const{
  return Vector4(-x,-y,-z,-w);
}

real Vector4::operator[](int i) const{
  assert(i >= 0 and i < 4);
  if(i == 0) return x;
  if(i == 1) return y;
  if(i == 2) return z;
  return w;  
}

real& Vector4::operator[](int i){
  assert(i >= 0 and i < 4);
  if(i == 0) return x;
  if(i == 1) return y;
  if(i == 2) return z;
  return w;
}

Vector4 operator*(real t,const Vector4& v){
  return Vector4(v.x*t,v.y*t,v.z*t,v.w*t);  
}

std::ostream& operator<<(std::ostream& os, const Vector4& v){
  os << "[" << v.x << "," << v.y << "," << v.z << "," << v.w << "]";
  return os;
}

real dot(const Vector4& a,const Vector4& b){
  return (a.x*b.x + a.y*b.y + a.z*b.z);
}

real norm(const Vector4& a){
  return sqrtf(dot(a,a));
}

Vector4 cross(const Vector4& a,const Vector4& b){    
  return Vector4(a.y*b.z - a.z*b.y,a.z*b.x - a.x*b.z,a.x*b.y - a.y*b.x,0);
}

Vector4 normalize(const Vector4& v){
  real n = norm(v);
  if(n == 0) return Vector4(0,0,0,0);
  
  return Vector4(v.x/n,v.y/n,v.z/n,0);
}

// counter-clock wise
Vector4 normal(const Vector4& p1,const Vector4& p2,const Vector4& p3){
  return cross(p2 - p1,p3 - p1);
}
