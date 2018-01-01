#include <iostream>
#include <shared_mutex>
#include "Node.h"

        Node::Node(){
            this->value = -1;
        }

        Node::Node(int value){
            this->value = value;
        }

        Node::~Node(){
            std::cout << "deleting: " << this->value << std::endl;
            
        }

        void Node::doSomething(){
            std::shared_lock<std::shared_mutex> smLk(this->mtex);
            std::cout << "lock created!" << std::endl;
            if(smLk.owns_lock()){
                std::cout << "I am the lock owner!" << std::endl;
            }else{
                std::cout << "noo!" << std::endl;
            //smLk.lock();
            }
            //std::cout << "lock locked!" << std::endl;
            //smLk.unlock();
            //std::cout << "lock unlocked!" << std::endl;
        }

        bool Node::checkLock(){
            std::shared_lock<std::shared_mutex> smLk(this->mtex);
            std::cout << "lock created!" << std::endl;
            if(smLk.owns_lock()){
                std::cout << "I am the lock owner!" << std::endl;
            }else{
                std::cout << "noo!" << std::endl;
            //smLk.lock();
            }
            return false;
        }