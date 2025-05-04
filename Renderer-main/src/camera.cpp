#include "camera.h"

Camera::Camera(){

}

Camera::Camera(const Vector4& eye,const Vector4& gaze,const Vector4& up):eye(eye),gaze(gaze),up(up){
  
}

void Camera::Viewport(int width,int height){
  vp_mat = Matrix4::ViewportMatrix(width,height); // Viewport Matrixを計算
}

void Camera::Perspective(real fov,real aspect,real near,real far){
  real t = fabs(near)*tanf(fov/2.0);
  real b = -t;
  real r = aspect*t;
  real l = -r;
  real n = near;
  real f = far;
  
  per_mat = Matrix4::PerspectiveMatrix(l,r,b,t,n,f); // Perspective Matrixを計算    
  cam_proj_mat = per_mat*cam_mat; // Camera-Perspective Matrixとして保存
}

void Camera::LookAt(const Vector4& new_eye,const Vector4& new_gaze,const Vector4& new_up){
  eye = new_eye; gaze = new_gaze; up = new_up;

  SetOrthonormal(gaze,up);  // gaze,upベクトルから正規直交基底を計算し,メンバ変数に保存  
  cam_mat = Matrix4::CameraMatrix(u,v,w,eye); // World CoordinateからCamera Coordinateに変換する行列を計算
  inv_cam_mat = Matrix4::InvCameraMatrix(u,v,w,eye); // Camera CoordinateからWorld Coordinateに変換する行列を計算 
  
  cam_proj_mat = per_mat*cam_mat;
}

void Camera::CameraProjectionTransform(Vertex& v0,Vertex& v1,Vertex& v2) const{
  v0.v = cam_proj_mat*v0.v;
  v1.v = cam_proj_mat*v1.v;
  v2.v = cam_proj_mat*v2.v;
}

// 正規直交基底に変換
void Camera::SetOrthonormal(const Vector4& gaze,const Vector4& up){
  w = -normalize(gaze);
  u = normalize(cross(up,w));
  v = cross(w,u);
}
