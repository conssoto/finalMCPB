#include "Truck.h"

Truck::Truck(int id, int totalCapacity): id(id), totalCapacity(totalCapacity) {}

Truck::~Truck() = default;

int Truck::getId() const{ return this->id; }

int Truck::getTotalCapacity() const{ return this->totalCapacity; }

void Truck::printAll(){
    cout << "Truck: " << this->getId() << " TC: " << this->getTotalCapacity() << endl;
}