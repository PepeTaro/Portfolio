#ifndef BUFFER_H
#define BUFFER_H

#include <cstring>
#include "color.h"
#include "context.h"
#include "type.h"

void PutColorInBuffer(char* buffer,int index,const Color& color);
void PutPixel(int x,int y,const Color& color,Context& ctx);
void FlipBuffer(void* frame_buffer,Context& ctx);
void ClearBuffer(Context& ctx);
void InitBuffer(int width,int height,const Color& color,Context& ctx);
void mallocBuffer(int width,int height,Context& ctx);
void FreeBuffer(Context& ctx);
void InitClearBuffer(const Color& color,Context& ctx);

#endif// BUFFER_H
