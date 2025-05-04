#include "polygon.h"

Mesh GetTriangleMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Color& c1,const Color& c2,const Color& c3,const Material& material){
  Mesh mesh;
  Vector4 n = normalize(cross(p2 - p1,p3 - p1));
  
  mesh.appendVertex(p1,c1,n);
  mesh.appendVertex(p2,c2,n);
  mesh.appendVertex(p3,c3,n);
  
  mesh.specifyTriangle({0,1,2});
  mesh.setMaterial(material);
  
  return mesh;
}

Mesh GetTriangleMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Color& c,const Material& material){
  return GetTriangleMesh(p1,p2,p3,c,c,c,material);
}

Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,
			const Color& c1,const Color& c2,const Color& c3,const Color& c4,
			const Material& material,const Texture& texture)
{
  Mesh mesh = GetPlaneMesh(p1,p2,p3,p4,c1,c2,c3,c4,material);
  mesh.setTexture(texture);
  return mesh;  
}

// テクスチャ座標は,p1,p2,p3,p4がそれぞれ(0,0),(1,0),(1,1),(0,1)となるように配置
Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,
		  const Color& c1,const Color& c2,const Color& c3,const Color& c4,
		  const Material& material)  
{
  Mesh mesh;
  Vector4 n = normalize(cross(p2 - p1,p3 - p1));
  
  mesh.appendVertex(p1,c1,n,Vector4(0,0,0,1));
  mesh.appendVertex(p2,c2,n,Vector4(1,0,0,1));
  mesh.appendVertex(p3,c3,n,Vector4(1,1,0,1));
  mesh.appendVertex(p4,c4,n,Vector4(0,1,0,1));
  
  mesh.specifyTriangle({0,1,2});
  mesh.specifyTriangle({2,3,0});
  mesh.setMaterial(material);
  
  return mesh;
}

Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,const Color& c,const Material& material,const Texture& texture){
  Mesh mesh = GetPlaneMesh(p1,p2,p3,p4,c,material);
  mesh.setTexture(texture);
  return mesh;
}

Mesh GetPlaneMesh(const Vector4& p1,const Vector4& p2,const Vector4& p3,const Vector4& p4,const Color& c,const Material& material){
  return GetPlaneMesh(p1,p2,p3,p4,c,c,c,c,material);
}

Mesh GetCubeMesh(float l,float b,float n,float r,float t,float f,
		 const Color& c1,const Color& c2,const Color& c3,const Color& c4,const Color& c5,const Color& c6,const Color& c7,const Color& c8,
		 const Material& material){
  Mesh mesh;
  
  Vector4 p1(l,b,n,1);
  Vector4 p2(l,t,n,1);
  Vector4 p3(r,t,n,1);
  Vector4 p4(r,b,n,1);
  Vector4 p5(l,b,f,1);
  Vector4 p6(l,t,f,1);
  Vector4 p7(r,t,f,1);
  Vector4 p8(r,b,f,1);

  // normals of the surfaces
  Vector4 ns1 = normalize(cross(p3 - p1,p2 - p1));
  Vector4 ns2 = normalize(cross(p7 - p4,p3 - p4));
  Vector4 ns3 = normalize(cross(p6 - p8,p7 - p8));
  Vector4 ns4 = normalize(cross(p2 - p5,p6 - p5));
  Vector4 ns5 = normalize(cross(p7 - p2,p6 - p2));
  Vector4 ns6 = normalize(cross(p4 - p5,p1 - p5));

  Vector4 n1 = (ns1 + ns6 + ns4)/3.0;
  Vector4 n2 = (ns1 + ns5 + ns4)/3.0;
  Vector4 n3 = (ns1 + ns5 + ns2)/3.0;
  Vector4 n4 = (ns1 + ns6 + ns2)/3.0;
  Vector4 n5 = (ns4 + ns6 + ns2)/3.0;
  Vector4 n6 = (ns4 + ns5 + ns3)/3.0;
  Vector4 n7 = (ns2 + ns5 + ns3)/3.0;
  Vector4 n8 = (ns2 + ns6 + ns3)/3.0;
  
  mesh.appendVertex(p1,c1,n1);
  mesh.appendVertex(p2,c2,n2);
  mesh.appendVertex(p3,c3,n3);
  mesh.appendVertex(p4,c4,n4);
  mesh.appendVertex(p5,c5,n5);
  mesh.appendVertex(p6,c6,n6);
  mesh.appendVertex(p7,c7,n7);
  mesh.appendVertex(p8,c8,n8);
    
  mesh.specifyTriangle({0,2,1});
  mesh.specifyTriangle({0,3,2});
  mesh.specifyTriangle({3,6,2});
  mesh.specifyTriangle({3,7,6});  
  mesh.specifyTriangle({7,5,6});
  mesh.specifyTriangle({7,4,5});
  mesh.specifyTriangle({4,1,5});
  mesh.specifyTriangle({4,0,1});
  mesh.specifyTriangle({1,6,5});
  mesh.specifyTriangle({1,2,6});
  mesh.specifyTriangle({4,3,0});
  mesh.specifyTriangle({4,7,3});
  
  mesh.setMaterial(material);
  
  return mesh;
}

