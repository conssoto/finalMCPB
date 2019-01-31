#pragma once
#include "Node.h"
#include <iostream>
#include <vector>
#include <memory>

using namespace std;

class Trip {
public:
    int routeId;
    Node *initialNode;
    Node *finalNode;
    double distance;
    double benefit;

public:
    Trip(Node *initialNode, Node *finalNode, int distance);
    ~Trip();
    Trip(const Trip &trip);

    void setRouteId(int id);
    void setBenefit(double benefit);

    void printAll();
};

bool sortByDistance(const Trip *t1, const Trip *t2);
bool sortByBenefit(const Trip *t1, const Trip *t2);
