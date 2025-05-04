/*
  [操作方法]
  WASD: 移動
  マウスクリック: ボールを発射
  スクリーン内でマウスを動かすことで視点操作
*/

#include "x11.h"
#include "utils.h"

#include "matrix4.h"
#include "vector4.h"
#include "mesh.h"
#include "strip.h"
#include "color.h"
#include "transform.h"
#include "material.h"
#include "pipeline.h"
#include "polygon.h"
#include "type.h"
#include "utils.h"

Context ctx;

static int width,height;

Material material(Color(0.5,0.5,0.5),Color(0.9,0.9,0.9),32);
Color color(1,0.2,0.2);

int cube_id,plane_id,light_id;

struct Surface{
  Vector4 pos,vel;
  real r;
  Mesh mesh;
  Surface(const Vector4& pos,const Vector4& vel,real radius):pos(pos),vel(vel),r(radius){
    mesh = GetSphereMesh(10,r,color,material);    
  }  
};

std::vector<Strip> strips;
std::vector<Mesh> meshes;
std::vector<Surface> bullets;
std::vector<Surface> targets;

Light light(Vector4(100,100,-150,1),Color(1,1,1),500);

void DrawObjects(){     
  for(auto mesh : meshes){
    DrawMesh(mesh,ctx);
  }
  
  for(auto bullet : bullets){
    DrawMesh(bullet.mesh,ctx);
  }
  for(auto target : targets){
    DrawMesh(target.mesh,ctx);
  }
}

int AppendMesh(std::vector<Mesh>& meshes,const Mesh& mesh){
  int id = meshes.size();
  meshes.push_back(mesh);
  return id;
}

int AppendSurface(std::vector<Surface>& surfaces,const Surface& surface){
  int id = surfaces.size();
  surfaces.push_back(surface);
  return id;
}

Vector4 RandomVector4(int low,int high){
  real x = (rand() % (high - low)) + low;
  real y = rand() % high;
  real z = (rand() % (high - low)) + low;
  return Vector4(x,y,z,1);
}

void InitObjects(){      
  Color r(1,0,0);    
  Color w(1,1,1);

  Material cube_material(Color(0.1,0.1,0.1),Color(0.7,0.7,0.7),8);  
  Material light_material(Color(1.0,1.0,1.0),Color(1.0,1.0,1.0),1);
  Material plane_material(Color(0.1,0.1,0.1),Color(0.1,0.1,0.1),1);
    
  Mesh plane_mesh = GetPlaneMesh(Vector4(-1,0,1,1),Vector4(1,0,1,1),Vector4(1,0,-1,1),Vector4(-1,0,-1,1),
				 Color(0.1,0.1,0.1),plane_material);
  {
    Transform transform;
    transform.pushScale(200,200,200);
    transform.pushTranslate(0,-100,0);
    plane_mesh.setDefaultTransform(transform);
  }
  
  plane_id = AppendMesh(meshes,plane_mesh);    
  light_id = AppendMesh(meshes,GetSphereMesh(30,5,w,light_material));    
  cube_id = AppendMesh(meshes,GetCubeMesh(-0.3,0.7,0,0,1,-1,r,cube_material));

  for(int i=0;i<10;++i){
    AppendSurface(targets,Surface(RandomVector4(-200,200),Vector4(0,0,0,0),10));
  }
}

// 手をカメラに固定するように変換
void UpdateArm(){  
  Transform transform;
  transform.pushTranslate(1,-2.0,-0.5);
  transform.pushRotateY(0.1);    
  transform.pushTransformMatrix(ctx.camera.inv_cam_mat,ctx.camera.cam_mat);
  meshes[cube_id].setTransform(transform);
}

// 銃弾を更新
void UpdateBullets(real delta){
  Vector4 d;
  Vector4 cam_pos = ctx.camera.eye;
  std::vector<int> idxs;
  
  for(int i=0;i<bullets.size();++i){    
    Transform transform;

    // Bulletの位置を更新    
    bullets[i].pos += -1000.0f*delta*bullets[i].vel;    
    transform.pushTranslate(bullets[i].pos.x,bullets[i].pos.y,bullets[i].pos.z);
    bullets[i].mesh.setTransform(transform);

    // 銃弾がカメラの位置から遠い場合,削除するために距離を計算
    d = bullets[i].pos - cam_pos;
    if(sqrtf(dot(d,d)) > 1000) idxs.push_back(i);
  }

  // 銃弾がカメラの位置から遠い場合,削除
  for(int i=0;i<idxs.size();++i){
    bullets.erase(bullets.begin()+idxs[i]);
  }
  
}

