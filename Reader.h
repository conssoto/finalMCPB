#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <list>
#include <vector>
#include "ProblemInstance.h"

using namespace std;

class Reader
{
private:
    string filePath;
    ifstream input;

public:
    explicit Reader(string option);
    ~Reader();

    ProblemInstance *readInputFile(double temperature);
    void readOutputs(vector<unsigned int> seeds, size_t beta, int gamma, int epsilon, bool corte);

private:
    void findDef(string def);
    ProblemInstance *createInstance(double temperature);

    void readTrucks(ProblemInstance *problemInstance);
    void readQuality(ProblemInstance *problemInstance);
    void readNodes(ProblemInstance *problemInstance);
    void readDistances(ProblemInstance *problemInstance);
};
