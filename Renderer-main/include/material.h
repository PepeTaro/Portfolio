#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"

class Material{
public:
  Color diffuse;
  Color specular;
  real shininess;
public:
  Material();
  Material(const Color& diffuse,const Color& specular,real shininess);
  
};//class Material

#endif// MATERIAL_H
