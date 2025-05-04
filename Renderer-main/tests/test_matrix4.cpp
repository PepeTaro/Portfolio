#include "matrix4.h"
#include "vector4.h"

#include "gtest/gtest.h"
#include "utils_for_test.h"

static bool CheckAdd(const Matrix4& a,const Matrix4& b,const Matrix4& c){
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      if(not AlmostEqual(c(i,j),a(i,j) + b(i,j))){
	return false;
      }
    }
  }

  return true;
}

static bool CheckSub(const Matrix4& a,const Matrix4& b,const Matrix4& c){
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      if(not AlmostEqual(c(i,j),a(i,j) - b(i,j))){
	return false;
      }
    }
  }

  return true;
}

static bool CheckMul(const Matrix4& a,const Matrix4& b,const Matrix4& c){
  real val;
  
  for(int i=0;i<4;++i){
    for(int j=0;j<4;++j){
      val = 0.0;
      
      for(int k=0;k<4;++k){
	val += a(i,k)*b(k,j);
      }
      if(not AlmostEqual(c(i,j),val)){
	return false;
      }
      
    }
  }

  return true;
}

TEST(TestAdd,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Matrix4 a = RandomMatrix4();
    Matrix4 b = RandomMatrix4();
    
    EXPECT_TRUE(CheckAdd(a,b,a+b));
  }  
}

TEST(TestSub,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Matrix4 a = RandomMatrix4();
    Matrix4 b = RandomMatrix4();
    
    EXPECT_TRUE(CheckSub(a,b,a-b));
  }
}

TEST(TestMul,DoesEqual){
  for(int i=0;i<num_test_random;++i){
    Matrix4 a = RandomMatrix4();
    Matrix4 b = RandomMatrix4();
    
    EXPECT_TRUE(CheckMul(a,b,a*b));
  }
}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
