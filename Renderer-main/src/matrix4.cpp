#include "matrix4.h"

Matrix4::Matrix4(){
  
}

Matrix4::Matrix4(const real mat[4][4]){
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      m[i][j] = mat[i][j];
    }
  }  
}

Vector4 Matrix4::operator*(const Vector4& v) const{  
  real x = m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z + m[0][3]*v.w;
  real y = m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z + m[1][3]*v.w;
  real z = m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z + m[2][3]*v.w;
  real w = m[3][0]*v.x + m[3][1]*v.y + m[3][2]*v.z + m[3][3]*v.w;
  
  return Vector4(x,y,z,w);
}

Matrix4 Matrix4::operator*(const Matrix4& mat) const{
  real return_mat[4][4] = {0};
  real r;

  for(int i=0;i<4;++i){
    for(int k=0;k<4;++k){
      r = m[i][k];
      for(int j=0;j<4;++j){
	return_mat[i][j] += r*mat.m[k][j];
      }
    }
  }
  
  return Matrix4(return_mat);  
}

Matrix4 Matrix4::operator+(const Matrix4& mat) const{
  real return_mat[4][4];
  
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      return_mat[i][j] = m[i][j] + mat.m[i][j];
      
    }
  }  
  
  return Matrix4(return_mat);  
}


Matrix4 Matrix4::operator-(const Matrix4& mat) const{
  real return_mat[4][4];
  
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      return_mat[i][j] = m[i][j] - mat.m[i][j];
      
    }
  }  
  
  return Matrix4(return_mat);
}

real Matrix4::operator()(int row,int col) const{
  assert(row >= 0 and row < 4);
  assert(col >= 0 and col < 4);
  
  return m[row][col];  
}

real& Matrix4::operator()(int row,int col){
  assert(row >= 0 and row < 4);
  assert(col >= 0 and col < 4);
  
  return m[row][col];
}

Matrix4 Matrix4::Identity(){
  real mat[4][4] = {0};
  
  for(int i=0;i<4;++i){
    mat[i][i] = 1.0;
  }

  return Matrix4(mat);
}

Matrix4 Matrix4::Zero(){
  real mat[4][4] = {0};    
  return Matrix4(mat);
}

Matrix4 Matrix4::RotateX(real angle){
  Matrix4 mat = Matrix4::Identity();
  real c = cosf(angle);
  real s = sinf(angle);
  
  mat.m[1][1] = c;
  mat.m[1][2] = -s;
  mat.m[2][1] = s;
  mat.m[2][2] = c;

  return mat;
}

Matrix4 Matrix4::RotateY(real angle){
  Matrix4 mat = Matrix4::Identity();
  real c = cosf(angle);
  real s = sinf(angle);

  mat.m[0][0] = c;
  mat.m[0][2] = s;
  mat.m[2][0] = -s;
  mat.m[2][2] = c;

  return mat;  
}

Matrix4 Matrix4::RotateZ(real angle){
  Matrix4 mat = Matrix4::Identity();
  real c = cosf(angle);
  real s = sinf(angle);

  mat.m[0][0] = c;
  mat.m[0][1] = -s;
  mat.m[1][0] = s;
  mat.m[1][1] = c;

  return mat;
}

Matrix4 Matrix4::Translate(real x,real y,real z){
  Matrix4 mat = Matrix4::Identity();
  mat.m[0][3] = x;
  mat.m[1][3] = y;
  mat.m[2][3] = z;

  return mat;
}

Matrix4 Matrix4::Scale(real x,real y,real z){
  Matrix4 mat = Matrix4::Identity();
  mat.m[0][0] = x;
  mat.m[1][1] = y;
  mat.m[2][2] = z;

  return mat;  
}

Matrix4 Matrix4::ShearX(real y,real z){
  Matrix4 mat = Matrix4::Identity();
  mat.m[0][1] = y;
  mat.m[0][2] = z;

  return mat;  
}

Matrix4 Matrix4::ShearY(real x,real z){
  Matrix4 mat = Matrix4::Identity();
  mat.m[1][0] = x;
  mat.m[1][2] = z;

  return mat;
}

Matrix4 Matrix4::ShearZ(real x,real y){
  Matrix4 mat = Matrix4::Identity();
  mat.m[2][0] = x;
  mat.m[2][1] = y;

  return mat;
}

Matrix4 Matrix4::Transpose(const Matrix4& mat){
  Matrix4 m = Matrix4::Zero();
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      m.m[i][j] = mat.m[j][i];
    }
  }

  return m;
}

/*
// Canonical View Volumeから,スクリーン座標へと変換する行列を返す
// スクリーンの左上が原点となるように変換
Matrix4 Matrix4::ViewportMatrix(int window_width,int window_height){
  real half_width  = window_width/2.0f;
  real half_height = window_height/2.0f;

  Matrix4 mat = Matrix4::Identity();  
  mat.m[0][0] = half_width;
  mat.m[0][3] = half_width - 0.5f;
  mat.m[1][1] = -half_height;
  mat.m[1][3] = half_height - 0.5f;
  
  return mat;  
}
*/

