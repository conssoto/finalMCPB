#include <iostream>
#include "ProblemInstance.h"
#include "Solution.h"
#include "Reader.h"
#include "Construction.h"

size_t NEIGHBORHOOD = 5;

int main() {
//    cout << "Enter instance number: ";
//    cin.getline(instanceOption, 3);
//    Reader r(instanceOption);

    Reader r("2");
    srand(3);
    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();
    problemInstance->printAll();

    auto *currentSolution = new Solution(problemInstance);

    auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

    construction->feasibleSolution(currentSolution);
//    currentSolution->printAll();
    double aux(0);
    for (Route *r:currentSolution->routes){
        for(Trip *t: r->trips){
            aux += t->benefit;
        }
    }
    cout << "ben" << aux;


    delete currentSolution;
    delete problemInstance;

    return 0;
}