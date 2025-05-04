#ifndef UTILS_H
#define UTILS_H

#include "type.h"

inline real max(real a,real b){
  if(a > b) return a;
  else return b;
}

inline real min(real a,real b){
  if(a > b) return b;
  else return a;
}

inline real clamp(real a){
  if(a > 1.0) return 1.0;
  else return a;
}

inline void GetMinMax(real& min,real& max,real value0,real value1,real value2){
  if(value0 > value1){
    if(value1 > value2){
      min = value2;
      max = value0;      
    }else{
      if(value0 > value2){
	min = value1;
	max = value0;	
      }else{
	min = value1;
	max = value2;	
      }
    }    
  }else{ // value0 <= value1
    if(value1 < value2){
      min = value0;
      max = value2;      
    }else{ // value1 >= value2
      if(value0 > value2){
	min = value2;
	max = value1;
      }else{
	min = value0;
	max = value1;
      }
    }
  }
}

#endif// UTILS_H
