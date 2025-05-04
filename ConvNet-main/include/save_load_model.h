#ifndef SAVE_LOAD_MODEL_H
#define SAVE_LOAD_MODEL_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <fstream>

#include <Eigen/Dense>
#include <nlohmann/json.hpp>

#include "conv.h"
#include "dense.h"
#include "pooling.h"
#include "network.h"

using json = nlohmann::json;

namespace convnet{
  void SaveNetwork(const Network& network,std::string file_name);
  Network LoadNetwork(std::string file_name);
};//namespace convnet

#endif// SAVE_LOAD_MODEL_H
