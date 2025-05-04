#include <iostream>
#include <vector>
#include <array>
#include <cassert>

#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "im2col.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestIm2Col,DoesEqual){
  { // test1
    Eigen::MatrixXf input1,input2,expected;
    std::vector<Eigen::MatrixXf> input;

    input1 = ArangeRow({3,3}),input2 = ArangeRow({3,3},{10,10,19});
    input.push_back(input1),input.push_back(input2);
    expected = Eigen::MatrixXf::Zero(8,4);
    expected <<
      1,  4,  2,  5,
      4,  7,  5,  8,
      2,  5,  3,  6,
      5,  8,  6,  9,
      10, 13, 11, 14,
      13, 16, 14, 17,
      11, 14, 12, 15,
      14, 17, 15, 18;
    EXPECT_TRUE(CompareMatrices(Im2Col(input,{2,2,2,1,1}),expected));
  };

  {//test2
    Eigen::MatrixXf input1,input2,expected;
    std::vector<Eigen::MatrixXf> input;
    
    input1 = ArangeRow({3,3}),input2 = ArangeRow({3,3},{10,10,19});
    input.push_back(input1),input.push_back(input2);
    expected = Eigen::MatrixXf::Zero(2,9);
    expected <<
      1,  4,  7,  2,  5,  8,  3,  6,  9,
      10, 13, 16, 11, 14, 17, 12, 15, 18;
    
    EXPECT_TRUE(CompareMatrices(Im2Col(input,{1,1,2,1,1}),expected));
  };

  {//test3
    Eigen::MatrixXf input1,input2,expected;
    std::vector<Eigen::MatrixXf> input;
    
    input1 = ArangeRow({3,3}),input2 = ArangeRow({3,3},{10,10,19});
    input.push_back(input1),input.push_back(input2);
    expected = Eigen::MatrixXf::Zero(18,1);
    expected <<
      1,4,7,2,5,8,3,6,9,10,13,16,11,14,17,12,15,18;

    EXPECT_TRUE(CompareMatrices(Im2Col(input,{3,3,2,1,1}),expected));
  };

  {//test4
    Eigen::MatrixXf input1,input2,input3,expected;
    std::vector<Eigen::MatrixXf> input;

    input1 = ArangeRow({4,3}),input2 = ArangeRow({4,3},{13,13,25}),input3=ArangeRow({4,3},{13,26,38});
    input.push_back(input1),input.push_back(input2),input.push_back(input3);  

    expected = Eigen::MatrixXf::Zero(18,2);
    expected <<
      1,  7,
      4, 10,
      2,  8,
      5, 11,
      3,  9,
      6, 12,
      13, 19,
      16, 22,
      14, 20,
      17, 23,
      15, 21,
      18, 24,
      26, 32,
      29, 35,
      27, 33,
      30, 36,
      28, 34,
      31, 37;


    EXPECT_TRUE(CompareMatrices(Im2Col(input,{2,3,3,2,1}),expected));
  };

  
  {//test5
    Eigen::MatrixXf input1,input2,input3,expected;
    std::vector<Eigen::MatrixXf> input;

    input1 = ArangeRow({4,5}),input2 = ArangeRow({4,5},{21,21,41}),input3=ArangeRow({4,5},{21,42,62});
    input.push_back(input1),input.push_back(input2),input.push_back(input3);  
    expected = Eigen::MatrixXf::Zero(18,4);
    expected <<
      1, 11,  3, 13,
      6, 16,  8, 18,
      2, 12,  4, 14,
      7, 17,  9, 19,
      3, 13,  5, 15,
      8, 18, 10, 20,
      21, 31, 23, 33,
      26, 36, 28, 38,
      22, 32, 24, 34,
      27, 37, 29, 39,
      23, 33, 25, 35,
      28, 38, 30, 40,
      42, 52, 44, 54,
      47, 57, 49, 59,
      43, 53, 45, 55,
      48, 58, 50, 60,
      44, 54, 46, 56,
      49, 59, 51, 61;


    EXPECT_TRUE(CompareMatrices(Im2Col(input,{2,3,3,2,2}),expected));
  };
  
}

