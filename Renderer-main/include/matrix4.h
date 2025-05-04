#ifndef MATRIX4_H
#define MATRIX4_H

#include "vector4.h"
#include "type.h"

class Matrix4{
public:
  real m[4][4];
public:
  Matrix4();
  Matrix4(const real mat[4][4]);
  
  Vector4 operator*(const Vector4& v) const;
  Matrix4 operator*(const Matrix4& mat) const;
  Matrix4 operator+(const Matrix4& mat) const;
  Matrix4 operator-(const Matrix4& mat) const;
  real operator()(int row,int col) const;
  real& operator()(int row,int col);
  
  static Matrix4 Identity();
  static Matrix4 Zero();
  static Matrix4 RotateX(real angle);
  static Matrix4 RotateY(real angle);
  static Matrix4 RotateZ(real angle);
  static Matrix4 Translate(real x,real y,real z);
  static Matrix4 Scale(real x,real y,real z);
  static Matrix4 ShearX(real y,real z);
  static Matrix4 ShearY(real x,real z);
  static Matrix4 ShearZ(real x,real y);  
  static Matrix4 Transpose(const Matrix4& mat);
  
  static Matrix4 ViewportMatrix(int window_width,int window_height);
  static Matrix4 OrthographicMatrix(real l,real r,real b,real t,real n,real f);
  static Matrix4 PerspectiveMatrix(real l,real r,real b,real t,real n,real f);
  
  static Matrix4 CameraMatrix(const Vector4& u,const Vector4& v,const Vector4& w,const Vector4& eye);  
  static Matrix4 InvCameraMatrix(const Vector4& u,const Vector4& v,const Vector4& w,const Vector4& eye);

  static Matrix4 RowVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3,const Vector4& v4);
  static Matrix4 RowVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3);
  static Matrix4 ColVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3,const Vector4& v4);
  static Matrix4 ColVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3);

};//class Matrix4;

#endif// MATRIX4_H
