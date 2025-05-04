#include "utils.h"

namespace convnet{
  
  bool isInteger(float x){
    if(floor(x) == x)
      return true;
    else
      return false;
  }

  bool isDivisible(int x,int y){
    if(x%y == 0)
      return true;
    else
      return false;
  }

};//namespace convnet