TEST(TestCol2Im,DoesEqual){

  {//test1
    Eigen::MatrixXf input,expected0,expected1,expected2;
    std::vector<Eigen::MatrixXf> output;

      input = ArangeRow({3,6});
      output = Col2Im(input,{3,2,3});
      
      expected0 = Eigen::MatrixXf::Zero(3,2);
      expected0 <<
	1, 4,
	2, 5,
	3, 6;
      
      expected1 = Eigen::MatrixXf::Zero(3,2);
      expected1 <<
	7, 10,
	8, 11,
	9, 12;

      expected2 = Eigen::MatrixXf::Zero(3,2);
      expected2 <<
	13, 16,
	14, 17,
	15, 18;

      EXPECT_TRUE(CompareMatrices(output[0],expected0));
      EXPECT_TRUE(CompareMatrices(output[1],expected1));
      EXPECT_TRUE(CompareMatrices(output[2],expected2));
  };

  
  {//test2
    Eigen::MatrixXf input,expected0,expected1,expected2,expected3,expected4;
    std::vector<Eigen::MatrixXf> output;

      input = ArangeRow({5,9});
      output = Col2Im(input,{3,3,5});
      
      expected0 = Eigen::MatrixXf::Zero(3,3);
      expected0 <<
	1, 4, 7,
	2, 5, 8,
	3, 6, 9;
      
      expected1 = Eigen::MatrixXf::Zero(3,3);
      expected1 <<
	10, 13, 16,
	11, 14, 17,
	12, 15, 18;

      expected2 = Eigen::MatrixXf::Zero(3,3);
      expected2 <<
	19, 22, 25,
	20, 23, 26,
	21, 24, 27;

      expected3 = Eigen::MatrixXf::Zero(3,3);
      expected3 <<
	28, 31, 34,
	29, 32, 35,
	30, 33, 36;

      expected4 = Eigen::MatrixXf::Zero(3,3);
      expected4 <<
	37, 40, 43,
	38, 41, 44,
	39, 42, 45;

      EXPECT_TRUE(CompareMatrices(output[0],expected0));
      EXPECT_TRUE(CompareMatrices(output[1],expected1));
      EXPECT_TRUE(CompareMatrices(output[2],expected2));
      EXPECT_TRUE(CompareMatrices(output[3],expected3));
      EXPECT_TRUE(CompareMatrices(output[4],expected4));
  };
}


