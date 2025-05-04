#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <SFML/Graphics.hpp>

#include "network.h"
#include "save_load_model.h"
#include "mnist.hpp"

using namespace convnet;

// MNISTの画像サイズ
const int image_height = 28; 
const int image_width  = 28;

// MNISTの画像をどのくらい拡大するかを決定する変数
const float block_size = 20;

// Windowのサイズ
const int height       = image_height*block_size;
const int width        = image_width*block_size;

// h_pos,w_posの位置にcolor_strength(0.0から1.0, 1.0が白, 0.0が黒)の"強度"で,白黒ピクセルを描写
void Draw(sf::RenderWindow& window,int h_pos,int w_pos,float color_strength){  
    float h,w;
    
    h = block_size*h_pos;
    w = block_size*w_pos;
    
    sf::RectangleShape rectangle(sf::Vector2f(block_size,block_size));
    rectangle.setPosition(sf::Vector2f(w,h));
    rectangle.setFillColor(sf::Color((int)255*color_strength,(int)255*color_strength,(int)255*color_strength));
    
    window.draw(rectangle);
}

// 描写した絵が格納されている行列imageをConvNetに"流し",判定
void Output(Network& network,const Eigen::MatrixXf& image){
  float prob;
  int prediction;
  std::vector<Eigen::MatrixXf> inputs;
  inputs.push_back(image);

  Eigen::MatrixXf output = network.Net(inputs);
  prob = Softmax(output).maxCoeff(&prediction);
  printf("\r判定結果:%d(確率:%7.3f%%)",prediction,(prob*100.0)),fflush(stdout);
}

void DrawMe(Network& network){
  sf::RenderWindow window(sf::VideoMode(height,width), "Draw me");
  Eigen::MatrixXf drawing_data(image_height,image_width); // 描写した絵を保存するための28x28行列(これがConvNetのインプットとなる)
  
  bool is_mouse_button_pressed=false; // マウスボタンが,現在押されているか否かを保存する変数 
  bool is_start_drawing = false;     //  描写中か否かを保存する変数
  
  sf::Vector2i pixelPos;  // 取得したピクセルの位置を保存
  sf::Event event;
  drawing_data.setZero();
  
  std::cout << "[使用方法]\nマウスボタンを押して,数字(0から9)を描写したあと,スペースキーを押下して数字を判定" << std::endl;
  while (window.isOpen()){    
    while (window.pollEvent(event)){
      switch(event.type){
	
      case sf::Event::Closed:
	window.close();
	break;
	
      case sf::Event::KeyPressed:// Enter or Spaceキーで判定
        if(event.key.code == sf::Keyboard::Return or event.key.code == sf::Keyboard::Space){
	  window.clear(sf::Color::Black); // Windowをクリア
	  Output(network,drawing_data); //判定
	  
	  drawing_data.setZero();       // 行列をクリア
	  is_mouse_button_pressed = false;
	  is_start_drawing = false;
	}
	break;
	
      case sf::Event::MouseButtonPressed: // マウスボタンが押された場合
        pixelPos = sf::Mouse::getPosition(window);
	is_mouse_button_pressed = true;
	is_start_drawing = true;
	break;
	
      case sf::Event::MouseButtonReleased: // マウスボタンが離された場合
	pixelPos = sf::Mouse::getPosition(window);	
	is_mouse_button_pressed = false;	
	break;
	
      }
    }
    
    if(is_mouse_button_pressed){ // マウスボタンを押している間,描写
      pixelPos = sf::Mouse::getPosition(window);	    
    } 
    
    // カーソルの位置が,Window内か否かチェック
    if(is_start_drawing and
       0 <= pixelPos.x and pixelPos.x < width and
       0 <= pixelPos.y and pixelPos.y < height){
      
      int row,col;
      // Window内のカーソルの位置を28x28"行列"内の点に変換
      row = floor(1.0*pixelPos.y/block_size);
      col = floor(1.0*pixelPos.x/block_size);
      
      Draw(window,row,col,1.0);    // windowに描写 
      drawing_data(row,col) = 1.0; // 行列に保存 

      // 以下のコードは,描写の"ペンの太さ"をワンサイズ上げている(さもなければ,細すぎるため)
      if(row > 0){
	Draw(window,row-1,col,1.0);
	drawing_data(row-1,col) = 1.0;
      }      
      if(col > 0){
	Draw(window,row,col-1,1.0);
	drawing_data(row,col-1) = 1.0;
      }      
      if(row+1 < 28){
	Draw(window,row+1,col,1.0);
	drawing_data(row+1,col) = 1.0;
      }      
      if(col+1 < 28){
	Draw(window,row,col+1,1.0);
	drawing_data(row,col+1) = 1.0;
      }
      
      if(row > 0 and col > 0){
	Draw(window,row-1,col-1,1.0);
	drawing_data(row-1,col-1) = 1.0;
      }            
      if(row+1 < 28 and col > 0){
	Draw(window,row+1,col-1,1.0);
	drawing_data(row+1,col-1) = 1.0;
      }      
      if(row > 0 and col+1 < 28){
	Draw(window,row-1,col+1,1.0);
	drawing_data(row-1,col+1) = 1.0;
      }      
      if(row+1 < 28 and col+1 < 28){
	Draw(window,row+1,col+1,1.0);
	drawing_data(row+1,col+1) = 1.0;
      }
    }
    
    window.display();  
  }
}

bool isFileExist(const std::string& filename){
    std::ifstream infile(filename);
    return infile.good();
}

std::string SelectModel(){
  std::string filename;
  while(true){
    std::cout << "使用したいモデルが保存されているファイルのパスを入力してください(例 ../models/adamw_lenet5.json):" << std::flush;
    std::cin >> filename;
    if(isFileExist(filename)){
      break;
    }else{
      std::cout << "[!] Error そのようなファイルは存在しません" << std::endl;
    }
  }
  return filename;
}

int main(int argc,char **argv){
  std::string filename = SelectModel();
  //std::string filename = "../models/adamw_lenet5.json";
  Network network = LoadNetwork(filename.c_str());
  DrawMe(network);
}