// Canonical View Volumeから,スクリーン座標へと変換する行列を返す
// スクリーンの左下が原点となるように変換
Matrix4 Matrix4::ViewportMatrix(int window_width,int window_height){
  real half_width  = window_width/2;
  real half_height = window_height/2;

  Matrix4 mat = Matrix4::Identity();  
  mat.m[0][0] = half_width;
  mat.m[0][3] = half_width - 0.5f;
  mat.m[1][1] = half_height;
  mat.m[1][3] = half_height - 0.5f;
  
  return mat;  
}

// カメラは,-z方向を向いているため 0 => n > f であることに注意
// 引数により表される,View VolumeをCanonical View Volumeに変換する行列を返す
Matrix4 Matrix4::OrthographicMatrix(real l,real r,real b,real t,real n,real f){
  real width  = r - l;
  real height = t - b;
  real depth  = n - f;

  real inv_w = 1.0f/width;
  real inv_h = 1.0f/height;
  real inv_d = 1.0f/depth;
  
  Matrix4 mat = Matrix4::Identity();
  
  mat.m[0][0] = 2.0f*inv_w;;
  mat.m[0][3] = -(r+l)*inv_w;
  mat.m[1][1] = 2.0f*inv_h;
  mat.m[1][3] = -(t+b)*inv_h;
  mat.m[2][2] = 2.0f*inv_d;
  mat.m[2][3] = -(n+f)*inv_d;
  
  return mat;
}

Matrix4 Matrix4::PerspectiveMatrix(real l,real r,real b,real t,real n,real f){
  Matrix4 mat = Matrix4::Zero();  
  mat.m[0][0] = n;
  mat.m[1][1] = n;
  mat.m[2][2] = (n + f);
  mat.m[2][3] = -f*n;
  mat.m[3][2] = 1.0;

  Matrix4 orth = OrthographicMatrix(l,r,b,t,n,f);
  
  return orth*mat;
}

Matrix4 Matrix4::CameraMatrix(const Vector4& u,const Vector4& v,const Vector4& w,const Vector4& eye){
  Matrix4 mat1 = Matrix4::Identity();  
  mat1.m[0][0] = u.x;
  mat1.m[0][1] = u.y;
  mat1.m[0][2] = u.z;
  
  mat1.m[1][0] = v.x;
  mat1.m[1][1] = v.y;
  mat1.m[1][2] = v.z;
  
  mat1.m[2][0] = w.x;
  mat1.m[2][1] = w.y;
  mat1.m[2][2] = w.z;

  Matrix4 mat2 = Matrix4::Identity();  
  mat2.m[0][3] = -eye.x;
  mat2.m[1][3] = -eye.y;
  mat2.m[2][3] = -eye.z;

  return mat1*mat2;
}

Matrix4 Matrix4::InvCameraMatrix(const Vector4& u,const Vector4& v,const Vector4& w,const Vector4& eye){
  Matrix4 mat = Matrix4::Identity();  
  mat.m[0][0] = u.x;
  mat.m[1][0] = u.y;
  mat.m[2][0] = u.z;
  
  mat.m[0][1] = v.x;
  mat.m[1][1] = v.y;
  mat.m[2][1] = v.z;
  
  mat.m[0][2] = w.x;  
  mat.m[1][2] = w.y;
  mat.m[2][2] = w.z;
  
  mat.m[0][3] = eye.x;  
  mat.m[1][3] = eye.y;
  mat.m[2][3] = eye.z;

  return mat;
}

Matrix4 Matrix4::RowVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3,const Vector4& v4){
  Matrix4 mat = Matrix4::Zero();
  
  for(int i=0;i<4;++i){
    mat.m[0][i] = v1[i];
    mat.m[1][i] = v2[i];
    mat.m[2][i] = v3[i];
    mat.m[3][i] = v4[i];
  }

  return mat;  
}

Matrix4 Matrix4::RowVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3){
  return RowVectorsMatrix(v1,v2,v3,Vector4(0,0,0,1));
}

Matrix4 Matrix4::ColVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3,const Vector4& v4){
  Matrix4 mat = Matrix4::Zero();
  
  for(int i=0;i<4;++i){
    mat.m[i][0] = v1[i];
    mat.m[i][1] = v2[i];
    mat.m[i][2] = v3[i];
    mat.m[i][3] = v4[i];
  }

  return mat;
}

Matrix4 Matrix4::ColVectorsMatrix(const Vector4& v1,const Vector4& v2,const Vector4& v3){
  return ColVectorsMatrix(v1,v2,v3,Vector4(0,0,0,1));
}
