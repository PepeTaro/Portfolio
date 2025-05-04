#include "x11.h"
#include "utils.h"

#include "matrix4.h"
#include "vector4.h"
#include "strip.h"
#include "color.h"
#include "transform.h"
#include "material.h"
#include "pipeline.h"
#include "type.h"

Context ctx;

Mesh fractal;
int fractal_idx = 0;

// depthは再帰回数
void SierpinskiRecursive(Mesh& mesh,const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& n,
			 const Color& c1,const Color& c2,const Color& c3,int depth)
{
  if(depth == 0){
    mesh.appendVertex(p1,c1,n);
    mesh.appendVertex(p2,c2,n);
    mesh.appendVertex(p3,c3,n);
    
    mesh.specifyTriangle({fractal_idx,fractal_idx+1,fractal_idx+2});    
    fractal_idx += 3;
    return;
  }

  Vector4 q1 = (p1+p2)/2.0f;
  Vector4 q2 = (p2+p3)/2.0f;
  Vector4 q3 = (p1+p3)/2.0f;

  Color q1_c = Color::Interpolate(c1,c2,0.5);
  Color q2_c = Color::Interpolate(c2,c3,0.5);
  Color q3_c = Color::Interpolate(c1,c3,0.5);
  
  SierpinskiRecursive(mesh,p1,q1,q3,n,c1,q1_c,q3_c,depth-1);
  SierpinskiRecursive(mesh,q1,p2,q2,n,q1_c,c2,q2_c,depth-1);
  SierpinskiRecursive(mesh,q3,q2,p3,n,q3_c,c2,c3,depth-1);    
}

// fractal_idxを0に初期化するためのラッピング関数
void Sierpinski(Mesh& mesh,const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& n,
		const Color& c1,const Color& c2,const Color& c3,int depth)
{
  fractal_idx = 0;
  SierpinskiRecursive(mesh,p1,p2,p3,n,c1,c2,c3,depth);
}

void InitObjects(){
  Color r(1,0,0);
  Color g(0,1,0);
  Color b(0,0,1);

  Vector4 p1(-10,-10,-20,1);
  Vector4 p2(10,-10,-20,1);
  Vector4 p3(0,10,-20,1);

  Vector4 n = normalize(cross(p2 - p1,p3 - p1));// 法線ベクトル
  Material material(Color(0.9,0.9,0.9),Color(0.9,0.9,0.9),1);
  
  Sierpinski(fractal,p1,p2,p3,n,r,g,b,6);
  
  fractal.turnOffBackfaceCulling();
  fractal.setMaterial(material);  
}
 
void DrawObjects(){
  DrawMesh(fractal,ctx);
}

void InitBuffers(int w,int h){  
  // Frame Bufferを初期化
  // w,hはウィンドウのサイズ,Colorはバックグラウンドカラーに相当
  InitBuffer(w,h,Color(0.0,0.0,0.0),ctx);    
}

void InitCamera(int w,int h){
  Vector4 eye = Vector4(0,0,0,1);
  Vector4 gaze = Vector4(0,0,-1,0);
  Vector4 up = Vector4(0,1,0,0);

  real near = -0.1;
  real far = -1000;
  real fov = M_PI/2;
  real aspect = w/h;

  // カメラ情報をセット  
  LookAt(eye,gaze,up,ctx);
  Viewport(w,h,ctx);
  Perspective(fov,aspect,near,far,ctx);
}

void Init(int w,int h){  
  InitCamera(w,h);
  InitBuffers(w,h);
  InitObjects();
}

void Free(){
  FreeBuffer(ctx);
}

void DrawMain(char* data){
  static real delta;
  delta = GetDeltaTime();
  
  // バッファをクリア
  ClearBuffer(ctx);
  
  DrawObjects(); // オブジェクトを描写
  FlipBuffer(data,ctx); // Back-bufferとFront-bufferを切り替え
}

bool KeyboardPress(unsigned int keycode){return false;}
void MouseMove(){}
void MouseButton(){}
