#pragma once
#include "Truck.h"
#include "Trip.h"
#include <vector>
#include <iostream>

using namespace std;

class Route {
public:
    int id;
    Truck *truck;
    vector<Trip *> trips;
    int remainingCapacity;
    int type;
    bool full;
    double distance;


public:
    Route(int id, Truck *truck, int type);
    ~Route();
    Route(const Route &route);

    int getId();
    int getTypeIndex();
    int getType();
    bool isFull();

    void setFull();
    bool fitsInTruck(Node *node);

    void printAll();
};