#include "utils.h"

void PrintFPS(real delta){
  static real fps = 0;
  fps = 1.0f/delta;
  printf("FPS:%f\n",fps);
}

real GetDeltaTime(){  
  static struct timeval tp;
    
  static double current=0;
  static double last=0;
  static real delta=1;

  gettimeofday(&tp,NULL);
  
  current = tp.tv_sec * 1000 + tp.tv_usec / 1000; // currentはミリ秒単位
  delta = (current - last)/1000; // deltaは秒単位
  
  last = current;
  
  return delta;
}
