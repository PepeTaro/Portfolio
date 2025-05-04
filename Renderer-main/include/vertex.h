#ifndef VERTEX_H
#define VERTEX_H

#include "vector4.h"
#include "color.h"
#include "texture.h"
#include "type.h"

class Vertex{
public:
  Vector4 v;// Vertexの位置を表すベクトル
  Color c;  // Vertexの位置における色
  Vector4 n; // Vertexの位置における法線
  Vector4 t; // Texture coodinate
public:
  Vertex();
  Vertex(const Vector4& v,const Color& c,const Vector4& n,const Vector4& t);
  
};//class Vertex

#endif// VERTEX_H