Mesh GetCubeMesh(float l,float b,float n,float r,float t,float f,const Color& c,const Material& material){
  return GetCubeMesh(l,b,n,r,t,f,c,c,c,c,c,c,c,c,material);
}

// nが大きければ,球の形が"滑らか"になる
Mesh GetSphereMesh(int n,real r,const Color& c,const Material& material,const Texture& texture){
  Mesh mesh = GetSphereMesh(n,r,c,material);
  mesh.setTexture(texture);
  return mesh;  
}

Mesh GetSphereMesh(int n,real r,const Color& c,const Material& material){
  Mesh mesh;
  real phi,theta,x,y,z;
  real u,v;
  
  for(int j=-n;j<=n;++j){
    for(int i=0;i<=n-1;++i){
      phi = 2.0f*M_PI*i/n; theta = M_PI*j/(2.0*n);
      x = r*cos(theta)*cos(phi); y = r*cos(theta)*sin(phi); z = r*sin(theta);
      u = (1.0*i)/(n); v = (j + n)/(2.0*n);
      
      mesh.appendVertex(Vector4(x,y,z,1),c,normalize(Vector4(x,y,z,0)),Vector4(u,v,0,1));
      
      if(j == -n or j == n) break;
    }
  }

  int k,i1,i2,i3,i4,last_idx;
  for(int j=-n;j<n;++j){
    for(int i=0;i<n;++i){
      if(j != -n and j != n-1){
	k = (j+(n-1))*n;
	i1 = k+(i+1)%n+1;
	i2 = k+n+i+1;
	i3 = k+i+1;
	i4 = k+(i+1)%n+n+1;
	
	mesh.specifyTriangle({i1,i2,i3});
	mesh.specifyTriangle({i1,i4,i2});
      }else if(j == -n){
	i1 = (i+1)%n+1;
	i2 = i+1;
	mesh.specifyTriangle({0,i1,i2});
      }else{
	k = (j+(n-1))*n;
	last_idx = 2*n*(n-1)+n+1;
	i1 = k+i+1;
	i2 = k+(i+1)%n+1;

	mesh.specifyTriangle({i1,i2,last_idx});
      }
    }
  }
  
  mesh.setMaterial(material);
  
  return mesh;
}

Strip GetTriangleStrip(const Vector4& p1,const Vector4& p2,const Vector4& p3,
			      const Color& c1,const Color& c2,const Color& c3,
			      const Vector4& t1,const Vector4& t2,const Vector4& t3,
			      const Material& material)
{
  Strip strip;
  Vector4 n = normalize(cross(p2 - p1,p3 - p1));
  
  strip.appendVertex(p1,c1,n,t1);
  strip.appendVertex(p2,c2,n,t2);
  strip.appendVertex(p3,c3,n,t3);
  
  strip.setMaterial(material);
  
  return strip;  
}

Strip GetTriangleStrip(const Vector4& p1,const Vector4& p2,const Vector4& p3,
			      const Color& c,
			      const Vector4& t1,const Vector4& t2,const Vector4& t3,
			      const Material& material)
{
  return GetTriangleStrip(p1,p2,p3,c,c,c,t1,t2,t3,material);
}
