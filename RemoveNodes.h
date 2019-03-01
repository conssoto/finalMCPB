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

public:
    explicit RemoveNodes();
    ~RemoveNodes();

    void setDeletable(Route *route, Solution *solution);
    void setTotalBenefit();

    int getCuttingDemand(int typeIndex, Solution *solution);
    double getDeletableBenefit(Trip *deletableTrip, Trip *nextTrip, Route *route, Solution *solution);

    int roulette(Solution *solution);
    void breakDemands(Solution *solution, int gamma);
    void movement(Solution *solution);
};
