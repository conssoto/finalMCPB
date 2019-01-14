#include <iostream>
#include <random>
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

vector<int> FS_1 = {0, 40, 0, 0, 31, 10, 4, 7, 0, 0, 34, 19, 16, 13, 1, 0, 0, 22, 25, 28, 0, 0, 29, 27, 0, 0, 23, 20, 32, 0, 0, 17, 5, 11, 0};
vector<int> FS_2 = {0, 34, 37, 40, 10, 0, 0, 19, 16, 13, 1, 4, 7, 0, 0, 31, 2, 5, 8, 11, 35, 32, 0, 0, 26, 23, 20, 17, 29, 0, 0, 25, 22, 28, 0};
vector<int> FS_3 = {29, 35, 32, 20, 23, 26, 31, 2, 5, 11, 8, 7, 17, 10, 4, 1, 13, 16, 19, 28, 25, 22, 40, 37};
vector<int> FS_4 = {19, 16, 13, 1, 4, 10, 27, 29, 35, 32, 20, 23, 26, 28, 25, 22, 40, 37,  17, 7, 8, 11, 5, 2, 31};
vector<int> FS_5 = {37, 40, 22, 25, 28, 27, 10, 7, 8, 5, 4, 11, 32, 35, 26, 23, 20, 29, 17, 16, 13, 14, 1, 2, 31};
vector<int> FS_6 = {22, 4, 1, 13, 40, 28, 26, 25, 20, 23, 11, 32, 35, 29, 31, 2, 5, 8, 7, 10};


int main() {
//    cout << "Enter instance number: ";
//    cin.getline(instanceOption, 3);
//    Reader r(instanceOption);

    Reader r("6");
    vector<int> currentList = NODES_6;
    srand(1);


    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile(100.0); // temperature
    problemInstance->printAll();

    auto *bestSolution = new Solution(problemInstance, PARAMETERS);
    auto *lastSolution = new Solution(problemInstance, PARAMETERS);

    auto *reorderNodes = new ReorderNodes(0); // 0->reordena todos, #n reordena n nodos por ruta
    auto *removeNodes = new RemoveNodes();
    auto *addNodes = new AddNodes();

    int better(0);
    int nobetter(0);
    int nofix(0);
    int fix(0);

    int aux(0);
    for(int i: problemInstance->qualities) {
        if (i == 0) {
            aux += 1;
        }
    }


    for (int j =0 ; j <150 ; ++j){
        cout << "---------------------------------------------------------reset " << j << endl;

        auto *currentSolution = new Solution(problemInstance, PARAMETERS);



        auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

        construction->feasibleSolution(currentSolution);

        lastSolution->resetSolution(*currentSolution);
        cout<<  "initial" << currentSolution->getTotalBenefit() << endl;

        cout << "***construccion**"<< endl;
        for (Route *r: currentSolution->routes){
            r->printAll();
        }


        if(construction->construct){

            for (int i =0 ; i <700 ; ++i){
                cout << "----------------iter " << i << " for reset " << j << endl;
//                currentSolution->printAll();



//                if (aux != currentSolution->unsatisfiedDemand.size()){ // si todos son 0, no rompas demandas -> todos los viajes tienen largo cero
                removeNodes->breakDemands(currentSolution);
//                }

//                addNodes->setTabuList(removeNodes->tabuList);
//                addNodes->setTabuList({});

                addNodes->movement(currentSolution);

                if(addNodes->fix) {
                    fix++;
                    cout << "***fix**"<< endl;
                    cout << endl;

                    reorderNodes->movement(currentSolution);

                    removeNodes->movement(currentSolution);

                    if (currentSolution->getTotalBenefit() > bestSolution->getTotalBenefit()) {
                        bestSolution->resetSolution(*currentSolution);

                        cout << "***mejora**"<< endl;
                        cout << endl;
                        cout<< endl;
                        for (Route *r: currentSolution->routes){
                            r->printAll();
                        }
//                        cout << "new best " << bestSolution->getTotalBenefit() << endl;
                        better++;
                    } else {
                        cout << "***no ben**"<< endl;
                        cout << endl;
//                        cout << "No ben " << currentSolution->getTotalBenefit() << endl;
                        nobetter++;
                    }
                    lastSolution->resetSolution(*currentSolution);
                } else{
                    nofix++;
//                    cout << "no fix" << endl;
                    currentSolution->resetSolution(*lastSolution);
                }
                currentSolution->temperature = currentSolution->temperature * 0.99;
            }
        }
        delete currentSolution;

    }

    cout << "******************finish ******************* "<< endl;

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
    for (int i: currentList){
        if (!(find(checklist.begin(), checklist.end(), i) != checklist.end())){ // true si esta presente
            no++;
        }
    }
    cout << "faltan " << no << " nodos"<< endl;
    if(checklist.size() > currentList.size()){
        cout << "sobran " << checklist.size() - currentList.size() << " nodos" << endl;
    }

//    for(int i = 0; i < currentList.size()-1; ++i){
//        if(currentList[i]!=0 or currentList[i+1]!=0){
//            cout << problemInstance->distances[currentList[i]][currentList[i+1]] << endl;
//        }
//    }

    delete removeNodes;
    delete reorderNodes;
    delete lastSolution;
    delete bestSolution;
    delete problemInstance;

    return 0;
}