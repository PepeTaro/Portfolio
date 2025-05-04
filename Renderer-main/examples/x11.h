#ifndef X11_H
#define X11_H

#include <iostream>

#include <X11/Xlib.h>           
#include <X11/Xutil.h>          
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "type.h"

void DrawMain(char* data);
void Init(int w,int h);
void Free();

bool KeyboardPress(unsigned int keycode);
void MouseMove();
void MouseButton();
void GetMouseCoordXY(int& x,int& y);

#endif// X11_H
