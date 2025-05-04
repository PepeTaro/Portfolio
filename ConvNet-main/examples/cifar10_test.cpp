#include <iostream>
#include <array>
#include <string>
#include <SFML/Graphics.hpp>

#include <Eigen/Dense>
#include "cifar10.hpp"

// CIFARの画像サイズ
const int image_height = 32; 
const int image_width  = 32;

// CIFARの画像をどのくらい拡大するかを決定する変数
const float block_size = 20;

// Windowのサイズ
const int height       = image_height*block_size;
const int width        = image_width*block_size;

std::string getImageName(int label_idx){
  switch(label_idx){
  case 0:
    return "Airplane";
    break;
  case 1:
    return "Automobile";
    break;
  case 2:
    return "Bird";
    break;
  case 3:
    return "Cat";
    break;
  case 4:
    return "Deer";
    break;
  case 5:
    return "Dog";
    break;
  case 6:
    return "Frog";
    break;
  case 7:
    return "Horse";
    break;
  case 8:
    return "Ship";
    break;
  case 9:
    return "Truck";
    break;
  default:
    return "None";
    break;
  }  
}

void Draw(sf::RenderWindow& window,int h_pos,int w_pos,const std::array<float,3>& rgb){  
    float h,w;
    
    h = block_size*h_pos;
    w = block_size*w_pos;
    
    sf::RectangleShape rectangle(sf::Vector2f(block_size,block_size));
    rectangle.setPosition(sf::Vector2f(w,h));
    rectangle.setFillColor(sf::Color((int)255*rgb[0],(int)255*rgb[1],(int)255*rgb[2]));
    
    window.draw(rectangle);
}

void DrawImage(sf::RenderWindow& window,const std::vector<Eigen::MatrixXf>& image){
  int rows = image[0].rows();
  int cols = image[0].cols();
  
  for(int i=0;i<rows;++i){
    for(int j=0;j<cols;++j){
      Draw(window,i,j,{image[0](i,j),image[1](i,j),image[2](i,j)});
    }
  }
}
void PrintLabel(const std::vector<Eigen::MatrixXf>& training_labels,int image_index){
  int correct_index;
  
  static_cast<Eigen::VectorXf>(training_labels[image_index]).maxCoeff(&correct_index);
  printf("\r[%05d番目の画像] ラベル:%10s",image_index,getImageName(correct_index).c_str()),fflush(stdout);
}

void Run(const std::vector<std::vector<Eigen::MatrixXf>>& data,const std::vector<Eigen::MatrixXf>& labels){
  int data_size = data.size();
  
  sf::RenderWindow window(sf::VideoMode(image_height*block_size,image_width*block_size), "CIFAR10");
  int image_index = 0;

  printf("[使用方法]\n *)左アローキーを押して,一つ前の画像を表示\n *)右アローキーを押して,次の画像を表示\n\n"),fflush(stdout);
  PrintLabel(labels,image_index);

  while (window.isOpen()){    
    sf::Event event;
    while (window.pollEvent(event)){
      switch(event.type){	
      case sf::Event::Closed:
	window.close();
	break;	
      case sf::Event::KeyPressed:	
	if(event.key.code == sf::Keyboard::Left){
	  if(image_index > 0){
	    --image_index;
	    PrintLabel(labels,image_index);
	  }	  
	}else if(event.key.code == sf::Keyboard::Right){
	  if(image_index < data_size-1){
	    ++image_index;
	    PrintLabel(labels,image_index);	    	    
	  }	  	  
	}
	break;	
      }
    }
    
    window.clear(sf::Color::Black);
    DrawImage(window,data[image_index]);    
    window.display();  
  }

}

bool UseTrainingData(){
  std::string reply;
  
  while(true){
    std::cout << "トレーニングデータ(0),テストデータ(1) どちらを表示しますか?(0 or 1):";
    std::cin >> reply;
    if(reply != "0" and reply != "1"){
      std::cout << "'0' または '1' と入力してください" << std::endl;
    }else{
      break;
    }
  }

  if(reply == "0"){
    std::cout << "トレーニングデータを表示します" << std::endl;
    return true;
  }else{
    std::cout << "テストデータを表示します" << std::endl;
    return false;
  }
}

int main(){  
  int training_data_size = 50000;
  int test_data_size = 10000;
  // 外部モジュールを使用して、CIFARデータセットを読み込み。
  auto dataset = cifar::read_dataset<std::vector, std::vector, uint8_t, uint8_t>();
  
  bool use_training_data = UseTrainingData();
    
  if(use_training_data){
    std::vector<std::vector<Eigen::MatrixXf>> training_data;
    std::vector<Eigen::MatrixXf> training_labels;    
    PreprocessTrainingData(dataset,training_data,training_labels,training_data_size);
    Run(training_data,training_labels);
  }else{
    std::vector<std::vector<Eigen::MatrixXf>> test_data;
    std::vector<Eigen::MatrixXf> test_labels;
    PreprocessTestData(dataset,test_data,test_labels,test_data_size);
    Run(test_data,test_labels);
  }
}
