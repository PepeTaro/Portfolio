#include "mesh.h"

Mesh::Mesh():num_triangles_(0),num_verts_(0){
  
}

void Mesh::appendVertex(const Vector4& v,const Color& c,const Vector4& n){
  appendVertex(v,c,n,Vector4(0,0,0,0));
}

void Mesh::appendVertex(const Vector4& v,const Color& c,const Vector4& n,const Vector4& t){  
  verts_.push_back(Vertex(v,c,n,t));
  num_verts_++;  
}

void Mesh::specifyTriangle(const std::array<int,3>& indices){
  assert(indices[0] >= 0 and indices[0] < num_verts_);
  assert(indices[1] >= 0 and indices[1] < num_verts_);
  assert(indices[2] >= 0 and indices[2] < num_verts_);
  
  indices_.push_back(indices);
  num_triangles_++;

  // 三角形の法線ベクトルを求めて保存
  int i0 = indices[0]; int i1 = indices[1]; int i2 = indices[2]; // 可読性のため,一時保存(なくてもいい)
  Vector4 n = normalize(cross(verts_[i1].v - verts_[i0].v,verts_[i2].v - verts_[i0].v));
  face_normals_.push_back(n);
}

void Mesh::setMaterial(const Material& material){
  p_ctx.material = material;
  p_ctx.material_enable = true;
}

void Mesh::setTexture(const Texture& texture){
  p_ctx.texture = texture;
  p_ctx.texture_enable = true;
}

void Mesh::setTransform(const Transform& transform){
  p_ctx.transform = transform;
}

void Mesh::setDefaultTransform(const Transform& default_transform){
  p_ctx.default_transform = default_transform;
}

void Mesh::turnOffBackfaceCulling(){
  p_ctx.backface_culling_enable = false;
}

void Mesh::turnOnBackfaceCulling(){
  p_ctx.backface_culling_enable = true;
}

int Mesh::getNumTriangles() const{return num_triangles_;}

void Mesh::getTriangle(int i,Vertex& v0,Vertex& v1,Vertex& v2,Vector4& n) const{
  assert(i >= 0 and i < num_triangles_);
  v0 = verts_[indices_[i][0]];
  v1 = verts_[indices_[i][1]];
  v2 = verts_[indices_[i][2]];
  n = face_normals_[i];
}
