#pragma once
#include <iostream>
#include <random>
#include <numeric>
#include <vector>
#include "Truck.h"
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"

using namespace std;

class Solution {
public:
    vector<int> recollected; //x type
    vector<int> unsatisfiedDemand;
    vector<int> newDemands;

    vector<Truck *> unusedTrucks;
    vector<Node *> unvisitedNodes;

    vector<int> nodesXQuality;

    vector<Route *> routes;

    ProblemInstance *problemInstance;

    Node * plant;
    vector<double> literCost;
    double kilometerCost;
    vector<bool> parameters;

    double temperature;

public:
    explicit Solution(ProblemInstance *problemInstance, vector<bool> parameters);
    ~Solution();
    Solution(const Solution &s2);

    double random_number(double min, double max);
    int random_int_number(int min, int max);

    void changeDemands();

    void stepUpdateSolution(Trip *trip, Route *route, bool repairing);
    void updateSolution(Node *node, bool add);
    void resetSolution(const Solution &s2);
    void updateDemands(int currentTypeIndex, Node *currentNode, Route *currentRoute, int production, bool repairing);
    void resetDemands();
    void resetRouteFull();
    void setQualities(double size);

    void addTruck(Truck *truck);
    void addNode(Node *node);
    void addRoute(int type);
    void addTrip(Trip *trip, Route *route);
    void addBackToPlant(int i, Node *currentNode, Route *currentRoute, bool pos, bool repairing);

    void insertTrip(Route *route, int index, Node *node);
    void insertNode(Node *node);

    Trip *newTrip(Node *node1, Node *node2, Route *route);
    Trip *fakeTrip(Node *node1, Node *node2, Node *node3, Route *route);

    vector<Route *> getUnfilledRoutes();
    int getUnsatisfiedType(int from);
    int getCurrentType();
    Node *getCurrentNode();
    Truck *getNextTruck();
    double getTotalBenefit();
    int getDemandSubtraction(int dda, int prod);
    char getType(int i);
    int getBlendingType(int currentRouteType);

    void removeTruck(Truck *truck);
    void removeNode(Node *node);
    void removeTrip(int tripIndex, Route *route);
    void deleteRoutes();

    double calculateBenefit(Trip *trip, int typeIndex);

    void printAll();
    void printSolution();
    vector<int> convertMilk(bool print);

};