TEST(TestKer2Col,DoesEqual){

  {//test1
    Eigen::MatrixXf ker11,ker21,ker31,ker12,ker22,ker32,ker13,ker23,ker33,expected;
    std::vector<Eigen::MatrixXf> ker1,ker2,ker3;
    std::vector<std::vector<Eigen::MatrixXf>> ker;
    
    ker11 = ArangeRow({2,2}),ker21 = ArangeRow({2,2},{5,5,9}),ker12 = ArangeRow({2,2},{5,10,14}),ker22 = ArangeRow({2,2},{5,15,19});  
    ker1.push_back(ker11),ker1.push_back(ker21);
    ker2.push_back(ker12),ker2.push_back(ker22);
    ker.push_back(ker1),ker.push_back(ker2);

    expected = Eigen::MatrixXf::Zero(8,2);
    expected <<
      1, 10,
      3, 12,
      2, 11,
      4, 13,
      5, 15,
      7, 17,
      6, 16,
      8, 18;
    
    EXPECT_TRUE(CompareMatrices(Ker2Col(ker),expected));
  }
  
  {//test2
    Eigen::MatrixXf ker11,ker21,ker31,ker12,ker22,ker32,ker13,ker23,ker33,expected;
    std::vector<Eigen::MatrixXf> ker1,ker2,ker3;
    std::vector<std::vector<Eigen::MatrixXf>> ker;
    
    ker11 = ArangeRow({2,2}),ker21 = ArangeRow({2,2},{5,5,9}),ker12 = ArangeRow({2,2},{5,10,14}),ker22 = ArangeRow({2,2},{5,15,19});  
    ker1.push_back(ker11),ker1.push_back(ker21);
    ker2.push_back(ker12),ker2.push_back(ker22);
    ker.push_back(ker1),ker.push_back(ker2);

    expected = Eigen::MatrixXf::Zero(8,2);
    expected <<
      1, 10,
      3, 12,
      2, 11,
      4, 13,
      5, 15,
      7, 17,
      6, 16,
      8, 18;
    
    EXPECT_TRUE(CompareMatrices(Ker2Col(ker),expected));
    
  };

  {//test3
    Eigen::MatrixXf ker11,ker21,ker31,ker12,ker22,ker32,ker13,ker23,ker33,expected;
    std::vector<Eigen::MatrixXf> ker1,ker2,ker3;
    std::vector<std::vector<Eigen::MatrixXf>> ker;
  
    ker11 = ArangeRow({4,3}),ker21 = ArangeRow({4,3},{13,13,25}),ker31 = ArangeRow({4,3},{13,26,38});
    ker12 = ArangeRow({4,3},{13,39,51}),ker22 = ArangeRow({4,3},{13,52,64}),ker32 = ArangeRow({4,3},{13,65,77});
    ker13 = ArangeRow({4,3},{13,78,90}),ker23 = ArangeRow({4,3},{13,91,103}),ker33 = ArangeRow({4,3},{13,104,116});
  
    ker1.push_back(ker11),ker1.push_back(ker21),ker1.push_back(ker31);
    ker2.push_back(ker12),ker2.push_back(ker22),ker2.push_back(ker32);
    ker3.push_back(ker13),ker3.push_back(ker23),ker3.push_back(ker33);
    ker.push_back(ker1),ker.push_back(ker2),ker.push_back(ker3);
    
    expected = Eigen::MatrixXf::Zero(36,3);
    expected <<
      1,  39,  78,
      4,  42,  81,
      7,  45,  84,
      10,  48,  87,
      2,  40,  79,
      5,  43,  82,
      8,  46,  85,
      11,  49,  88,
      3,  41,  80,
      6,  44,  83,
      9,  47,  86,
      12,  50,  89,
      13,  52,  91,
      16,  55, 94,
      19,  58,  97,
      22,  61, 100,
      14,  53,  92,
      17,  56,  95,
      20,  59,  98,
      23,  62, 101,
      15,  54,  93,
      18,  57,  96,
      21,  60,  99,
      24,  63, 102,
      26,  65, 104,
      29,  68, 107,
      32,  71, 110,
      35,  74, 113,
      27,  66, 105,
      30,  69, 108,
      33,  72, 111,
      36,  75, 114,
      28,  67, 106,
      31,  70, 109,
      34,  73, 112,
      37,  76, 115;
    
    EXPECT_TRUE(CompareMatrices(Ker2Col(ker),expected));
    
  };  
}

TEST(TestCol2ImBack,DoesEqual){
  {//test1
    Eigen::MatrixXf error,expected0,expected1;
    std::vector<Eigen::MatrixXf> output;
  
    error = ArangeRow({4,8});
    output = Col2ImBack(error,{3,3,2},{2,2,2},{2,2,2,1,1});
    expected0 = Eigen::MatrixXf::Zero(3,3);
    expected0 <<
      1, 20, 19,
      11, 58, 47,
      10, 38, 28;

    expected1 = Eigen::MatrixXf::Zero(3,3);
    expected1 <<
      5, 28, 23,
      19, 74, 55,
      14, 46, 32;

    EXPECT_TRUE(CompareMatrices(output[0],expected0));
    EXPECT_TRUE(CompareMatrices(output[1],expected1));
  };
}
