#include <iostream>
#include <Eigen/Dense>
#include "gtest/gtest.h"

#include "network.h"
#include "utils_for_test.h"

using namespace convnet;

TEST(TestLossType,DoesEqual){
  Network network1({28,28,1},new optimizers::AdamW(60000,32,1e-4,1e-1),Losses::CategoricalCrossEntropy);  
  network1.addConv({5,5,6},{1,1},{2,2},Activations::LeakyRelu);  
  network1.addPooling({2,2},{2,2});
  network1.addConv({5,5,16},{1,1},{0,0},Activations::LeakyRelu);
  network1.addPooling({2,2},{2,2});
  network1.addDense(120,Activations::LeakyRelu);
  network1.addDense(84,Activations::LeakyRelu);
  network1.addDense(10);  
  EXPECT_TRUE(network1.getLossType() == Losses::CategoricalCrossEntropy);

  Network network2({28,28,1},new optimizers::Adam(60000,32,1e-4),Losses::Mse);      
  network2.addDense(120,Activations::LeakyRelu);
  network2.addDense(84,Activations::LeakyRelu);
  network2.addDense(10);
  EXPECT_TRUE(network2.getLossType() == Losses::Mse);  
}

TEST(TestLayerTypes,DoesEqual){
  Network network1({28,28,1},new optimizers::AdamW(60000,32,1e-4,1e-1),Losses::CategoricalCrossEntropy);  
  network1.addConv({5,5,6},{1,1},{2,2},Activations::LeakyRelu);  
  network1.addPooling({2,2},{2,2});
  network1.addConv({5,5,16},{1,1},{0,0},Activations::LeakyRelu);
  network1.addPooling({2,2},{2,2});
  network1.addDense(120,Activations::LeakyRelu);
  network1.addDense(84,Activations::LeakyRelu);
  network1.addDense(10);
  {
    std::vector<LayerType> layer_types =  network1.getLayerTypes();
    EXPECT_TRUE(layer_types.size() == 7);
    EXPECT_TRUE(layer_types[0] == LayerType::Conv);
    EXPECT_TRUE(layer_types[1] == LayerType::Pooling);
    EXPECT_TRUE(layer_types[2] == LayerType::Conv);
    EXPECT_TRUE(layer_types[3] == LayerType::Pooling);
    EXPECT_TRUE(layer_types[4] == LayerType::Dense);
    EXPECT_TRUE(layer_types[5] == LayerType::Dense);
    EXPECT_TRUE(layer_types[6] == LayerType::Dense);
  }
  
  
  Network network2({28,28,1},new optimizers::Adam(60000,32,1e-4),Losses::Mse);      
  network2.addDense(120,Activations::LeakyRelu);
  network2.addDense(84,Activations::LeakyRelu);
  network2.addDense(10);
  {
    std::vector<LayerType> layer_types =  network2.getLayerTypes();
    EXPECT_TRUE(layer_types.size() == 3);        
    EXPECT_TRUE(layer_types[0] == LayerType::Dense);
    EXPECT_TRUE(layer_types[1] == LayerType::Dense);
    EXPECT_TRUE(layer_types[2] == LayerType::Dense);
  }
}

TEST(TestOutputShapes,DoesEqual){
  Network network1({28,28,1},new optimizers::AdamW(60000,32,1e-4,1e-1),Losses::CategoricalCrossEntropy);  
  network1.addConv({5,5,6},{1,1},{2,2},Activations::LeakyRelu);  
  network1.addPooling({2,2},{2,2});
  network1.addConv({5,5,16},{1,1},{0,0},Activations::LeakyRelu);
  network1.addPooling({2,2},{2,2});
  network1.addDense(120,Activations::LeakyRelu);
  network1.addDense(84,Activations::LeakyRelu);
  network1.addDense(10);
  {
    std::vector<std::array<int,3>> output_shapes =  network1.getOutputShapes();
    EXPECT_TRUE(output_shapes.size() == 8);
    EXPECT_TRUE(ComapreArrays3(output_shapes[0],{28,28,1}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[1],{28,28,6}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[2],{14,14,6}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[3],{10,10,16}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[4],{5,5,16}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[5],{120,-1,-1}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[6],{84,-1,-1}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[7],{10,-1,-1}));    
  }
  
  Network network2({28,28,1},new optimizers::Adam(60000,32,1e-4),Losses::Mse);      
  network2.addDense(120,Activations::LeakyRelu);
  network2.addDense(84,Activations::LeakyRelu);
  network2.addDense(10);
  {
    std::vector<std::array<int,3>> output_shapes =  network2.getOutputShapes();
    EXPECT_TRUE(output_shapes.size() == 4);
    EXPECT_TRUE(ComapreArrays3(output_shapes[0],{28,28,1}));        
    EXPECT_TRUE(ComapreArrays3(output_shapes[1],{120,-1,-1}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[2],{84,-1,-1}));
    EXPECT_TRUE(ComapreArrays3(output_shapes[3],{10,-1,-1}));
  }
  
}
