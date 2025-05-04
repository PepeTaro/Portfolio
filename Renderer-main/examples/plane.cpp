#include "x11.h"
#include "utils.h"
#include "matrix4.h"
#include "vector4.h"
#include "strip.h"
#include "color.h"
#include "transform.h"
#include "material.h"
#include "pipeline.h"
#include "texture.h"
#include "context.h"
#include "type.h"

#include "polygon.h"
#include "texture_loader.h"

Context ctx;
Mesh plane;

void DrawObjects(){      
  DrawMesh(plane,ctx);
}

void InitObjects(){  
  Color c(1,1,1);
  Material material(Color(0.01,0.01,0.01),Color(0.9,0.9,0.9),32); // 材質を指定

  Vector4 p1(-1,-1,0,1);
  Vector4 p2(1,-1,0,1);
  Vector4 p3(1,1,0,1);
  Vector4 p4(-1,1,0,1);
  
  Texture texture = LoadTexture("../../images/test.png"); // テクスチャ画像読み込み
  plane = GetPlaneMesh(p1,p2,p3,p4,c,material,texture); // 平面のMeshクラスを保存
  
  Transform default_transform; // デフォルト変換(時間に依存しない,つまり毎フレームごとに必ずこの変換をする)を設定
  default_transform.pushScale(4,4,1); // x,y方向に4倍スケーリング
  default_transform.pushShearX(0.3,0.1); // x軸方向にシアリング
  default_transform.pushTranslate(0,0,-10); // z方向に-10(カメラから遠ざかる)動かす
  plane.setDefaultTransform(default_transform); // Meshクラスであるplaneにデフォルト変換を設定

}
 
void InitBuffers(int w,int h){    
  // Frame Bufferを初期化    
  InitBuffer(w,h,Color(0,0,0),ctx);    
}

void InitCamera(int w,int h){
  Vector4 eye = Vector4(0,0,0,1);
  Vector4 gaze = Vector4(0,0,-1,0);
  Vector4 up = Vector4(0,1,0,0);

  real near = -0.1;
  real far = -10000;
  real fov = M_PI/2;
  real aspect = w/h;

  // カメラ情報を設定  
  LookAt(eye,gaze,up,ctx);
  Viewport(w,h,ctx);
  Perspective(fov,aspect,near,far,ctx);
  
}

void InitLights(){
  // 光源を設定,位置が3次元空間で(10,10,0),色が白((1,1,1)は白色を表す),光源の強さを10に設定
  Light light(Vector4(10,10,0,1),Color(1,1,1),10);
  addLight(light,ctx); // 光源をコンテキストに追加
}

void Init(int w,int h){
  InitCamera(w,h);
  InitBuffers(w,h);
  InitObjects();
  InitLights();  
}

void Free(){
  FreeBuffer(ctx);
}

void DrawMain(char* data){
  // バッファをクリア
  ClearBuffer(ctx);
  
  DrawObjects(); // オブジェクトを描写
  FlipBuffer(data,ctx); // Back-bufferとFront-bufferを切り替え  
}

bool KeyboardPress(unsigned int keycode){return false;}
void MouseMove(){}
void MouseButton(){}
