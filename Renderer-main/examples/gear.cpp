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

std::vector<std::vector<Strip>> gears;
Light light(Vector4(100,100,-150,1),Color(1,1,1),300);

/**
 *  以下のGetGearStrip関数は,TinyGLのコードを参考として使用
 *  https://github.com/ska80/tinygl/blob/master/examples/gears.c
 */
void GetGearStrip(std::vector<Strip>& gear,
		  real inner_radius,real outer_radius,real gear_width,int teeth,real tooth_depth,
		  const Color& c,const Material& material)
{
  Strip front_face,back_face,cylinder,front_teeth,back_teeth,outward;
  real angle,u,v,len;
  int i;
  Vector4 n;
  
  real r0 = inner_radius;
  real r1 = outer_radius - tooth_depth/2.0;
  real r2 = outer_radius + tooth_depth/2.0;
  real da = 2.0*M_PI / teeth / 4.0;
  
  // 歯車の手前側を描写
  n = Vector4(0,0,1,0);
  for(i=0;i<=teeth;++i){
    angle = i * 2.0*M_PI / teeth;
    front_face.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),gear_width*0.5,1),c,n);
    front_face.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),gear_width*0.5,1),c,n);
    front_face.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),gear_width*0.5,1),c,n);
    front_face.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),gear_width*0.5,1),c,n);    
  }  
  front_face.setMaterial(material);
  gear.push_back(front_face);

  // 歯の手前側を描写
  da = 2.0*M_PI / teeth / 4.0;
  for(i=0;i<=teeth;i++){
    angle = i * 2.0*M_PI / teeth;        
    front_teeth.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),gear_width*0.5,1),c,n);
    front_teeth.appendVertex(Vector4(r2*cos(angle+2*da),r2*sin(angle+2*da),gear_width*0.5,1),c,n);
    front_teeth.appendVertex(Vector4(r2*cos(angle+da),r2*sin(angle+da),gear_width*0.5,1),c,n);
    front_teeth.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),gear_width*0.5,1),c,n);

  }
  front_teeth.setMaterial(material);
  gear.push_back(front_teeth);

  
  // 歯車の後ろ側を描写
  n = Vector4(0,0,-1,0);    
  for(i=0;i<=teeth;i++){
    angle = i * 2.0*M_PI / teeth;
    back_face.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),-gear_width*0.5,1),c,n);
    back_face.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),-gear_width*0.5,1),c,n);
    back_face.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),-gear_width*0.5,1),c,n);    
    back_face.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),-gear_width*0.5,1),c,n);
  }
  back_face.setMaterial(material);
  gear.push_back(back_face);
  
  // 歯の後ろ側を描写
  da = 2.0*M_PI / teeth / 4.0;
  for(i=0;i<=teeth;i++){
    angle = i * 2.0*M_PI / teeth;
    back_teeth.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),-gear_width*0.5,1),c,n);
    back_teeth.appendVertex(Vector4(r2*cos(angle+2*da),r2*sin(angle+2*da),-gear_width*0.5,1),c,n);
    back_teeth.appendVertex(Vector4(r2*cos(angle+da),r2*sin(angle+da),-gear_width*0.5,1),c,n);
    back_teeth.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),-gear_width*0.5,1),c,n);    
  }
  back_teeth.setMaterial(material);
  gear.push_back(back_teeth);

  // 歯の表面を描写
  for(i=0;i<=teeth;i++){
      angle = i * 2.0*M_PI / teeth;
      
      outward.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),gear_width*0.5,1),c,n);
      outward.appendVertex(Vector4(r1*cos(angle),r1*sin(angle),-gear_width*0.5,1),c,n);      

      u = r2*cos(angle+da) - r1*cos(angle);
      v = r2*sin(angle+da) - r1*sin(angle);
      len = sqrt( u*u + v*v );
      u /= len;
      v /= len;

      n = Vector4(v,-u,0,0);    
      outward.appendVertex(Vector4(r2*cos(angle+da),r2*sin(angle+da),gear_width*0.5,1),c,n);
      outward.appendVertex(Vector4(r2*cos(angle+da),r2*sin(angle+da),-gear_width*0.5,1),c,n);

      n = Vector4(cos(angle),sin(angle),0,0);    
      outward.appendVertex(Vector4(r2*cos(angle+2*da),r2*sin(angle+2*da),gear_width*0.5,1),c,n);
      outward.appendVertex(Vector4(r2*cos(angle+2*da),r2*sin(angle+2*da),-gear_width*0.5,1),c,n);

      u = r1*cos(angle+3*da) - r2*cos(angle+2*da);
      v = r1*sin(angle+3*da) - r2*sin(angle+2*da);

      n = Vector4(v,-u,0,0);    
      outward.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),gear_width*0.5,1),c,n);
      outward.appendVertex(Vector4(r1*cos(angle+3*da),r1*sin(angle+3*da),-gear_width*0.5,1),c,n);
      n = Vector4(cos(angle),sin(angle),0,0);    
   }
  outward.appendVertex(Vector4(r1*cos(0),r1*sin(0),gear_width*0.5,1),c,n);
  outward.appendVertex(Vector4(r1*cos(0),r1*sin(0),-gear_width*0.5,1),c,n);
  outward.setMaterial(material);
  gear.push_back(outward);

  // 歯車の筒状の部分を描写
  for(i=0;i<=teeth;i++){
    angle = i * 2.0*M_PI / teeth;
    n = Vector4(-cos(angle),-sin(angle),0,0);    
    cylinder.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),-gear_width*0.5,1),c,n);
    cylinder.appendVertex(Vector4(r0*cos(angle),r0*sin(angle),gear_width*0.5,1),c,n);
  }  
  cylinder.setMaterial(material);
  gear.push_back(cylinder);
  
}

