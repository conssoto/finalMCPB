#include "ProblemInstance.h"

ProblemInstance::ProblemInstance(int totalNodes){
    this->distances = new int *[totalNodes];
    for (int i = 0; i < totalNodes; ++i){
        this->distances[i] = new int[totalNodes];
    }
}

ProblemInstance::~ProblemInstance(){
    cout << "Deleting Problem Instance" << endl;

    for (int i = 0; i < this->getNumberOfNodes(); ++i){
        delete[] this->distances[i];
    }
    delete[] distances;

    for (Truck *truck: this->trucks){
        delete truck;
    }
    this->trucks.clear();
    this->trucks.shrink_to_fit();

    for (Node *node: this->nodes){
        delete node;
    }
    this->nodes.clear();
    this->nodes.shrink_to_fit();

    this->qualities.clear();
    this->qualities.shrink_to_fit();
}

int ProblemInstance::getNumberOfNodes(){ return (int)this->nodes.size(); }

int ProblemInstance::getNumberOfQualities(){ return (int)this->qualities.size(); }

int ProblemInstance::getNumberOfTrucks(){ return (int)this->qualities.size(); }


void ProblemInstance::addNode(Node *node){ this->nodes.push_back(node); }

void ProblemInstance::addTruck(Truck *truck){ this->trucks.push_back(truck); }

void ProblemInstance::addQuality(int quality){ this->qualities.push_back(quality); }

void ProblemInstance::setDistance(int row, int col, int distance) { this->distances[row][col] = distance; }


int ProblemInstance::getDistance(Node *initialNode, Node *finalNode) {
    return this->distances[initialNode->getId()][finalNode->getId()];
}

void ProblemInstance::sortNodes() {
    sort(nodes.begin(), nodes.end(), sortByType);
}


void ProblemInstance::printAll(){
    cout << endl;
    cout << "**Problem Instance**" << endl;
    for(double q: this->qualities){
        cout << "Quality: " << q << endl;
    }
    for(Node *n: this->nodes){
        n->printAll();
    }
    for(Truck *t: this->trucks){
        t->printAll();
    }

    cout << "Matrix[0][0]: " << this->distances[0][0] << endl; //matriz con diagonal de ceros
}

