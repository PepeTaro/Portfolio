#ifndef CAMERA_H
#define CAMERA_H

#include <cmath>
#include "vector4.h"
#include "matrix4.h"
#include "vertex.h"
#include "type.h"

class Camera{  
public:
  Vector4 eye,gaze,up;
  Vector4 u,v,w;  // 正規直交基底
  // Viewport Matrix,Perspective Matrix,Camera Matrix,Inverse Camera Matrix.
  Matrix4 vp_mat,per_mat,cam_mat,inv_cam_mat;
  Matrix4 cam_proj_mat;
  
  Camera();
  Camera(const Vector4& eye,const Vector4& gaze,const Vector4& up);
  void Viewport(int width,int height);
  void Perspective(real fov,real aspect,real near,real far);
  void LookAt(const Vector4& new_eye,const Vector4& new_gaze,const Vector4& new_up);  
  void CameraProjectionTransform(Vertex& v0,Vertex& v1,Vertex& v2) const;

private:
  void SetOrthonormal(const Vector4& gaze,const Vector4& up);
  
};//class Camera

#endif// CAMERA_H
