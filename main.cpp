#include <iostream>
#include "ProblemInstance.h"
#include "Solution.h"
#include "Reader.h"
#include "Construction.h"

size_t NEIGHBORHOOD = 5;
vector<bool> PARAMETERS = {true}; // {cut Neighborhood}

int main() {
//    cout << "Enter instance number: ";
//    cin.getline(instanceOption, 3);
//    Reader r(instanceOption);

    Reader r("2");
    srand(3);

    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();
//    problemInstance->printAll();

    auto *bestSolution = new Solution(problemInstance, PARAMETERS);
    auto *lastSolution = new Solution(problemInstance, PARAMETERS);


    int better(0);
    for (int j =0 ; j <10 ; ++j){



        auto *currentSolution = new Solution(problemInstance, PARAMETERS);
        auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

        construction->feasibleSolution(currentSolution);

        cout<<  "last" << lastSolution->getTotalBenefit();
        cout <<  "\tcurrent" << currentSolution->getTotalBenefit();
        cout <<  "\tbest" << bestSolution->getTotalBenefit()<< endl;

        if (currentSolution->getTotalBenefit() > bestSolution->getTotalBenefit()){
            bestSolution->resetSolution(*currentSolution);
            cout << "new best " << bestSolution->getTotalBenefit() << endl;
            better++;
        }
        else{
            cout << "No ben " << currentSolution->getTotalBenefit() << endl;
        }
        lastSolution->resetSolution(*currentSolution);
        delete currentSolution;
    }

//    currentSolution->printAll();

    cout << "ben " << bestSolution->getTotalBenefit() << " better " << better << endl;
    bestSolution->printAll();


    delete problemInstance;

    return 0;
}