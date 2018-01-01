#include <iostream>
#include "NodeList.h"

        NodeList::NodeList(){
            this->value = -1;
        }

        NodeList::NodeList(int value){
            this->value = value;
        }

        NodeList::~NodeList(){
            std::cout << "deleting: " << this->value << std::endl;
            
        }