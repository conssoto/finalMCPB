#pragma once
#include <iostream>
#include <vector>
#include <random>
#include "Trip.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"

using namespace std;

class ReorderNodes {
public:
    double bestDistance;
    int bestIndex;
    Node *node;
    int randomIterations;


public:
    ReorderNodes(int ri = 0);
    ~ReorderNodes();

    void removeTrip(int tripIndex, Route *route, Solution *solution);

    void setBest(Route *route, ProblemInstance *problemInstance);

    void movement(Solution *solution);

    void printRoute(Route *route);
};
