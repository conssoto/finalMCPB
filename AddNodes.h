#pragma once
#include <vector>
#include "Truck.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"
#include <iostream>
#include <numeric>
#include <cmath>

class AddNodes {
public:
    vector<bool> neighborhood;
    int insertPosition;
    int currentType;
    bool fix;
    vector <Route *> unfilledRoutes;
    Route *currentRoute;
    bool stopCriteria;

public:
    explicit AddNodes();
    ~AddNodes();

    Trip *roulette(Route *route);
    Trip *getBestOption(Trip *trip, Route *route, Solution *solution);
    int getInsertPosition(Route *route, Trip *selectedTrip);

    void deleteOptions(vector<Trip *> options);

    void nodeAdding(Route *route, Solution *solution);
    void movement(Solution *solution);
};
