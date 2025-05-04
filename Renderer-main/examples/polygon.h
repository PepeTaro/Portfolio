#ifndef POLYGON_H
#define POLYGON_H

#include "mesh.h"
#include "strip.h"

/**
 * デモ用ポリゴン生成ユーティリティ関数
 */

/**
 *   Mesh
 **/
Mesh GetTriangleMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Color& c1,const Color& c2,const Color& c3,const Material& material);
Mesh GetTriangleMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Color& c,const Material& material);

Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,
		  const Color& c1,const Color& c2,const Color& c3,const Color& c4,
		  const Material& material,const Texture& texture);

// テクスチャ座標は,p1,p2,p3,p4がそれぞれ(0,0),(1,0),(1,1),(0,1)となるように配置
Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,
		  const Color& c1,const Color& c2,const Color& c3,const Color& c4,const Material& material);

Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,const Color& c,const Material& material,const Texture& texture);
Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,const Color& c,const Material& material);

Mesh GetCubeMesh(float l,float b,float n,float r,float t,float f,
		       const Color& c1,const Color& c2,const Color& c3,const Color& c4,const Color& c5,const Color& c6,const Color& c7,const Color& c8,
		       const Material& material);
Mesh GetCubeMesh(float l,float b,float n,float r,float t,float f,const Color& c,const Material& material);

// nが大きければ,球の形が"滑らか"になる
Mesh GetSphereMesh(int n,real r,const Color& c,const Material& material,const Texture& texture);
Mesh GetSphereMesh(int n,real r,const Color& c,const Material& material);

/** 
 *  Strip
 **/
Strip GetTriangleStrip(const Vector4& p1,const Vector4& p2,const Vector4& p3,
		       const Color& c1,const Color& c2,const Color& c3,
		       const Vector4& t1,const Vector4& t2,const Vector4& t3,
		       const Material& material);

Strip GetTriangleStrip(const Vector4& p1,const Vector4& p2,const Vector4& p3,
		       const Color& c,
		       const Vector4& t1,const Vector4& t2,const Vector4& t3,
		       const Material& material);

#endif// POLYGON_H
