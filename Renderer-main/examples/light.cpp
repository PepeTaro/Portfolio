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

#include "polygon.h"

// 光源を定義
Light light_red(Vector4(0,10,-10,1),Color(1,0,0),100);
Light light_green(Vector4(0,0,-10,1),Color(0,1,0),100);
Light light_blue(Vector4(0,-10,-10,1),Color(0,0,1),100);

int red_id;
int green_id;
int blue_id;

Context ctx;
Mesh plane;
Mesh red_ball,green_ball,blue_ball;

void InitObjects(){
  Color c(1,1,1);
  Material material(Color(0.1,0.1,0.1),Color(0.9,0.9,0.9),32);

  Vector4 p1(-1,-1,0,1);
  Vector4 p2(1,-1,0,1);
  Vector4 p3(1,1,0,1);
  Vector4 p4(-1,1,0,1);

  // 平面
  plane = GetPlaneMesh(p1,p2,p3,p4,c,material);  
  {
    Transform default_transform;
    default_transform.pushScale(8,8,1);
    default_transform.pushTranslate(0,0,-20);
    plane.setDefaultTransform(default_transform);
  }

  // 以下において,それぞれ赤,緑,青の球体を定義(光源を表す)
  
  // 赤
  red_ball = GetSphereMesh(20,1,Color(1,0,0),material);
  {
    Transform default_transform;
    default_transform.pushTranslate(light_red.v.x,light_red.v.y,light_red.v.z);
    red_ball.setDefaultTransform(default_transform);
  }

  // 緑
  green_ball = GetSphereMesh(20,1,Color(0,1,0),material);
  {
    Transform default_transform;
    default_transform.pushTranslate(light_green.v.x,light_green.v.y,light_green.v.z);
    green_ball.setDefaultTransform(default_transform);
  }

  // 青
  blue_ball = GetSphereMesh(20,1,Color(0,0,1),material);
  {
    Transform default_transform;
    default_transform.pushTranslate(light_blue.v.x,light_blue.v.y,light_blue.v.z);
    blue_ball.setDefaultTransform(default_transform);
  }
  
}
 
void Update(real delta){
  static real angle1 = 0.0;
  static real angle2 = 0.0;
  static real angle3 = 0.0;
  
  if(delta >= 10)
    delta = 10;
  else if(delta < 0)
    delta = 0;
  
  if(angle1 >= 2*M_PI) angle1 = 0.0;
  if(angle2 >= 2*M_PI) angle2 = 0.0;
  if(angle3 >= 2*M_PI) angle3 = 0.0;

  angle1 += delta;
  angle2 += 0.7*delta;
  angle3 += 0.4*delta;
  
  { // 赤
    // 上記において定義した平面の回りを,Y軸に関してangle1角度回転
    Transform transform;
    transform.pushTranslate(0,0,30);
    transform.pushRotateY(angle1);
    transform.pushTranslate(0,0,-30);  
    red_ball.setTransform(transform);

    // 光源も一緒に回転
    ctx.lights[red_id].v = transform.mat*light_red.v;
  }

  { // 緑
    Transform transform;
    transform.pushTranslate(0,0,30);
    transform.pushRotateY(angle2);
    transform.pushTranslate(0,0,-30);  
    green_ball.setTransform(transform);
    
    ctx.lights[green_id].v = transform.mat*light_green.v;
  }

  { // 青
    Transform transform;
    transform.pushTranslate(0,0,30);
    transform.pushRotateY(angle3);
    transform.pushTranslate(0,0,-30);  
    blue_ball.setTransform(transform);
    
    ctx.lights[blue_id].v = transform.mat*light_blue.v;
  }

}

void DrawObjects(){
  DrawMesh(plane,ctx);
  DrawMesh(red_ball,ctx);
  DrawMesh(green_ball,ctx);
  DrawMesh(blue_ball,ctx);
}

void InitBuffers(int w,int h){  
  InitBuffer(w,h,Color(0.0,0.0,0.0),ctx);
  InitZBuffer(w,h,ctx);
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

void InitLights(){  
  // 光を追加して,後の処理のためにidを保存
  red_id = addLight(light_red,ctx);
  green_id = addLight(light_green,ctx);
  blue_id = addLight(light_blue,ctx);
}

void Init(int w,int h){  
  InitCamera(w,h);
  InitBuffers(w,h);
  InitObjects();
  InitLights();  
}

void Free(){
  FreeBuffer(ctx);
  FreeZBuffer(ctx);
}

void DrawMain(char* data){
  static real delta;
  delta = GetDeltaTime();
  
  Update(delta); // オブジェクトを更新(位置など)
  
  // バッファをクリア
  ClearBuffer(ctx);
  ClearZBuffer(ctx);
  
  DrawObjects(); // オブジェクトを描写
  FlipBuffer(data,ctx); // Back-bufferとFront-bufferを切り替え
}

bool KeyboardPress(unsigned int keycode){return false;}
void MouseMove(){}
void MouseButton(){}