void DrawObjects(){
  for(auto gear : gears){
    for(auto part : gear){
      DrawStrip(part,ctx);
    }
  }  
}

void InitObjects(){  
  Color r(1,0,0);
  Color g(0,1,0);
  Color b(0,0,1);

  std::vector<Strip> gear1,gear2,gear3;
  
  Material material(Color(0.5,0.5,0.5),Color(0.8,0.8,0.8),8);    
  GetGearStrip(gear1,1,4,1,20,0.7,r,material);
  GetGearStrip(gear2,0.5,2.0,2.0,10,0.7,g,material);
  GetGearStrip(gear3,0.5,2.0,2.0,10,0.7,b,material);
  
  gears.push_back(gear1);
  gears.push_back(gear2);
  gears.push_back(gear3);
}
 
void Update(real delta){
  static real angle = 0.0;
  
  if(delta >= 10)
    delta = 10;
  else if(delta < 0)
    delta = 0;
  
  if(angle >= 2*M_PI) angle = 0.0;
  
  angle += delta;
  
  {
    Transform transform;            
    transform.pushRotateZ(angle);
    transform.pushTranslate(-3.0,-2.0,-15.0);

    for(int j=0;j<gears[0].size();++j){            
      gears[0][j].setTransform(transform);
    }
  }

  {
    Transform transform;      
    transform.pushRotateZ(-2.0*angle-9.0);      
    transform.pushTranslate(3.1,-2.0,-15.0);

    for(int j=0;j<gears[1].size();++j){            
      gears[1][j].setTransform(transform);
    }
  }

  {
    Transform transform;      
    transform.pushRotateZ(-2.0*angle-25.0);
    transform.pushTranslate(-3.1,4.2,-15.0);

    for(int j=0;j<gears[2].size();++j){            
      gears[2][j].setTransform(transform);
    }       
  }
  
}

void InitBuffers(int w,int h){  
  // Frame Bufferを初期化    
  InitBuffer(w,h,Color(0.3,0.3,0.3),ctx);
  
  // ZBufferを初期化    
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
  
  LookAt(eye,gaze,up,ctx);
  Viewport(w,h,ctx);
  Perspective(fov,aspect,near,far,ctx);
    
}

void InitLights(){
  addLight(light,ctx);    
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
  PrintFPS(delta);
  
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
