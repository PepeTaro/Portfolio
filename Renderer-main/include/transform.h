#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "matrix4.h"
#include "vector4.h"
#include "vertex.h"
#include "camera.h"
#include "type.h"

class Transform{
public:
  Matrix4 mat;
  Matrix4 inv_mat;
  Matrix4 normal_mat; // 法線ベクトルを"正しく"変換する行列(法線ベクトルにmat_を掛けても法線ベクトルとならないため)

public:
  Transform();
  void pushRotateX(real angle);
  void pushRotateY(real angle);
  void pushRotateZ(real angle);
  void pushTranslate(real x,real y,real z);
  void pushShearX(real y,real z);
  void pushShearY(real x,real z);
  void pushShearZ(real x,real y);
  void pushScale(real x,real y,real z);
  
  void pushTransformMatrix(const Matrix4& m,const Matrix4& inv_m);  
  static Matrix4 get3x3Matrix(const Matrix4& inv_m);
  
  void clear();
    
  void ModelTransform(Vertex& v0,Vertex& v1,Vertex& v2,Vector4& n) const;
  
};//class Transform
#endif// TRANSFORM_H
