#include "material.h"

Material::Material(){

}

Material::Material(const Color& diffuse,const Color& specular,real shininess):diffuse(diffuse),specular(specular),shininess(shininess){

}
