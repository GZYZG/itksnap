#include "label.h"

unsigned int Label::count = 0;

Label::Label(){
    this->name = "";
    this->r = 0.0;
    this->g = 0.0;
    this->b = 0.0;
    this->opacity = 0.0;
    this->index = count;
    count++;
}

Label::Label(std::string name, float r, float g, float b, float opacity){
    this->name = name;
    this->r = r;
    this->g = g;
    this->b = b;
    this->opacity = opacity;
    this->index = count;
    count++;
}

Label::~Label(){

}

