#ifndef PIPELINE_H
#define PIPELINE_H

#include "vertex.h"
#include "vector4.h"
#include "matrix4.h"
#include "material.h"
#include "transform.h"
#include "light.h"
#include "shading.h"
#include "clip.h"
#include "camera.h"
#include "mesh.h"
#include "strip.h"
#include "context.h"
#include "type.h"

void GraphicsPipeline(Vertex v0,Vertex v1,Vertex v2,Vector4 n,const PolygonContext& p_ctx,Context& ctx);
void DrawMesh(const Mesh& mesh,Context& ctx);
void DrawStrip(const Strip& strip,Context& ctx);

int addLight(const Light& light,Context& ctx);
void freeLight(int id,Context& ctx);

void Viewport(int width,int height,Context& ctx);
void Perspective(real fov,real aspect,real near,real far,Context& ctx);
void LookAt(const Vector4& new_eye,const Vector4& new_gaze,const Vector4& new_up,Context& ctx);

#endif// PIPELINE_H
