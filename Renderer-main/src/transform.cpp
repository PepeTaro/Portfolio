#include "transform.h"

Transform::Transform(){
  mat = Matrix4::Identity();
  inv_mat = Matrix4::Identity();
  normal_mat = Matrix4::Identity();
}

void Transform::pushRotateX(real angle){
  Matrix4 inv_m = Matrix4::RotateX(-angle);
  
  mat = Matrix4::RotateX(angle)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;  
}

void Transform::pushRotateY(real angle){
  Matrix4 inv_m = Matrix4::RotateY(-angle);
  
  mat = Matrix4::RotateY(angle)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;
}
void Transform::pushRotateZ(real angle){
  Matrix4 inv_m = Matrix4::RotateZ(-angle);
  
  mat = Matrix4::RotateZ(angle)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;  
}

void Transform::pushTranslate(real x,real y,real z){
  mat = Matrix4::Translate(x,y,z)*mat;
  inv_mat = inv_mat*Matrix4::Translate(-x,-y,-z);
}

void Transform::pushShearX(real y,real z){
  Matrix4 inv_m = Matrix4::ShearX(-y,-z);
  
  mat = Matrix4::ShearX(y,z)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;  
}

void Transform::pushShearY(real x,real z){
  Matrix4 inv_m = Matrix4::ShearY(-x,-z);
  
  mat = Matrix4::ShearY(x,z)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;
}

void Transform::pushShearZ(real x,real y){
  Matrix4 inv_m = Matrix4::ShearZ(-x,-y);
  
  mat = Matrix4::ShearZ(x,y)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;
}

void Transform::pushScale(real x,real y,real z){
  Matrix4 inv_m = Matrix4::Scale(1.0/x,1.0/y,1.0/z);
  
  mat = Matrix4::Scale(x,y,z)*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*inv_m;
}

/**
 * 任意の変換行列をTransformに"push"するメソッド
 * 引数mは,変換行列,引数inv_mは,行列mの逆行列
 */
void Transform::pushTransformMatrix(const Matrix4& m,const Matrix4& inv_m){
  Matrix4 normal_m = Transform::get3x3Matrix(inv_m);
  
  mat = m*mat;
  inv_mat = inv_mat*inv_m;
  normal_mat = normal_mat*normal_m;
}

/**
 * 引数mat(4x4行列)の左上3x3行列部分を取り出す。
 * 
 * mat = [[m11,m12,m13,m14],
 *       [m21,m21,m23,m24],
 *       [m31,m32,m33,m34],
 *       [m41,m42,m43,m44]]
 *
 * 戻り値:
 *
 * mat = [[m11,m12,m13,0],
 *        [m21,m21,m23,0],
 *        [m31,m32,m33,0],
 *	  [0 ,   0,  0,1]]
 *
 */

Matrix4 Transform::get3x3Matrix(const Matrix4& mat){
  Matrix4 mat33 = Matrix4::Identity();
  for(int i=0;i<3;++i){
    mat33.m[0][i] = mat.m[0][i];
    mat33.m[1][i] = mat.m[1][i];
    mat33.m[2][i] = mat.m[2][i];
  }
  return mat33;
}

void Transform::clear(){
  mat = Matrix4::Identity();
  inv_mat = Matrix4::Identity();
  normal_mat = Matrix4::Identity();
}

void Transform::ModelTransform(Vertex& v0,Vertex& v1,Vertex& v2,Vector4& n) const{
  Matrix4 n_mat = Matrix4::Transpose(normal_mat);
  
  v0.v = mat*v0.v;
  v1.v = mat*v1.v;
  v2.v = mat*v2.v;

  // 三角形の各頂点に対する"法線ベクトル"を変換
  v0.n = n_mat*v0.n; 
  v1.n = n_mat*v1.n;
  v2.n = n_mat*v2.n;

  n = n_mat*n; // 三角形の面に対する法線ベクトルを変換
}
