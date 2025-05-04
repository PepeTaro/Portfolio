#ifndef STRIP_H
#define STRIP_H

#include <vector>
#include <array>

#include "vector4.h"
#include "color.h"
#include "material.h"
#include "transform.h"
#include "vertex.h"
#include "texture.h"
#include "context.h"
#include "type.h"

class Strip{
private:
  int num_verts_; // メッシュに含まれる頂点数
  std::vector<Vertex> verts_; // 頂点を保存
  std::vector<Vector4> face_normals_; // メッシュに含まれる各々の三角形に対する,法線ベクトルを保存(Backface Culling時に必要)

public:    
  PolygonContext p_ctx;
  
public:
  Strip();
  void appendVertex(const Vector4& v,const Color& c,const Vector4& n);
  void appendVertex(const Vector4& v,const Color& c,const Vector4& n,const Vector4& t);
  
  void setMaterial(const Material& material);
  void setTexture(const Texture& texture);
  void setTransform(const Transform& transform);
  void setDefaultTransform(const Transform& default_transform);
  void turnOffBackfaceCulling();
  void turnOnBackfaceCulling();
  
  int getNumVertices() const;
  void getTriangle(int i,Vertex& v0,Vertex& v1,Vertex& v2,Vector4& n) const;

};//class Strip

#endif// STRIP_H
