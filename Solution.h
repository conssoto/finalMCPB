#pragma once
#include <vector>
#include "Truck.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include <iostream>
#include <numeric>

using namespace std;

class Solution {
public:
    vector<int> recollected; //x type
    vector<int> unsatisfiedDemand;
    vector<Truck *> unusedTrucks;
    vector<Node *> unvisitedNodes;

    vector<int> nodesXQuality;

    vector<Route *> routes;

    ProblemInstance *problemInstance;

    Node * plant;
    vector<double> literCost;
    double kilometerCost;

public:
    explicit Solution(ProblemInstance *problemInstance);
    ~Solution();
    Solution(const Solution &s2);

    void updateSolutionAndRoute(Trip *trip, Route *route, bool repairing);
    void updateSolution(Node *node, bool add);
    void resetSolution(const Solution &s2);
    void updateDemands(int currentTypeIndex, Node *currentNode, Route *currentRoute, int production, bool repairing);
    void resetDemands();

    void setQualities(double size);

    void addTruck(Truck *truck);
    void addNode(Node *node);
    void addRoute(int type);
    void addTrip(Trip *trip, Route *route);
    void addBackToPlant(int i, Node *currentNode, Route *currentRoute, bool pos, bool repairing);

    void insertTrip(Route *route, Solution *solution, int index, Node *node);
    void insertNode(Node *node);

    Trip *newTrip(Node *node1, Node *node2, Route *route);
    Trip *fakeTrip(Node *node1, Node *node2, Node *node3, Route *route);

    vector<Route *> getUnfilledRoutes();
    int getUnsatisfiedType();
    Node *getCurrentNode();
    Truck *getNextTruck();
    double getTotalBenefit();
    int getDemandSubtraction(int dda, int prod);

    void removeTruck(Truck *truck);
    void removeNode(Node *node);
    void removeTrip(int tripIndex, Route *route, Solution *solution);

    double calculateBenefit(Trip *trip, int typeIndex);

    void printAll();
};