#include "buffer.h"

void PutColorInBuffer(char* buffer,int index,const Color& color){
  buffer[index] = color.b*255;   // blue
  buffer[index+1] = color.g*255; // green
  buffer[index+2] = color.r*255; // red
}

void PutPixel(int x,int y,const Color& color,Context& ctx){      
  int i = ctx.buffer_size + 4*(x - y*ctx.buffer_width);// スクリーンの左下が原点となるように変換
  //int i = 4*(x + y*ctx.buffer_width); // スクリーンの左上が原点となるように変換

  PutColorInBuffer(ctx.back_buffer,i,color);
}

void FlipBuffer(void* frame_buffer,Context& ctx){    
  memcpy(frame_buffer,ctx.back_buffer,ctx.buffer_size);
}

void ClearBuffer(Context& ctx){
  memcpy(ctx.back_buffer,ctx.clear_buffer,ctx.buffer_size);
}

void InitBuffer(int width,int height,const Color& color,Context& ctx){
  mallocBuffer(width,height,ctx);
  InitClearBuffer(color,ctx);
}

void mallocBuffer(int width,int height,Context& ctx){
  ctx.buffer_width = width;
  ctx.buffer_height = height;
  ctx.buffer_size = 4*width*height; // 4倍しているのはRGBAのチャンネル数分(アルファチャンネルは使用しない)
  
  ctx.back_buffer = new char[ctx.buffer_size];
  ctx.clear_buffer = new char[ctx.buffer_size];
}

void FreeBuffer(Context& ctx){
  delete[] ctx.back_buffer;
  delete[] ctx.clear_buffer;
}

void InitClearBuffer(const Color& color,Context& ctx){
  for(int i=0;i<ctx.buffer_size;i += 4){
    PutColorInBuffer(ctx.clear_buffer,i,color);        
  }
}
