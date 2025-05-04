#ifndef SHADING_H
#define SHADING_H

#include <vector>

#include "matrix4.h"
#include "vector4.h"
#include "vertex.h"
#include "color.h"
#include "light.h"
#include "material.h"
#include "utils.h"
#include "zbuffer.h"
#include "buffer.h"
#include "camera.h"
#include "pipeline.h"
#include "texture.h"
#include "context.h"
#include "type.h"

void Shading(const Vertex& q1,const Vertex& q2,const Vertex& q3,const PolygonContext& p_ctx,Context& ctx);
void PhongReflection(Vertex& p0,Vertex& p1,Vertex& p2,const PolygonContext& p_ctx,const Context& ctx);

#endif// SHADING_H
