#ifndef CONTEXT_H
#define CONTEXT_H

#include "texture.h"
#include "material.h"
#include "transform.h"
#include "camera.h"
#include "light.h"

typedef struct PolygonContext{  
  Texture texture; // テクスチャの情報を保持
  bool texture_enable=false;

  Material material;  // オブジェクトの材質
  bool material_enable=false;

  Transform transform; // メッシュに対する時間依存変換(例えば,弾丸が移動する場合など)
  Transform default_transform;  // メッシュに対する時間非依存変換(つまり,Model変換)
 
  bool backface_culling_enable=true;
  
} PolygonContext;

const int kMaxLights = 16; // 光源数の最大値

typedef struct Context{
  //int window_width;
  //int window_height;
  
  Camera camera; 
  Light lights[kMaxLights];    // 光源を保存
  bool usedLights[kMaxLights]; // インデックスが与えられたとき,その光源が使用されているか否か表す配列(使用されているならTrue)
  
  char* back_buffer;
  char* clear_buffer; // フレーム毎に,まずこのバッファでback_bufferを上書き(つまり,バックグランドカラー)
  int buffer_size;
  int buffer_height;
  int buffer_width;
  
  real* zbuffer;
  real* clear_zbuffer; // デフォルトの値である-1で初期化。フレームごとにzbufferをclear_zbufferで上書きする。
  int zbuffer_size;
  int zbuffer_height;
  int zbuffer_width;
  bool use_depth_test = false; // デフォルトでDepth Testを使用しない
  
  Context(){
    for(int i=0;i<kMaxLights;++i){
      usedLights[i] = false;
    }
  }

} Context;

#endif//CONTEXT_H
