#include "clip.h"

/**
  このファイル内の関数において,cp変数(clipping plane)は,以下を意味する:
    cp = 0 LEFT,cp = 1 RIGHT,cp = 2 BOTTOM,cp = 3 TOP,cp = 4 FAR,cp = 5 NEAR
    (ここで,例えばLEFTは,Canonical Viewing Volumeのx=-1でのy-z平面を表す)
 **/

// 点posが,点eyeから見えない場合Trueを返す,見える場合Falseを返す(つまり,BackfaceCullingしたならTrue,してないならFalse)
bool BackfaceCulling(const Vector4& pos,const Vector4& n,const Context& ctx){
  if(dot(n,ctx.camera.eye - pos) < 0) return true;
  else return false;
}

// 点pのOutcodeを計算
int Outcode(const Vector4& p){
  // wがマイナスと仮定  
  real w = p.w*(1.0 + clip_epsilon); //点pが数値計算の誤差で実際にはクリッピング内にあるのに,クリッピング外と判定される誤差を緩和するための処置
  
  return (p.x>-w) |
     ((p.x<w)<<1) |
    ((p.y>-w)<<2) |
     ((p.y<w)<<3) |
    ((p.z>-w)<<4) | 
    ((p.z<w)<<5);  
}

// 引数cpにより表されるClipping Planeとp1,p2を結ぶ直線との交点を求めて,引数qに保存。戻り値として,直線のパラメーター値を返す。
// http://medialab.di.unipi.it/web/IUM/Waterloo/node51.html 参考
real Intersect(const Vector4& p1,const Vector4& p2,int cp,Vector4& q){
  real t;
  real denom;
  
  if(cp == 0){
    denom = (p1.x - p2.x) + (p1.w - p2.w);
    t = (p1.w + p1.x)/denom;
    
  }else if(cp == 1){
    denom = (p2.x - p1.x) + (p1.w - p2.w);
    t = (p1.w - p1.x)/denom;
    
  }else if(cp == 2){
    denom = (p1.y - p2.y) + (p1.w - p2.w);
    t = (p1.w + p1.y)/denom;
    
  }else if(cp == 3){
    denom = (p2.y - p1.y) + (p1.w - p2.w);
    t = (p1.w - p1.y)/denom;
    
  }else if(cp == 4){
    denom = (p1.w - p2.w) + (p1.z - p2.z);
    t = (p1.w + p1.z)/denom;
    
  }else if(cp == 5){
    denom = (p2.w - p1.w) + (p1.z - p2.z);
    t = (p1.z - p1.w)/denom;
  }
  
  // 数値誤差の影響でパラメーター値が0.0から1.0にない場合,補正
  if(t < 0.0) t = 0.0;
  else if(t > 1.0) t = 1.0;  
  
  q = p1 + t*(p2 - p1);
  
  return t;
}

// Cohen–Sutherland algorithmとSutherland–Hodgman algorithmを組み合わせたClippingアルゴリズムを使用。  
void Clipping(const Vertex& p1,const Vertex& p2,const Vertex& p3,int cp,const PolygonContext& p_ctx,Context& ctx){
  int oc1,oc2,oc3,cp_code,oc1_out,oc2_out,oc3_out,num_out;
  oc1 = Outcode(p1.v);oc2 = Outcode(p2.v);oc3 = Outcode(p3.v); // 3点のOutcodeを各々計算
  
  if(not (oc1|oc2|oc3)){ // 完全にClipping Window内に3点が入っている場合
    Shading(p1,p2,p3,p_ctx,ctx);    // Shading関数は,Perspective division,Depth test,Viewport transformを順に実行し,ピクセルを描写する関数
  }else if(oc1 & oc2 & oc3){return;}// 3点が完全にClipping Window外にある場合
  else{ // 1点あるいは2点がClipping Window外にある場合

    // 理論上cp == 6(すべてのClipping Planeに対してClippingした後にまだClipping Window外に点があると判定)とはならない
    if(cp == 6){
      #ifndef NDEBUG
      std::cout << "(Clipping)数値誤差発生!" << std::endl;
      #endif
      return;
    }

    cp_code = 1 << cp; // cp_codeは,cp変数の値に対応するOutcodeとなる(cp=0なら,clip.hで定義されているLEFT定数,cp=1なら,RIGHT定数 ...)
    oc1_out = oc1&cp_code;oc2_out = oc2&cp_code;oc3_out = oc3&cp_code; // 3点の内どれが,Clipping Plane外にあるかを計算
    num_out = (oc1_out != 0) + (oc2_out != 0) + (oc3_out != 0); // p1,p2,p3の内いくつが,Clipping Window外にあるか保存
    
    if(num_out == 0){ Clipping(p1,p2,p3,cp+1,p_ctx,ctx);} // Clipping Plane cpの"内側"の場合,すぐに次のClipping Planeへ移行
    else if(num_out == 1){ // １点のみがClipping Plane外
      Vertex q1,q2,q3,a,b;
      real t1,t2;
      
      // 反時計回りになるように調整
      if(oc1_out){
	q1 = p1; q2 = p2; q3 = p3;
      }else if(oc2_out){
	q1 = p2; q2 = p3; q3 = p1;
      }else{
	q1 = p3; q2 = p1; q3 = p2;
      }

      // a,bがClipping Planeとの接点となる      
      t1 = Intersect(q1.v,q2.v,cp,a.v);
      t2 = Intersect(q1.v,q3.v,cp,b.v);      
      
      // aとbの色を計算
      a.c = Color::Interpolate(q1.c,q2.c,t1);
      b.c = Color::Interpolate(q1.c,q3.c,t2);
      
      // Texture
      a.t = q1.t + t1*(q2.t - q1.t);
      b.t = q1.t + t2*(q3.t - q1.t);
      
      Clipping(a,q2,b,cp+1,p_ctx,ctx); Clipping(q2,q3,b,cp+1,p_ctx,ctx);      
    }else{ // 2点がClipping Plane外
      Vertex q1,q2,q3,a,b;
      real t1,t2;
      
      // 反時計回りになるように調整
      if(not oc1_out){
	q1 = p3; q2 = p1; q3 = p2;
      }else if(not oc2_out){
	q1 = p1; q2 = p2; q3 = p3;
      }else{
	q1 = p2; q2 = p3; q3 = p1;
      }
      
      // a,bがClipping Planeとの接点となる
      t1 = Intersect(q1.v,q2.v,cp,a.v);
      t2 = Intersect(q2.v,q3.v,cp,b.v);
      
      // aとbの色を計算
      a.c = Color::Interpolate(q1.c,q2.c,t1);
      b.c = Color::Interpolate(q2.c,q3.c,t2);

      // Texture
      a.t = q1.t + t1*(q2.t - q1.t);
      b.t = q2.t + t2*(q3.t - q2.t);
      
      Clipping(a,q2,b,cp+1,p_ctx,ctx);
    }    
  }  
}
