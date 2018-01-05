#include "node.h"

Node::Node(){
    this->key = -1;
    this->markedToDelele = false;
}

Node::Node(int key){
    this->key = key;
    this->markedToDelele = false;
}