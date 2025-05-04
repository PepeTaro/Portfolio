#include "strip.h"

Strip::Strip():num_verts_(0){
  
}

void Strip::appendVertex(const Vector4& v,const Color& c,const Vector4& n){
  appendVertex(v,c,n,Vector4(0,0,0,0));
}

void Strip::appendVertex(const Vector4& v,const Color& c,const Vector4& n,const Vector4& t){  
  verts_.push_back(Vertex(v,c,n,t));
  num_verts_++;
  
  // 三角形の法線ベクトルを求めて保存
  if(num_verts_ >= 3){ // 三角形が形成できる最低条件(3点が与えた場合)を満たした場合
    Vector4 p0 = verts_[num_verts_ - 3].v;
    Vector4 p1,p2;
      
    if(num_verts_%2 == 0){ // 頂点数が偶数の場合      
      p1 = verts_[num_verts_ - 1].v;
      p2 = verts_[num_verts_ - 2].v;
    }else{ // 奇数の場合      
      p1 = verts_[num_verts_ - 2].v;
      p2 = verts_[num_verts_ - 1].v;      
    }
    
    Vector4 normal = normalize(cross(p1 - p0,p2 - p0));
    face_normals_.push_back(normal);
  }  
}

void Strip::setMaterial(const Material& material){
  p_ctx.material = material;
  p_ctx.material_enable = true;  
}

void Strip::setTexture(const Texture& texture){
  p_ctx.texture = texture;
  p_ctx.texture_enable = true;
}

void Strip::setTransform(const Transform& transform){
  p_ctx.transform = transform;
}

void Strip::setDefaultTransform(const Transform& default_transform){
  p_ctx.default_transform = default_transform;
}

void Strip::turnOffBackfaceCulling(){
  p_ctx.backface_culling_enable = false;
}

void Strip::turnOnBackfaceCulling(){
  p_ctx.backface_culling_enable = true;
}

int Strip::getNumVertices() const{return num_verts_;}

void Strip::getTriangle(int i,Vertex& v0,Vertex& v1,Vertex& v2,Vector4& n) const{
  assert(num_verts_ >= 3); // 少なくとも3点必要
  assert(i >= 0 and i < num_verts_);
  
  v0 = verts_[i];
  v1 = verts_[i+1];
  v2 = verts_[i+2];
  n = face_normals_[i];
}

