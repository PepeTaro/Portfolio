#include "pipeline.h"

void GraphicsPipeline(Vertex v0,Vertex v1,Vertex v2,Vector4 n,const PolygonContext& p_ctx,Context& ctx){
  // 以下の一連の関数において,v0,v1,v2,nの内容が段々と変更されていく。
  
  // Model変換。v0,v1,v2,nの位置が変換される
  p_ctx.default_transform.ModelTransform(v0,v1,v2,n); // デフォルト変換(つまり,時間に依存しない変換,例えば位置など)
  p_ctx.transform.ModelTransform(v0,v1,v2,n); // 時間に依存する変換(例えば,動くオブジェクトなど)
  
  // Backface Culling
  if(p_ctx.backface_culling_enable and BackfaceCulling(v0.v,n,ctx)) return;

  // Phong Reflection Modelを使用して,ライティングを計算,結果はv0,v1,v2の色(v0.c,v1.c,v2.c)に保存
  if(p_ctx.material_enable){
    PhongReflection(v0,v1,v2,p_ctx,ctx);
  }
  
  // Camera-Projection Transformation
  ctx.camera.CameraProjectionTransform(v0,v1,v2);  
  
  // Clipping(Clipping関数内において,ピクセル描写を行う)  
  Clipping(v0,v1,v2,0,p_ctx,ctx);
}

void DrawMesh(const Mesh& mesh,Context& ctx){
  int num_triangles = mesh.getNumTriangles(); // いくつ三角形を描写するか判断するために必要
  Vertex v0,v1,v2; // 三角形の3つの頂点を保存用
  Vector4 n;      // 三角形の面に対する法線ベクトルを保存
     
  for(int i=0;i<num_triangles;++i){
    mesh.getTriangle(i,v0,v1,v2,n); // v0,v1,v2,nにそれぞれ頂点0,頂点1,頂点2,法線ベクトルを保存        
    GraphicsPipeline(v0,v1,v2,n,mesh.p_ctx,ctx); // パイプラインに"流し込む"
  }
}

void DrawStrip(const Strip& strip,Context& ctx){
  int num_verts = strip.getNumVertices(); // 頂点数を取得
  Vertex v0,v1,v2; // 三角形の3つの頂点を保存用
  Vector4 n;      // 三角形の面に対する法線ベクトルを保存
  
  for(int i=0;i<(num_verts-2);++i){ // 2を引いているのは,"最後"の三角形を成す頂点のインデックスは,num_verts-3,num_verts-2,num_verts-1のため
    strip.getTriangle(i,v0,v1,v2,n); // v0,v1,v2,nにそれぞれ頂点0,頂点1,頂点2,法線ベクトルを保存
    GraphicsPipeline(v0,v1,v2,n,strip.p_ctx,ctx); // パイプラインに"流し込む"
  }
}

int addLight(const Light& light,Context& ctx){
  int i,id;

  // 空いているスロットを探す
  for(i=0;i < kMaxLights;++i){
    if(not ctx.usedLights[i]) break;
  }

  // スロットが満杯の場合
  if(i == kMaxLights){
    std::cerr << "Error(addLight): これ以上光源を追加できません" << std::endl;
    exit(-1);
  }

  // スロットが見つかった場合,追加しidを返す
  id = i;  
  ctx.lights[id] = light;
  ctx.usedLights[id] = true;
  
  return id;
}

void freeLight(int id,Context& ctx){    
  ctx.usedLights[id] = false;    
}

void Viewport(int width,int height,Context& ctx){
  ctx.camera.Viewport(width,height);
}

void Perspective(real fov,real aspect,real near,real far,Context& ctx){
  ctx.camera.Perspective(fov,aspect,near,far);
}

void LookAt(const Vector4& new_eye,const Vector4& new_gaze,const Vector4& new_up,Context& ctx){
  ctx.camera.LookAt(new_eye,new_gaze,new_up);
}
