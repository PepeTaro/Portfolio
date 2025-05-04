#ifndef ZBUFFER_H
#define ZBUFFER_H

#include <cstring>
#include "context.h"
#include "type.h"

static real z_epsilon = 1e-5; // depth-test時の数値誤差回避用として使用(この変数を使用しないと,重なったオブジェクトの互いの色がチカチカする(数値誤差の影響))

bool DepthTest(real p1_z,real p2_z,real p3_z,real alpha,real beta,real gamma,int x,int y,Context& ctx);
void ClearZBuffer(Context& ctx);
void InitZBuffer(int width,int height,Context& ctx);
void mallocZBuffer(int width,int height,Context& ctx);
void FreeZBuffer(Context& ctx);
void InitZBuffer(Context& ctx);

#endif// ZBUFFER_H
