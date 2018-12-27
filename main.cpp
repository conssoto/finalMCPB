#include <iostream>
#include "ProblemInstance.h"
#include "Solution.h"
#include "Reader.h"
#include "Construction.h"
#include "ReorderNodes.h"
#include "RemoveNodes.h"
#include "AddNodes.h"

size_t NEIGHBORHOOD = 5;
vector<bool> PARAMETERS = {true}; // {cut Neighborhood}

int main() {
//    cout << "Enter instance number: ";
//    cin.getline(instanceOption, 3);
//    Reader r(instanceOption);

    Reader r("2");
    srand(1);

    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();
    problemInstance->printAll();

    auto *bestSolution = new Solution(problemInstance, PARAMETERS);
    auto *lastSolution = new Solution(problemInstance, PARAMETERS);

    auto *reorderNodes = new ReorderNodes();
    auto *removeNodes = new RemoveNodes();
    auto *addNodes = new AddNodes();

    int better(0);
    for (int j =0 ; j <200 ; ++j){
        cout << "----------------reset " << j << endl;

        auto *currentSolution = new Solution(problemInstance, PARAMETERS);
        auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

        construction->feasibleSolution(currentSolution);


        lastSolution->resetSolution(*currentSolution);
        cout<<  "initial" << currentSolution->getTotalBenefit() << endl;
//        cout <<  "\tbest" << bestSolution->getTotalBenefit()<< endl;

        if(construction->construct){
            for (int i =0 ; i <100 ; ++i){
                cout << "----------------iter " << i << endl;

                cout << "antes" << currentSolution->getTotalBenefit() << endl;

                removeNodes->breakDemands(currentSolution);
                cout << "dsp break" << currentSolution->getTotalBenefit() << endl;

                addNodes->setTabuList(removeNodes->tabuList);

                addNodes->movement(currentSolution);
                cout << "dsp add" << currentSolution->getTotalBenefit() << endl;

                if(addNodes->fix) {
                    removeNodes->movement(currentSolution);
                    cout << "dsp remove" << currentSolution->getTotalBenefit() << endl;

                    reorderNodes->movement(currentSolution);
                    cout << "dsp reorder" << currentSolution->getTotalBenefit() << endl;

                    cout << "best" << bestSolution->getTotalBenefit() << endl;
                    if (currentSolution->getTotalBenefit() > bestSolution->getTotalBenefit()) {
                        bestSolution->resetSolution(*currentSolution);
                        cout << "new best " << bestSolution->getTotalBenefit() << endl;
                        better++;
                    } else {
                        cout << "No ben " << currentSolution->getTotalBenefit() << endl;
                    }
                    lastSolution->resetSolution(*currentSolution);
                }

                else{

                    cout << "vuelve atras" << endl;
                    currentSolution->resetSolution(*lastSolution);
                }
            }
        }

        delete currentSolution;

    }

//    currentSolution->printAll();

    cout << "ben " << bestSolution->getTotalBenefit() << " better " << better << endl;
    bestSolution->printAll();

    bestSolution->printSolution();

    delete removeNodes;
    delete reorderNodes;
    delete lastSolution;
    delete bestSolution;
    delete problemInstance;

    return 0;
}