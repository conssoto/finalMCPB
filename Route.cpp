#include "Route.h"

Route::Route(int id, Truck *truck, int type):
id(id), truck(truck), type(type), remainingCapacity(truck->getTotalCapacity()), full(false), distance(0){}

Route::~Route() {
    this->truck = nullptr;

    for (Trip *trip: this->trips) {
        delete trip;
    }
    this->trips.clear();
    this->trips.shrink_to_fit();
}

Route::Route(const Route &route){
    id = route.id;
    truck = route.truck;
    remainingCapacity = route.remainingCapacity;
    type = route.type;
    full = route.full;
    distance = route.distance;

    for(Trip* trip: route.trips){
        auto *copy = new Trip(*trip);
        trips.push_back(copy);
    }

}

int Route::getId(){ return this->id; }

int Route::getTypeIndex(){ return this->type - 1; }

int Route::getType(){ return this->type; }

bool Route::isFull(){ return this->full; }

void Route::setFull() { this->full = true; }


void Route::printAll() {
    double aux(0);
    for(Trip *trip: this->trips){
        aux += trip->benefit;
    }
    cout << "Route: " << this->getId() << "\ttruck: " << this->truck->getId() << "\tmilk type: " << this->type << "\tisFull: "
         << this->full << "\ttotal benefit: " << aux << endl;
    cout << "Num. of trips: " << this->trips.size() << "\tremaining Capacity: " << this->remainingCapacity << "\tdistance: " << this->distance << endl;
    for(Trip *trip: this->trips){
        trip->printAll();
    }
}
