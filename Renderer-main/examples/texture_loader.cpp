#include "texture_loader.h"

// テクスチャ画像を読み込む関数
Texture LoadTexture(const char* file_path){    
  TextureImage tex_array;
  CImg<unsigned char> src(file_path);
  
  int tex_width = src.width();
  int tex_height = src.height();  

  for(int i=0;i<tex_width;++i){
    for(int j=0;j<tex_height;++j){      
      int k = 3*(i + j*tex_width);
      int flip_j = tex_height - j; // 単純にjを使用してインデックスを指定すると上下反転する(フレームバッファは上から下に走るため)
      tex_array[k + 0] = src(i,flip_j,0,0)/255.0f;
      tex_array[k + 1] = src(i,flip_j,0,1)/255.0f;
      tex_array[k + 2] = src(i,flip_j,0,2)/255.0f;
      
    }
  }

  return Texture(tex_array,tex_width,tex_height);
}
