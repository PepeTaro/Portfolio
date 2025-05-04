#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <sys/time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>

#include <X11/Xlib.h>           
#include <X11/Xutil.h>          
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "matrix4.h"
#include "vector4.h"
#include "mesh.h"
#include "strip.h"
#include "color.h"
#include "transform.h"
#include "material.h"
#include "pipeline.h"

#include "type.h"
#include "utils.h"

const int width  = 640;
const int height = 480;

Display *display;
XImage *image;
XShmSegmentInfo shminfo;
GC gc;
Window win;

Material material(Color(0.5,0.5,0.5),Color(0.5,0.5,0.5),8);
Color color(0.2,0.2,0.2);

Color r(1,0,0);
Color g(0,1,0);
Color b(0,0,1);
Color y(1,1,0);
Color p(1,0,1);
Color s(0,1,1);
Color d(0,0,0);
Color w(1,1,1);

struct Surface{
  Vector4 pos,vel;
  real r;
  Mesh mesh;
  Surface(const Vector4& pos,const Vector4& vel,real r):pos(pos),vel(vel),r(r){
    Transform default_transform;
    mesh = Mesh::GetSphereMesh(10,r,color,material);    
    default_transform.pushTranslate(pos.x,pos.y,pos.z);  
    mesh.setDefaultTransform(default_transform);
  }  
};

std::vector<Strip> strips;
std::vector<Mesh> meshes;
std::vector<Surface> bullets;
std::vector<Surface> targets;

Vector4 light_pos(100,100,-150,1);

void DrawGeometric(){    
  for(auto strip : strips){
    DrawStrip(strip);
  }  
  for(auto mesh : meshes){
    DrawMesh(mesh);
  }
  for(auto bullet : bullets){
    DrawMesh(bullet.mesh);
  }
  for(auto target : targets){
    DrawMesh(target.mesh);
  }
}

int AppendStrip(std::vector<Strip>& strips,const Strip& strip){
  int id = strips.size();
  strips.push_back(strip);
  return id;
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
  real y = (rand() % (high - low)) + low;
  real z = (rand() % (high - low)) + low;
  return Vector4(x,y,z,1);
}

int sphere,cube,plane,triangle,light;
void InitGeometric(){    
  
  Material cube_material(Color(0.1,0.1,0.1),Color(0.7,0.7,0.7),8);
  Material triangle_material(Color(0.9,0.9,0.9),Color(0.9,0.9,0.9),8);
  Material plane_material(Color(0.1,0.1,0.1),Color(0.1,0.1,0.1),1);
  Material sphere_material(Color(0.5,0.5,0.5),Color(0.5,0.5,0.5),8);
  Material light_material(Color(1.0,1.0,1.0),Color(1.0,1.0,1.0),1);
  
  plane = AppendMesh(meshes,
		     Mesh::GetPlaneMesh(Vector4(-100,-60,-10,1),Vector4(100,-60,-10,1),Vector4(100,-60,-140,1),Vector4(-100,-60,-140,1),r,triangle_material));
  
  light = AppendMesh(meshes,
		     Mesh::GetSphereMesh(30,5,w,light_material));
  
  //sphere = AppendMesh(meshes,Mesh::GetSphereMesh(30,5,d,sphere_material));
  /*
  Mesh t = Mesh::GetTriangleMesh(Vector4(-50,-50,-100,1),Vector4(50,-50,-100,1),Vector4(0,80,-100,1),r,g,b,triangle_material);
  t.turnOffBackfaceCulling();
  triangle = AppendMesh(meshes,t);
  */

  /*
  Strip t = Strip::GetTriangleStrip(Vector4(-50,-50,-100,1),Vector4(50,-50,-100,1),Vector4(0,80,-100,1),r,g,b,triangle_material);
  t.turnOffBackfaceCulling();
  triangle = AppendStrip(strips,t); 
  */

  //cube = AppendMesh(Mesh::GetCubeMesh(-50,-50,-130,50,50,-150,p,cube_material));

  cube = AppendMesh(meshes,
		    Mesh::GetCubeMesh(-0.3,0.7,0,0,1,-1,r,cube_material));

  for(int i=0;i<10;++i){
    AppendSurface(targets,Surface(RandomVector4(-50,50),Vector4(0,0,0,0),5));
  }
}
 
void UpdateArm(){
  Transform transform;
  transform.pushTranslate(1,-2.0,-0.5);
  transform.pushRotateY(0.1);    
  transform.pushCameraFixTransform(camera);        
  meshes[cube].setTransform(transform);
}

