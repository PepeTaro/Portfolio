#include "zbuffer.h"

bool DepthTest(real p1_z,real p2_z,real p3_z,real alpha,real beta,real gamma,int x,int y,Context& ctx){  
  real z_pos = alpha*p1_z + beta*p2_z + gamma*p3_z; // z_posを補間
  int i = ctx.zbuffer_size + x - y*ctx.zbuffer_width;
  
  if(z_pos + z_epsilon < ctx.zbuffer[i]) return false;
  else{
    ctx.zbuffer[i] = z_pos;
    return true;
  }      
}

void ClearZBuffer(Context& ctx){  
  memcpy(ctx.zbuffer,ctx.clear_zbuffer,ctx.zbuffer_size*sizeof(real));  
}

void InitZBuffer(int width,int height,Context& ctx){
  mallocZBuffer(width,height,ctx);
  InitZBuffer(ctx);
  ctx.use_depth_test = true;
}

void mallocZBuffer(int width,int height,Context& ctx){
  ctx.zbuffer_width = width;
  ctx.zbuffer_height = height;
  ctx.zbuffer_size = width*height;
  
  ctx.zbuffer = new real[ctx.zbuffer_size];
  ctx.clear_zbuffer = new real[ctx.zbuffer_size];// zbufferの初期値(-1.0)を保存する用
}

void FreeZBuffer(Context& ctx){
  delete[] ctx.zbuffer;
  delete[] ctx.clear_zbuffer;
}

void InitZBuffer(Context& ctx){  
  for(int i=0;i<ctx.zbuffer_size;++i){
    ctx.clear_zbuffer[i] = -1.0; // zbufferの初期値である-1.0を保存
  }
}