void UpdateTargets(real delta){
  Vector4 d;
  std::vector<int> idxs;
  static real angle = 0.01;  
  static Matrix4 rot_mat = Matrix4::RotateY(angle);
  
  for(int i=0;i<targets.size();++i){
    Transform transform;
    
    targets[i].pos = rot_mat*targets[i].pos;    
    transform.pushTranslate(targets[i].pos.x,targets[i].pos.y,targets[i].pos.z);
    targets[i].mesh.setTransform(transform);


    // 銃弾が当たった場合削除するための計算
    for(int j=0;j<bullets.size();++j){        
      d = bullets[j].pos - targets[i].pos;
      if(sqrtf(dot(d,d)) <= bullets[j].r + targets[i].r){
	idxs.push_back(i);
      }
    }    
  }

  // 銃弾が当たった場合削除
  for(int i=0;i<idxs.size();++i){
    targets.erase(targets.begin()+idxs[i]);
  }
  
}
 
void Update(real delta){
  static real angle = 0.0;

  if(delta <= 0.01) delta = 0.01;
  else if(delta >= 10) delta = 10;
  
  if(angle >= 2*M_PI) angle = 0.0;
  
  angle += delta;
  
  {
    Transform transform;  
    transform.pushTranslate(light.v.x,light.v.y,light.v.z);
    meshes[light_id].setTransform(transform);
  }
  
  UpdateArm();
  UpdateBullets(delta);
  UpdateTargets(delta);
}

void InitBuffers(int w,int h){
  // Frame Bufferを初期化    
  InitBuffer(w,h,Color(0.0,0.0,0.0),ctx);
  
  // ZBufferを初期化    
  InitZBuffer(w,h,ctx);
}

void InitCamera(int w,int h){
  Vector4 eye = Vector4(0,0,100,1);
  Vector4 gaze = Vector4(0,0,-1,0);
  Vector4 up = Vector4(0,1,0,0);

  real near = -0.01;
  real far = -500;
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
  width = w; height = h;
  
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
  //PrintFPS(delta);
  
  Update(delta); // オブジェクトを更新(位置など)

  // バッファをクリア
  ClearBuffer(ctx);
  ClearZBuffer(ctx);
  
  DrawObjects(); // オブジェクトを描写
  FlipBuffer(data,ctx); // Back-bufferとFront-bufferを切り替え  
}

void TranslateEye(const Vector4& delta){
  static int step = 3;    
  static Matrix4 mat;
  static Vector4 step_vec;

  step_vec = step*normalize(ctx.camera.inv_cam_mat*delta);
  mat = Matrix4::Translate(step_vec.x,step_vec.y,step_vec.z);  
  ctx.camera.LookAt(mat*ctx.camera.eye,ctx.camera.gaze,ctx.camera.up);
}

bool KeyboardPress(unsigned int keycode){  
  switch(keycode){
  case 0x18:
    return false;
  case 0x19:
    TranslateEye(Vector4(0,0,-1,0));
    break;
  case 0x27:
    TranslateEye(Vector4(0,0,1,0));        
    break;
  case 0x26:
    TranslateEye(Vector4(-1,0,0,0));    
    break;
  case 0x28:
    TranslateEye(Vector4(1,0,0,0));    
    break;
  case 0x6f:
    TranslateEye(Vector4(0,1,0,0));    
    break;
  case 0x74:
    TranslateEye(Vector4(0,-1,0,0));
    break;
  }

  return true;
}

void MouseButton(){ 
  static Transform default_transform;
  static real radius = 4;
  Vector4 cam_pos = ctx.camera.eye;
  Vector4 cam_gaze = ctx.camera.gaze;

  // 銃弾を発射
  AppendSurface(bullets,Surface(ctx.camera.eye,ctx.camera.w,radius));
}

void MouseMove(){
  static const real angle_sensitivity = 1e-1;
  static const real atan_sensitivity = 1.0f/2000.0;
  static const real max_angle = 0.95;

  static int x,y;    
  static real test_val;
  static real thetax,thetay;
  static Matrix4 rot,cam_mat,inv_cam_mat;
  static real w2 = width/2.0;
  static real h2 = height/2.0;
  
  GetMouseCoordXY(x,y);
  
  if(x >= 0 and x < width and y >= 0 and y < height){
	
    test_val = dot(ctx.camera.gaze,ctx.camera.up);    
    thetax = atan((h2 - y)*atan_sensitivity);
    thetay = atan((w2 - x)*atan_sensitivity);

    // 下或いは上を向きすぎると,gazeとupがほぼ線形従属状態となり,カメラがおかしくなるため  
    if((test_val >= max_angle and thetax >= 0) or (test_val <= -max_angle and thetax <= 0)){ 
	  
    }else{
      thetax *= angle_sensitivity; thetay *= angle_sensitivity;	  	  
      rot = ctx.camera.inv_cam_mat*Matrix4::RotateY(thetay)*Matrix4::RotateX(thetax)*ctx.camera.cam_mat;
      
      ctx.camera.LookAt(ctx.camera.eye,rot*ctx.camera.gaze,ctx.camera.up);
    }
  }	
}
