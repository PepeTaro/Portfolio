/**
 * おそらく,もっともシンプルなデモ
 * z-buffer,光源,テクスチャ,Backface-Cullingなし
 *
 */

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

Strip triangle;

void InitObjects(){
  // 3頂点の色を定義
  Color r(1,0,0);
  Color g(0,1,0);
  Color b(0,0,1);

  // 3頂点の位置をHomogeneous coordinatesにおいて定義(点を表すので,w成分は1)
  Vector4 p1(-10,-10,0,1);
  Vector4 p2(10,-10,0,1);
  Vector4 p3(0,10,0,1);

  // 三角形の面に対する法線ベクトルを求める
  Vector4 n = normalize(cross(p2 - p1,p3 - p1));

  // 三角形の材質(Diffuse,Specular,Shininessをそれぞれ指定)
  Material material(Color(0.5,0.5,0.5),Color(0.5,0.5,0.5),1);

  // Stripクラスであるtriangleに頂点を追加(半時計回りに追加していることに注意)
  triangle.appendVertex(p1,r,n);
  triangle.appendVertex(p2,g,n);
  triangle.appendVertex(p3,b,n);

  // Backface-Cullingをオフにする(三角形なので,回転したら見えなくなるため)
  triangle.turnOffBackfaceCulling();

  // triangleに材質をセット
  triangle.setMaterial(material);  
}
 
void Update(real delta){
  static real angle = 0.0;
  
  if(delta >= 10)
    delta = 10;
   else if(delta < 0)
    delta = 0;
  
  if(angle >= 2*M_PI) angle = 0.0;  
  angle += delta;

  // Y軸にangle角度回転させて,(0,0,-30)並進移動させる変換クラス(Transform)を定義
  Transform transform; 
  transform.pushRotateY(angle);
  transform.pushTranslate(0,0,-30);

  // Stripクラスであるtrinagleに,その変換をアタッチ(つまり,これで三角形が上述のように変換される)
  triangle.setTransform(transform);    
}

void DrawObjects(){
  // Stripクラスであるtriangleを描写
  DrawStrip(triangle,ctx);
}

void InitBuffers(int w,int h){  
  // Frame Bufferを初期化
  // w,hはウィンドウのサイズ,Colorはバックグラウンドカラーに相当
  InitBuffer(w,h,Color(0.3,0.3,0.3),ctx);    
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
  
  Update(delta); // オブジェクトを更新(位置など)
  
  // バッファをクリア
  ClearBuffer(ctx);
  
  DrawObjects(); // オブジェクトを描写
  FlipBuffer(data,ctx); // Back-bufferとFront-bufferを切り替え
}

bool KeyboardPress(unsigned int keycode){return false;}
void MouseMove(){}
void MouseButton(){}
