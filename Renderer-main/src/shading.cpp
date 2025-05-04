#include "shading.h"

void Shading(const Vertex& q1,const Vertex& q2,const Vertex& q3,const PolygonContext& p_ctx,Context& ctx){
  real min_x,max_x,min_y,max_y;  
  real alpha,beta,gamma,det_denom;
  real p3x_p1x,p3y_p1y,p2x_p1x,p2y_p1y,x_p1x,y_p1y;
  Color color;

  real u_s,v_s,denom,u,v,q1_denom,q2_denom,q3_denom,alpha_q1_denom,beta_q2_denom,gamma_q3_denom; // For texture
  q1_denom = 1.0/q1.v.w; q2_denom = 1.0/q2.v.w; q3_denom = 1.0/q3.v.w;
  
  // Perspective division
  Vector4 p1 = q1.v*q1_denom;
  Vector4 p2 = q2.v*q2_denom;
  Vector4 p3 = q3.v*q3_denom;
  
  // Viewport Transformation
  p1 = ctx.camera.vp_mat*p1;
  p2 = ctx.camera.vp_mat*p2;
  p3 = ctx.camera.vp_mat*p3;
  
  p2x_p1x = p2.x - p1.x;
  p3y_p1y = p3.y - p1.y;
  p3x_p1x = p3.x - p1.x;
  p2y_p1y = p2.y - p1.y;
  
  det_denom = p2x_p1x*p3y_p1y - p3x_p1x*p2y_p1y;
  if(det_denom == 0) return;
  else det_denom = 1.0/det_denom;
    
  GetMinMax(min_x,max_x,p1.x,p2.x,p3.x);
  GetMinMax(min_y,max_y,p1.y,p2.y,p3.y);
  
  // 以下の２重for loopは,三角形のBounding Boxを走る
  for(int y=min_y;y<max_y;++y){
    for(int x=min_x;x<max_x;++x){      
      x_p1x = x - p1.x;
      y_p1y = y - p1.y;

      // alpha,beta,gammaの内少なくとも1つ負の値なら,三角形外を意味する,よって即座にcontinue
      beta  = (p3y_p1y*x_p1x - p3x_p1x*y_p1y)*det_denom;
      if(beta < 0) continue;
      
      gamma = (p2x_p1x*y_p1y - p2y_p1y*x_p1x)*det_denom;
      if(gamma < 0) continue;
      
      alpha  = (1 - beta - gamma);
      if(alpha < 0) continue;

      // Depth Test行い,テストに通らなかった場合continue
      if(ctx.use_depth_test and not DepthTest(p1.z,p2.z,p3.z,alpha,beta,gamma,x,y,ctx)) continue;
      
      color = Color::Interpolate(q1.c,q2.c,q3.c,alpha,beta,gamma); // 色を補間

      if(p_ctx.texture_enable){
	// テクスチャの色を計算
	alpha_q1_denom = alpha*q1_denom; beta_q2_denom = beta*q2_denom; gamma_q3_denom = gamma*q3_denom;
	u_s = q1.t.x*alpha_q1_denom + q2.t.x*beta_q2_denom + q3.t.x*gamma_q3_denom;
	v_s = q1.t.y*alpha_q1_denom + q2.t.y*beta_q2_denom + q3.t.y*gamma_q3_denom;
	denom = 1.0/(alpha_q1_denom + beta_q2_denom + gamma_q3_denom);      
	u = u_s*denom;
	v = v_s*denom;
	
	color = color*p_ctx.texture.TextureLookup(u,v); // テクスチャの色をライティングの色とブレンディング
      }
      
      PutPixel(x,y,color,ctx); // ピクセルを描写
    }
  }
}

