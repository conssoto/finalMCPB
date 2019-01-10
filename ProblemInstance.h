#pragma once
#include <iostream>
#include <vector>
#include "Node.h"
#include "Truck.h"

using namespace std;

class ProblemInstance {
public:
    int **distances;

    vector<int> qualities;
    vector<Node *> nodes;
    vector<Truck *> trucks;

    double temperature;

public:
    explicit ProblemInstance(int totalNodes, double temperature);
    ~ProblemInstance();

    int getNumberOfNodes();
    int getNumberOfQualities();
    int getNumberOfTrucks();

    void addNode(Node *node);
    void addTruck(Truck *truck);
    void addQuality(int quality);
    void setDistance(int row, int col, int distance);

    int getDistance(Node *initialNode, Node *finalNode); //calculateDistance

    void sortNodes();

    void printAll();
};
