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
vector<int> NODES_1 = {40, 31, 10, 4, 7, 34, 19, 16, 13, 1, 22, 25, 28, 29, 27, 23, 20, 32, 17, 5, 11};
vector<int> NODES_2 = {34, 37, 40, 10,  19, 16, 13, 1, 4, 7, 31, 2, 5, 8, 11, 35, 32, 26, 23, 20, 17, 29, 25, 22, 28};
vector<int> NODES_3 = {29, 35, 32, 20, 23, 26, 31, 2, 5, 11, 8, 7, 17, 10, 4, 1, 13, 16, 19, 28, 25, 22, 40, 37};
vector<int> NODES_4 = {19, 16, 13, 1, 4, 10, 27, 29, 35, 32, 20, 23, 26, 28, 25, 22, 40, 37,  17, 7, 8, 11, 5, 2, 31};
vector<int> NODES_5 = {37, 40, 22, 25, 28, 27, 10, 7, 8, 5, 4, 11, 32, 35, 26, 23, 20, 29, 17, 16, 13, 14, 1, 2, 31};
vector<int> NODES_6 = {22, 4, 1, 13, 40, 28, 26, 25, 20, 23, 11, 32, 35, 29, 31, 2, 5, 8, 7, 10};


int main() {
//    cout << "Enter instance number: ";
//    cin.getline(instanceOption, 3);
//    Reader r(instanceOption);

    Reader r("6");
    srand(1);

    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();
    problemInstance->printAll();

    auto *bestSolution = new Solution(problemInstance, PARAMETERS);
    auto *lastSolution = new Solution(problemInstance, PARAMETERS);

    auto *reorderNodes = new ReorderNodes(0);
    auto *removeNodes = new RemoveNodes();
    auto *addNodes = new AddNodes();

    int better(0);
    int nobetter(0);
    int nofix(0);
    int fix(0);

    for (int j =0 ; j <700 ; ++j){
        cout << "----------------reset " << j << endl;

        auto *currentSolution = new Solution(problemInstance, PARAMETERS);
        auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

        construction->feasibleSolution(currentSolution);


        lastSolution->resetSolution(*currentSolution);
        cout<<  "initial" << currentSolution->getTotalBenefit() << endl;
//        cout <<  "\tbest" << bestSolution->getTotalBenefit()<< endl;

        if(construction->construct){
            for (int i =0 ; i <150 ; ++i){
                cout << "----------------iter " << i << endl;

                cout << "antes" << currentSolution->getTotalBenefit() << endl;

                removeNodes->breakDemands(currentSolution);
                cout << "dsp break" << currentSolution->getTotalBenefit() << endl;

//                addNodes->setTabuList(removeNodes->tabuList);
                addNodes->setTabuList({});


                addNodes->movement(currentSolution);
                cout << "dsp add" << currentSolution->getTotalBenefit() << endl;

                if(addNodes->fix) {
                    removeNodes->movement(currentSolution);
                    cout << "dsp remove" << currentSolution->getTotalBenefit() << endl;

                    reorderNodes->movement(currentSolution);
                    cout << "dsp reorder" << currentSolution->getTotalBenefit() << endl;

                    cout << "best" << bestSolution->getTotalBenefit() << endl;
                    fix++;
                    if (currentSolution->getTotalBenefit() > bestSolution->getTotalBenefit()) {
                        bestSolution->resetSolution(*currentSolution);
                        cout << "new best " << bestSolution->getTotalBenefit() << endl;
                        better++;
                    } else {
                        cout << "No ben " << currentSolution->getTotalBenefit() << endl;
                        nobetter++;
                    }
                    lastSolution->resetSolution(*currentSolution);
                }

                else{
                    nofix++;
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

    cout << "better " << better << endl;
    cout << "fix " << fix << endl;
    cout << "no better " << nobetter<< endl;
    cout << "no fix " << nofix << endl;

    vector<int> checklist;
    for(Route *ro:bestSolution->routes){
        for(Trip *t: ro->trips){
            if(t->finalNode->getId() != 0){
                checklist.push_back(t->finalNode->getId());
            }
        }
    }
    int no(0);
    for (int i: NODES_6){
        if (!(find(checklist.begin(), checklist.end(), i) != checklist.end())){ // true si esta presente
            no++;
        }
    }
    cout << "faltan " << no << " nodos"<< endl;
    if(checklist.size() > NODES_6.size()){
        cout << "sobran " << checklist.size() - NODES_6.size() << " nodos" << endl;
    }

    delete removeNodes;
    delete reorderNodes;
    delete lastSolution;
    delete bestSolution;
    delete problemInstance;

    return 0;
}