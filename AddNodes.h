#pragma once
#include <vector>
#include <iostream>
#include <numeric>
#include <cmath>
#include <random>
#include "Truck.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"

class AddNodes {
public:
    vector <Route *> unfilledRoutes;
    Route *currentRoute;
    int currentType;
    bool fix;
    bool stopCriteria;

public:
    explicit AddNodes();
    ~AddNodes();

    Trip *getBestOption(Trip *trip, Route *route, Solution *solution);
    int getInsertPosition(Route *route, Trip *selectedTrip);

    Trip *roulette(Route *route, Solution *solution);
    void changeRouteType(Route *route, Solution *solution);

    void deleteOptions(vector<Trip *> options);

    void nodeAdding(Route *route, Solution *solution, int epsilon);
    void movement(Solution *solution, int epsilon);
};
