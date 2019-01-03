#include "Trip.h"

Trip::Trip(Node * initialNode, Node * finalNode, int distance): initialNode(initialNode), finalNode(finalNode), distance(distance){}

Trip::~Trip(){
    this->initialNode = nullptr;
    this->finalNode = nullptr;
};

Trip::Trip(const Trip &trip) {
    routeId = trip.routeId;
    distance = trip.distance;
    benefit = trip.benefit;
    initialNode = trip.initialNode;
    finalNode = trip.finalNode;
}

void Trip::setBenefit(double benefit){ this->benefit = benefit;}

void Trip::setRouteId(int id){ this->routeId = id; }

void Trip::printAll() {
    cout << "Trip: " << this->routeId << "\t" << this->initialNode->getId() << " - "
         << this->finalNode->getId() << "\tT: "
         << this->finalNode->getType() << "\tP: " << this->finalNode->getProduction() << "\tD: "
         << this->distance << "\tB: " << this->benefit << endl;
}

bool sortByDistance(const Trip *t1, const Trip *t2){ return t1->distance < t2->distance; }

bool sortByBenefit(const Trip *t1, const Trip *t2){ return t1->benefit > t2->benefit; }