void UpdateBullets(real delta){
  Vector4 d;
  Vector4 cam_pos = getCameraPos();
  std::vector<int> idxs;
  
  for(int i=0;i<bullets.size();++i){    
    Transform transform;
    Vector4 vel = bullets[i].vel;
    bullets[i].pos += vel*(0.7)*delta;
    transform.pushTranslate(bullets[i].pos.x,bullets[i].pos.y,bullets[i].pos.z);
    bullets[i].mesh.setTransform(transform);
    
    d = bullets[i].pos - cam_pos;
    if(sqrtf(dot(d,d)) > 1000) idxs.push_back(i);
  }

  for(int i=0;i<idxs.size();++i){// 遠い場合削除
    bullets.erase(bullets.begin()+i);
  }  
}

void UpdateTargets(real angle){
  Vector4 d;
  Transform transform;
  std::vector<int> idxs;
  
  for(int i=0;i<targets.size();++i){
    transform.pushTranslate(targets[i].pos.x,targets[i].pos.y,targets[i].pos.z);
    transform.pushRotateY(angle);
    targets[i].mesh.setTransform(transform);


    for(int j=0;j<bullets.size();++j){        
      d = bullets[j].pos - targets[i].pos;
      if(sqrtf(dot(d,d)) <= bullets[j].r + targets[i].r){
	idxs.push_back(i);
	std::cout << "Hit !!" << std::endl;
      }
    }
    
  }
  
  for(int i=0;i<idxs.size();++i){// 当たった場合削除
    targets.erase(targets.begin()+i);
  }
  
}
 
void Update(real delta){
  static real angle = 0.0;
  
  if(delta >= 10) delta = 10;
  if(angle >= 2*M_PI) angle = 0.0;
  
  angle += 0.001*delta;
  /*
  {
    Transform transform;  
    transform.pushRotateY(angle);
    transform.pushTranslate(0,0,-10);
    transform.pushRotateY(angle);
    meshes[sphere].setTransform(transform);
  }
  */
  /*
  {
    Transform transform;  
    transform.pushTranslate(0,0,100);
    transform.pushRotateY(angle);
    transform.pushTranslate(0,0,-100);  
    strips[triangle].setTransform(transform);
  }
  */
  

  {
    Transform transform;  
    transform.pushTranslate(light_pos.x,light_pos.y,light_pos.z);
    meshes[light].setTransform(transform);
  }
  
  UpdateArm();
  UpdateBullets(delta);
  UpdateTargets(angle);
}

void Draw(){
  // バッファをクリア
  ClearBuffer();
  ClearZBuffer();
  
  DrawGeometric(); // オブジェクトを描写
  FlipBuffer(image->data); // Back-bufferとFront-bufferを切り替え
  
  XShmPutImage(display,win,gc,image,0,0,0,0,width,height,True);  
  XFlush(display);
}

void InitBuffers(){
  // Back Bufferを初期化
  AllocateBuffer(width,height);
  InitClearBuffer(Color(0.3,0.3,0.3));

  // ZBufferを初期化
  AllocateZBuffer(width,height);
  InitZBuffer();  
}

void FreeBuffers(){  
  FreeBuffer();
  FreeZBuffer();
}

void InitCamera(){
  Vector4 eye = Vector4(0,0,0,1);
  Vector4 gaze = Vector4(0,0,-1,0);
  Vector4 up = Vector4(0,1,0,0);

  real near = -0.1;
  real far = -1000;
  real fov = M_PI/2;
  
  SetCamera(eye,gaze,up,fov,near,far,width,height);  
}

void InitLights(){
  AppendLight(light_pos,Color(1,1,1));    
}

void InitAll(){
  InitCamera();
  InitBuffers();
  InitGeometric();
  InitLights();
}

void FreeAll(){
  FreeBuffers();
}

void Idle(real delta){
  Update(delta);
  Draw();
}

void abort(const std::string& message){
  std::cerr << "[!]Error:" << message << std::endl;
  exit(-1);
}

void InitX(){
  //https://www.x.org/releases/X11R7.7/doc/xextproto/shm.html 参照
  
  int screen;
  int major,minor;
  Bool pixmaps;
  XVisualInfo vinfo;  
  XWindowAttributes attributes;
  Status error;
  
  display = XOpenDisplay(NULL);
  if(display == NULL) abort("ディスプレイを開けませんでした");    
  
  screen = DefaultScreen(display);    
  win = XCreateSimpleWindow(display,RootWindow(display,screen),
			    0,0,
			    width,height,1,0,0);
    
  gc = XCreateGC(display,win,0,0);    
  error = XShmQueryVersion(display, &major, &minor, &pixmaps);
  if(not error) abort("Xshmがサポートされていません");
  
  XMatchVisualInfo(display,
		   screen,
		   24,
		   TrueColor,
		   &vinfo);

  XGetWindowAttributes(display,
		       DefaultRootWindow(display),
		       &attributes);
    
  image = XShmCreateImage(display,
			  vinfo.visual,24, ZPixmap, NULL,
			  &shminfo,
			  width,height);
    
  shminfo.shmid = shmget(IPC_PRIVATE,
			 image->bytes_per_line * image->height,
			 IPC_CREAT | 0777);
    
  shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid,0,0);
  shminfo.readOnly = False;
          
  error = XShmAttach(display, &shminfo);
  if(error == 0) abort("共有メモリセグメントにアタッチできませんでした");

  XSelectInput(display,win,ExposureMask | ButtonPressMask | KeyPressMask);
  XMapWindow(display,win);  
}

