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

std::vector<Mesh> meshes;
Light sun_light(Vector4(0,-100,-800,1),Color(1,1,1),10000);

void DrawObjects(){      
  for(auto mesh : meshes){
    DrawMesh(mesh,ctx);
  }  
}

void InitObjects(){  
  Color r(1,0,0);
  Color g(0,1,0);
  Color b(0,0,1);
  Color w(1,1,1);
  
  Material earth_material(Color(0.1,0.1,0.1),Color(0.3,0.3,0.3),8);
  Material sun_material(Color(1.0,1.0,1.0),Color(1.0,1.0,1.0),1);
  
  Texture earth_texture = LoadTexture("../../images/earth.png");
  Texture sun_texture = LoadTexture("../../images/sun.png");
  
  Mesh earth,sun;

  earth = GetSphereMesh(50,50,w,earth_material,earth_texture);
  sun = GetSphereMesh(50,100,w,sun_material,sun_texture);
  
  meshes.push_back(earth); 
  meshes.push_back(sun);  
}
 
void Update(real delta){  
  if(delta >= 10)
    delta = 10;
  else if(delta < 0)
    delta = 0;
  
  static const real orbit_ang_vel = 1.0; // 地球が太陽を周回する角速度
  static const real earth_ang_vel = 1.0; // 地球の自転の角速度
  static const real orbit_radius = 500.0; // 太陽から地球までの距離
  static real orbit_angle = 0;    // x-z平面上での極座標における角度
  static real earth_angle = 0;    // 地球の自転角度
  
  orbit_angle += orbit_ang_vel*delta;
  earth_angle += earth_ang_vel*delta;

  if(earth_angle >= 2*M_PI) earth_angle = 0.0;
  if(orbit_angle >= 2*M_PI) orbit_angle = 0.0;
  
  {//地球
    Transform transform;
    transform.pushRotateX(-M_PI/2);
    transform.pushRotateY(earth_angle);
    transform.pushTranslate(orbit_radius*cos(orbit_angle),0.0,orbit_radius*sin(orbit_angle));
    transform.pushTranslate(sun_light.v.x,sun_light.v.y,sun_light.v.z);
    meshes[0].setTransform(transform);    
  }
  
  {//太陽
    Transform transform;
    transform.pushRotateX(-M_PI/2);
    transform.pushTranslate(sun_light.v.x,sun_light.v.y,sun_light.v.z);
    meshes[1].setTransform(transform);     
  }
  
}

void InitBuffers(int w,int h){    
  // Frame Bufferを初期化    
  InitBuffer(w,h,Color(0,0,0),ctx);
  
  // ZBufferを初期化    
  InitZBuffer(w,h,ctx);
}

void InitCamera(int w,int h){
  Vector4 eye = Vector4(0,0,0,1);
  Vector4 gaze = Vector4(0,0,-1,0);
  Vector4 up = Vector4(0,1,0,0);

  real near = -0.1;
  real far = -10000;
  real fov = M_PI/2;
  real aspect = w/h;

  
  LookAt(eye,gaze,up,ctx);
  Viewport(w,h,ctx);
  Perspective(fov,aspect,near,far,ctx);

}

void InitLights(){
  addLight(sun_light,ctx);    
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
