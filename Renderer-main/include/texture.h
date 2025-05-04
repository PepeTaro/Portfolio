#ifndef TEXTURE_H
#define TEXTURE_H

#include <cassert>
#include <cstring>
#include <vector>
#include <array>
#include "vector4.h"
#include "color.h"
#include "type.h"

const int max_texture_width = 512;
const int max_texture_height = 512;
const int max_tex_size = max_texture_width*max_texture_height*3;//3は,RGBのチャンネル数

typedef std::array<real,max_tex_size> TextureImage;
extern std::vector<TextureImage> tex_images;

class Texture{  
public:
  int w,h,id;  
public:
  Texture();
  Texture(const TextureImage& tex_image,int width,int height);
  
  Color TextureLookup(real u,real v) const;
  static int AppendTextureIntoTable(const TextureImage& tex_image);
  
};//class Texture

#endif// TEXTURE_H
