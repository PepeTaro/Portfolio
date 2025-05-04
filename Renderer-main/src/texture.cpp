#include "texture.h"

std::vector<TextureImage> tex_images;

Texture::Texture(){}

Texture::Texture(const TextureImage& tex_image,int width,int height){
  assert(width >= 1 and width <= max_texture_width);
  assert(height >= 1 and height <= max_texture_height);

  w = width; h = height;
  id = Texture::AppendTextureIntoTable(tex_image);
}

Color Texture::TextureLookup(real u,real v) const{
  /*
  // 数値誤差の影響で極稀にu,vの値が0になるため,導入
  if(u < 0) u = 0;
  if(v < 0) v = 0;
  */
  
  int i = (int)ceil(u*w - 0.5) % w;
  int j = (int)ceil(v*h - 0.5) % h;
  int k = 3*(i + j*w);
  
    
  return Color(tex_images[id][k],tex_images[id][k+1],tex_images[id][k+2]);  
}

int Texture::AppendTextureIntoTable(const TextureImage& tex_image){
  tex_images.push_back(tex_image);
  return tex_images.size() - 1;
}
