#include <iostream>
#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "optimizer.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestType,DoesEqual){
  optimizers::Sgd sgd(10,2,1e-1,1e-1,0.9);
  EXPECT_TRUE(sgd.getType() == optimizers::optimizer_type::Sgd);

  optimizers::Adam adam(10,2,1e-1);
  EXPECT_TRUE(adam.getType() == optimizers::optimizer_type::Adam);
  
  optimizers::AdamW adamw(10,2,1e-1,1e-2);
  EXPECT_TRUE(adamw.getType() == optimizers::optimizer_type::AdamW);
}

TEST(TestTimeStep,DoesEqual){
  optimizers::Sgd sgd(10,2,1e-1,1e-1,0.9);
  EXPECT_TRUE(sgd.getTimeStep() == 0);
  
  sgd.IncrementTimeStep();
  EXPECT_TRUE(sgd.getTimeStep() == 1);

  sgd.IncrementTimeStep();
  EXPECT_TRUE(sgd.getTimeStep() == 2);

  sgd.IncrementTimeStep();
  EXPECT_TRUE(sgd.getTimeStep() == 3);
}

TEST(TestBatchCount,DoesEqual){
  optimizers::Sgd sgd(6,2,1e-1,1e-1,0.9);
  EXPECT_TRUE(sgd.getBatchCount() == 1);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == false);
  
  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 2);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == false);

  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 3);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == true);

  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 4);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == false);

  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 5);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == true);

  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 6);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == false);

  sgd.IncrementBatchCount();
  EXPECT_TRUE(sgd.getBatchCount() == 1);
  EXPECT_TRUE(sgd.getMinibatchUpdate() == true);
}
