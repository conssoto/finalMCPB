#pragma once
#include <vector>
#include "Truck.h"
#include "Trip.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"
#include <iostream>
#include <numeric>

class RemoveNodes {
public:
    vector<int> deletableTripsIndex;
    vector<double> benefits;
    double totalBenefit;
    vector<Node *> tabuList;

public:
    explicit RemoveNodes();
    ~RemoveNodes();

    void setDeletable(Route *route, Solution *solution);
    void setTotalBenefit();

    int getCuttingDemand(int typeIndex, Solution *solution);

    int roulette();
    void breakDemands(Solution *solution);
    void movement(Solution *solution);
};
