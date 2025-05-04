#include "vector4.h"

#include <iostream>
#include <cmath>
#include "gtest/gtest.h"
#include "utils_for_test.h"

static bool CheckAdd(const Vector4& a,const Vector4& b,const Vector4& c){
  for(int i=0;i<4;++i){
    if(not AlmostEqual(c[i],a[i] + b[i])){
      return false;
    }    
  }

  return true;
}

static bool CheckSub(const Vector4& a,const Vector4& b,const Vector4& c){
  for(int i=0;i<4;++i){
    if(not AlmostEqual(c[i],a[i] - b[i])){
      return false;
    }    
  }

  return true;
}

static bool CheckMul(const Vector4& a,real c,const Vector4& b){
  for(int i=0;i<4;++i){
    if(not AlmostEqual(c*a[i],b[i])){
      return false;
    }    
  }

  return true;
}

static bool CheckDiv(const Vector4& a,real c,const Vector4& b){
  for(int i=0;i<4;++i){
    if(not AlmostEqual(a[i]/c,b[i])){
      return false;
    }    
  }

  return true;
}

bool TestIndex(const Vector4& vec,real x,real y,real z,real w){
  if(vec[0] != x) return false;
  if(vec[1] != y) return false;
  if(vec[2] != z) return false;
  if(vec[3] != w) return false;
  return true;
}

TEST(TestAdd,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Vector4 a = RandomVector4();
    Vector4 b = RandomVector4();
    
    EXPECT_TRUE(CheckAdd(a,b,a+b));

    Vector4 c = a+b;
    a += b;
    EXPECT_TRUE(EXPECT_VEC4_EQ(a,c));
  }
}

TEST(TestSub,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Vector4 a = RandomVector4();
    Vector4 b = RandomVector4();
    
    EXPECT_TRUE(CheckSub(a,b,a-b));

    Vector4 c = a-b;
    a -= b;
    EXPECT_TRUE(EXPECT_VEC4_EQ(a,c));
  }  
}

TEST(TestMul,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Vector4 a = RandomVector4();
    real c = GenerateRandom();

    EXPECT_TRUE(CheckMul(a,c,c*a));
    EXPECT_TRUE(CheckMul(a,c,a*c));

    Vector4 b = c*a;
    a *= c;
    
    EXPECT_TRUE(EXPECT_VEC4_EQ(a,b));
  }  
}

TEST(TestDiv,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Vector4 a = RandomVector4();
    real c;
    
    while((c = GenerateRandom()) == 0.0)
      ;

    EXPECT_TRUE(CheckDiv(a,c,a/c));

    Vector4 b = a/c;
    a /= c;

    EXPECT_TRUE(EXPECT_VEC4_EQ(a,b));
  }

}

TEST(TestIndex,DoesEqual){  
  {
    Vector4 a({1,2,3,4});
    EXPECT_TRUE(TestIndex(a,1,2,3,4));
  };
  
  {
    Vector4 a({0,-1,1,0});
    EXPECT_TRUE(TestIndex(a,0,-1,1,0));    
  };

  {
    Vector4 a({-3,-1,-4,-1});
    EXPECT_TRUE(TestIndex(a,-3,-1,-4,-1));
  };

}

// w成分を無視した,dot積であることに注意
TEST(TestDot,DoesEqual){

  {
    Vector4 a({1,2,3,4});
    Vector4 b({1,2,3,4});
    EXPECT_FLOAT_EQ(dot(a,b),1*1 + 2*2 + 3*3);
  };
  
  {
    Vector4 a({0,-1,1,0});
    Vector4 b({1,0,0,-1});
    EXPECT_FLOAT_EQ(dot(a,b),0);    
  };

  {
    Vector4 a({-3,-1,-4,-1});
    Vector4 b({3,0,0,0});
    EXPECT_FLOAT_EQ(dot(a,b),-3*3);    
  };

  {
    Vector4 a({1,2,3,3141592});
    Vector4 b({1,2,3,653989});
    EXPECT_FLOAT_EQ(dot(a,b),1*1 + 2*2 + 3*3);    
  };

}

// w成分を無視した,ノルムであることに注意
TEST(TestNorm,DoesEqual){

  {
    Vector4 a({1,2,3,4});
    EXPECT_FLOAT_EQ(norm(a),sqrtf(1*1 + 2*2 + 3*3));
  };
  
  {
    Vector4 a({0,-1,1,0});
    EXPECT_FLOAT_EQ(norm(a),sqrtf(-1*-1 + 1*1));    
  };

  {
    Vector4 a({-3,-1,-4,-1});
    EXPECT_FLOAT_EQ(norm(a),sqrtf(-3*-3 + -1*-1 + -4*-4));    
  };

  {
    Vector4 a({1,2,3,3141592});
    EXPECT_FLOAT_EQ(norm(a),sqrtf(1*1 + 2*2 + 3*3));    
  };

}

// w成分を無視した,Cross積であることに注意
TEST(TestCross,DoesEqual){

  {
    Vector4 a({1,0,0,0});
    Vector4 b({0,1,0,0});
    Vector4 expectation({0,0,1,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(cross(a,b),expectation));
  };

  {
    Vector4 a({1,0,0,314});
    Vector4 b({0,1,0,1592});
    Vector4 expectation({0,0,1,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(cross(a,b),expectation));
  };
  
  {
    Vector4 a({0,1,0,0});
    Vector4 b({0,0,1,0});
    Vector4 expectation({1,0,0,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(cross(a,b),expectation));
  };

  {
    Vector4 a({0,0,1,0});
    Vector4 b({0,1,0,0});
    Vector4 expectation({-1,0,0,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(cross(a,b),expectation));
  };

}

// w成分を無視した,正規化であることに注意
TEST(TestNormalize,DoesEqual){

  {
    Vector4 a({1,2,3,4});
    Vector4 b = normalize(a);
    EXPECT_FLOAT_EQ(dot(b,b),1);    
  };
  
  {
    Vector4 a({0,-1,1,0});
    Vector4 b = normalize(a);
    EXPECT_FLOAT_EQ(dot(b,b),1);
    
  };

  {
    Vector4 a({-3,-1,-4,-1});
    Vector4 b = normalize(a);
    EXPECT_FLOAT_EQ(dot(b,b),1);
  };
  
  {
    Vector4 a({1,2,3,3141592});
    Vector4 b = normalize(a);
    EXPECT_FLOAT_EQ(dot(b,b),1);    
  };

}


TEST(TestNormal,DoesEqual){
  {
    Vector4 a({-1,0,0,0});
    Vector4 b({1,0,0,0});
    Vector4 c({0,1,0,0});
    Vector4 expectation({0,0,2,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(normal(a,b,c),expectation));
  };

  {
    Vector4 a({1,0,0,0});
    Vector4 b({-1,0,0,0});
    Vector4 c({0,1,0,0});
    Vector4 expectation({0,0,-2,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(normal(a,b,c),expectation));
  };

  {
    Vector4 a({-1,0,0,314});
    Vector4 b({1,0,0,-1592});
    Vector4 c({0,1,0,6535});
    Vector4 expectation({0,0,2,0});    
    EXPECT_TRUE(EXPECT_VEC4_EQ(normal(a,b,c),expectation));
  };

}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