void FreeX(){      
  XShmDetach(display,&shminfo);
  XDestroyImage(image);
  shmdt(shminfo.shmaddr);
  shmctl(shminfo.shmid, IPC_RMID, 0);
  XCloseDisplay(display);
}

real DisplayFPS(){  
  static struct timeval tp;
  static unsigned int current=0;
  static unsigned int last=0;
  static unsigned int delta=1;
  static real delta_time=0;

  gettimeofday(&tp, NULL);
  current = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  delta = (current - last);
  delta_time = 1.0/delta;
  printf("FPS:%f\n",1000.0*delta_time);
  last = current;
  
  return delta;
}

void TranslateEye(const Vector4& delta){
  static int step = 3;    
  static Matrix4 mat,inv_cam_mat;
  static Vector4 step_vec;

  inv_cam_mat = getInvCameraMatrix();
  step_vec = step*normalize(inv_cam_mat*delta);
  mat = Matrix4::Translate(step_vec.x,step_vec.y,step_vec.z);
  TransformCameraEye(mat);
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
  //printf( "KeyPress: %x\n", event.xkey.keycode );
}

void MouseButton(){ 
  static Transform default_transform;
  static real radius = 10;
  Vector4 cam_pos = getCameraPos();
  Vector4 cam_gaze = getCameraGaze();
  
  /*
  Mesh mesh = Mesh::GetSphereMesh(8,radius,Color(0,0,0),Material(Color(0.1,0.1,0.1),Color(0.1,0.1,0.1),1));	
  default_transform.clear();
  default_transform.pushTranslate(cam_pos.x,cam_pos.y,cam_pos.z);  
  mesh.setDefaultTransform(default_transform);
  AppendSurface(bullets,Surface(mesh,cam_pos,cam_gaze,radius));
  */
  
  AppendSurface(bullets,Surface(cam_pos,cam_gaze,radius));
}

void MouseMove(){
  static Window root_window;
  static int root_x,root_y;
  static int win_x,win_y;
  static unsigned int mask;
  static real dist = 1000.0;//farと同じ
  static real test_val;
  static real max_angle = 0.95;
  static real thetax,thetay;
  static Matrix4 rot,cam_mat,inv_cam_mat;
  static Vector4 gaze,up;

  XQueryPointer(display,win,&root_window,&root_window,&root_x,&root_y,&win_x,&win_y,&mask);
  if(win_x >= 0 and win_x < width and win_y >= 0 and win_y < height){

    gaze = getCameraGaze();
    up = getCameraUp();
	
    test_val = dot(gaze,up);
    thetax = atan((height/2.0 - win_y)/dist);
    thetay = atan((width/2.0 - win_x)/dist);

    if(test_val >= max_angle and thetax >= 0){ // 下或いは上を向きすぎると,gazeとupがほぼ線形従属状態となり,カメラがおかしくなるため	  

    }else if(test_val <= -max_angle and thetax <= 0){
	  
    }else{
      thetax *= 0.1; thetay *= 0.1;
	  
      inv_cam_mat = getInvCameraMatrix();
      cam_mat = getCameraMatrix();
	  	  
      rot = inv_cam_mat*Matrix4::RotateY(thetay)*Matrix4::RotateX(thetax)*cam_mat;
	  
      TransformCameraGaze(rot);
    }
  }	
}

void Run(){
  XEvent event;
  KeySym key;
  real delta;
  
  while(1){
    if (XPending(display) > 0) {
      XNextEvent(display,&event);
    
      if(event.type == KeyPress){
	if(not KeyboardPress(event.xkey.keycode)) return;
      }
      
      if(event.type == ButtonPress){
        MouseButton();	
      }

      MouseMove();          
      
    }else{      
      delta = DisplayFPS();
      Idle(delta);
    }
  }  
}

int main(int argc, char *argv[]){
  InitX();
  InitAll();
  Run();
  FreeAll();  
  FreeX();
  
  return 0;
}
