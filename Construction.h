#pragma once
#include <iostream>
#include <vector>
#include <random>
#include "Trip.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"

using namespace std;

class Construction{
public:
    size_t alpha;
    int totalProduction;
    vector<Trip *> neighborhood;

    int currentType;
    Node *currentNode;
    Route *currentRoute;
    bool construct;

public:
    explicit Construction(size_t alpha, Solution *solution);
    ~Construction();

    void setNeighborhood(Solution *solution, bool repairing);
    void setTotalProduction();

    bool fitsInTruck(Route *route, Node *node);

    void deleteNeighborhood();
    vector<Trip *> getOptions(Solution *solution, int currentType, Node *currentNode, bool resize);
    Trip *roulette();

    void feasibleSolution(Solution *solution);
    void updateIds(vector<Route *> routes);




    };