// 方程式はFundamentals of Computer Graphics 4th p238から引用
// コードが三倍に重複しているが,効率を考えて敢えてこのようにしている(マクロを使って重複を減らすのもあり)
void PhongReflection(Vertex& p0,Vertex& p1,Vertex& p2,const PolygonContext& p_ctx,const Context& ctx){
  Color ambient_rgb0,diffuse_rgb0,specular_rgb0;
  Color ambient_rgb1,diffuse_rgb1,specular_rgb1;
  Color ambient_rgb2,diffuse_rgb2,specular_rgb2;
  
  Vector4 eye_dir0,light_dir0,n0,h0;
  Vector4 eye_dir1,light_dir1,n1,h1;
  Vector4 eye_dir2,light_dir2,n2,h2;
  
  real diffuse_coeff0,phong_coeff0;
  real diffuse_coeff1,phong_coeff1;
  real diffuse_coeff2,phong_coeff2;

  real inv_d0;
  real inv_d1;
  real inv_d2;

  Color color0,color1,color2; // 戻り値となる変数

  // 点からカメラへの単位ベクトル
  eye_dir0 = normalize(ctx.camera.eye - p0.v);
  eye_dir1 = normalize(ctx.camera.eye - p1.v);
  eye_dir2 = normalize(ctx.camera.eye - p2.v);

  // 法線ベクトルを正規化
  n0 = normalize(p0.n);
  n1 = normalize(p1.n);
  n2 = normalize(p2.n);

  // Ambientの色を計算(光源によらないため)
  ambient_rgb0 = p_ctx.material.diffuse*p0.c;
  ambient_rgb1 = p_ctx.material.diffuse*p1.c;
  ambient_rgb2 = p_ctx.material.diffuse*p2.c;
  
  color0 = ambient_rgb0;
  color1 = ambient_rgb1;
  color2 = ambient_rgb2;
  
  for(int i=0;i<kMaxLights;++i){
    if(not ctx.usedLights[i]) continue;

    // 点から光源への単位ベクトル
    light_dir0 = ctx.lights[i].v - p0.v;
    light_dir1 = ctx.lights[i].v - p1.v;
    light_dir2 = ctx.lights[i].v - p2.v;

    // あとで光の減衰を計算するために,距離を保存しておく
    inv_d0 = norm(light_dir0);
    inv_d1 = norm(light_dir1);
    inv_d2 = norm(light_dir2);

    // 点から光源への距離が,0の場合(つまり,重なっている場合),その光の寄与を0とする
    // そうでない場合,距離の逆数を保存
    if(inv_d0 == 0) inv_d0 = 0;
    else inv_d0 = 1.0/inv_d0;
    
    if(inv_d1 == 0) inv_d1 = 0;
    else inv_d1 = 1.0/inv_d1;
    
    if(inv_d2 == 0) inv_d2 = 0;
    else inv_d2 = 1.0/inv_d2;

    // 正規化
    light_dir0 *= inv_d0;
    light_dir1 *= inv_d1;
    light_dir2 *= inv_d2;
    
    // half-way ベクトル
    h0 = normalize(eye_dir0 + light_dir0);
    h1 = normalize(eye_dir1 + light_dir1);
    h2 = normalize(eye_dir2 + light_dir2);

    // Diffuse計算に使うスカラー値を求める
    diffuse_coeff0 = max(0,dot(n0,light_dir0));
    diffuse_coeff1 = max(0,dot(n1,light_dir1));
    diffuse_coeff2 = max(0,dot(n2,light_dir2));

    // Specular計算に使うスカラー値を求める
    phong_coeff0 = powf(max(0,dot(h0,n0)),p_ctx.material.shininess);
    phong_coeff1 = powf(max(0,dot(h1,n1)),p_ctx.material.shininess);
    phong_coeff2 = powf(max(0,dot(h2,n2)),p_ctx.material.shininess);

    // Diffuse
    diffuse_rgb0 = p_ctx.material.diffuse*(ctx.lights[i].c*diffuse_coeff0);
    diffuse_rgb1 = p_ctx.material.diffuse*(ctx.lights[i].c*diffuse_coeff1);
    diffuse_rgb2 = p_ctx.material.diffuse*(ctx.lights[i].c*diffuse_coeff2);

    // Specular
    specular_rgb0 = (ctx.lights[i].c*p_ctx.material.specular)*phong_coeff0;
    specular_rgb1 = (ctx.lights[i].c*p_ctx.material.specular)*phong_coeff1;
    specular_rgb2 = (ctx.lights[i].c*p_ctx.material.specular)*phong_coeff2;

    // (diffuse + specular) * 光源の強さ * 距離の逆数(減衰)
    color0 += (diffuse_rgb0 + specular_rgb0)*ctx.lights[i].i*inv_d0;
    color1 += (diffuse_rgb1 + specular_rgb1)*ctx.lights[i].i*inv_d1;
    color2 += (diffuse_rgb2 + specular_rgb2)*ctx.lights[i].i*inv_d2;      
  }   

  p0.c = Color::Clamp(color0);
  p1.c = Color::Clamp(color1);
  p2.c = Color::Clamp(color2);
  
}
