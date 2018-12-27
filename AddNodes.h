#pragma once
#include <vector>
#include "Truck.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Solution.h"
#include <iostream>
#include <numeric>

class AddNodes {
public:
    vector<Trip *> neighborhood;
    Node *removedNode;
    double totalBenefit;
    int insertPosition;
    int currentType;
    int benefitOnly;
    bool fix;
    vector <Node *> tabuList;

public:
    explicit AddNodes();
    ~AddNodes();

    void setNeighborhood(Solution *solution, Route *route);
    void setTotalBenefit();
    void setInsertPosition(Route *route, Trip *selectedTrip);
    void setTabuList(vector <Node *> tabuList);

    Trip *roulette();
    Trip *getNeighbor(Solution *solution, Route *currentRoute, Trip *trip);

    bool fitsInTruck(Route *route, Node *node);

    void deleteOptions(vector<Trip *> options, int aux);
    void deleteNeighborhood();

    void nodeAdding(Route *route, Solution *solution);
    void movement(Solution *solution);
};
