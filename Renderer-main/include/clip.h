#ifndef CLIP_H
#define CLIP_H

#include "vector4.h"
#include "color.h"
#include "vertex.h"
#include "shading.h"
#include "context.h"
#include "type.h"

const int LEFT = 1 << 0;
const int RIGHT = 1 << 1;
const int BOTTOM = 1 << 2;
const int TOP = 1 << 3;
const int FAR  = 1 << 4;
const int NEAR  = 1 << 5;

//数値計算の誤差で実際にはクリッピング内にあるのに,クリッピング外と判定される誤差を緩和するための定数
// 値が小さくなればなるほど,実際のCanonical View Volumeに近づくが,本来はクリッピング内にある点が,
//  数値誤差で点がクリッピング外と判定される回数が増す(クリッピング外のポリゴンは表示しないため,チラツキが頻繁に起こる)
// 値が大きくなればなるほど,本来クリッピング外にある点をCanonical View Volume内と判定し,レンダリングがおかしくなる(例えば,後ろのオブジェクトが見えるとか)
// Farの絶対値を上げれば,ある程度問題は回避できる

const real clip_epsilon = 1e-3;

bool BackfaceCulling(const Vector4& pos,const Vector4& n,const Context& ctx);
int Outcode(const Vector4& p);
real Intersect(const Vector4& p1,const Vector4& p2,int cp,Vector4& q);
void Clipping(const Vertex& p1,const Vertex& p2,const Vertex& p3,int cp,const PolygonContext& p_ctx,Context& ctx);

#endif// CLIP_H
