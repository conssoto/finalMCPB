#include <iostream>
#include <random>
#include <time.h>
#include "ProblemInstance.h"
#include "Solution.h"
#include "Reader.h"
#include "Construction.h"
#include "ReorderNodes.h"
#include "RemoveNodes.h"
#include "AddNodes.h"

size_t NEIGHBORHOOD = 5;// 1008, 1505, 494, 15, 937, 1801, 522, 7, 24, 116, 9574, 7121, 803, 347, 3795, 672, 49, 2647, 223, 236
vector<unsigned int> SEEEDS = {347}; // {cut Neighborhood}
vector<int> NODES_1 = {40, 31, 10, 4, 7, 34, 19, 16, 13, 1, 22, 25, 28, 29, 27, 23, 20, 32, 17, 5, 11};
vector<int> NODES_2 = {34, 37, 40, 10,  19, 16, 13, 1, 4, 7, 31, 2, 5, 8, 11, 35, 32, 26, 23, 20, 17, 29, 25, 22, 28};
vector<int> NODES_3 = {29, 35, 32, 20, 23, 26, 31, 2, 5, 11, 8, 7, 17, 10, 4, 1, 13, 16, 19, 28, 25, 22, 40, 37};
vector<int> NODES_4 = {19, 16, 13, 1, 4, 10, 27, 29, 35, 32, 20, 23, 26, 28, 25, 22, 40, 37,  17, 7, 8, 11, 5, 2, 31};
vector<int> NODES_5 = {37, 40, 22, 25, 28, 27, 10, 7, 8, 5, 4, 11, 32, 35, 26, 23, 20, 29, 17, 16, 13, 14, 1, 2, 31};
vector<int> NODES_6 = {22, 4, 1, 13, 40, 28, 26, 25, 20, 23, 11, 32, 35, 29, 31, 2, 5, 8, 7, 10};

vector<int> FS_1 = {0, 40, 0, 0, 31, 10, 4, 7, 0, 0, 34, 19, 16, 13, 1, 0, 0, 22, 25, 28, 0, 0, 29, 27, 0, 0, 23, 20, 32, 0, 0, 17, 5, 11, 0};
vector<int> FS_2 = {0, 34, 37, 40, 10, 0, 0, 9, 16, 13, 1, 4, 7, 0, 0, 31, 2, 5, 8, 11, 35, 32, 0, 0, 26, 23, 20, 17, 29, 0, 0, 25, 22, 28, 0};
vector<int> FS_3 = {29, 35, 32, 20, 23, 26, 31, 2, 5, 11, 8, 7, 17, 10, 4, 1, 13, 16, 19, 28, 25, 22, 40, 37};
vector<int> FS_4 = {19, 16, 13, 1, 4, 10, 27, 29, 35, 32, 20, 23, 26, 28, 25, 22, 40, 37,  17, 7, 8, 11, 5, 2, 31};
vector<int> FS_5 = {0, 37, 40, 22, 25, 28, 0, 0, 27, 0, 0, 10, 7, 8, 5, 4, 11, 0, 0, 32, 35, 26, 23, 20, 29, 0, 0, 17, 16, 13, 14, 1, 2, 31, 0};
vector<int> FS_6 = {0, 22, 4, 1, 13, 0, 0, 40, 0, 0, 28, 26, 25, 20, 23, 0, 0, 11, 32, 35, 29, 0, 0, 31, 2, 5, 8, 7, 10, 0};


int main() {

    ///GENERAR RESUMEN
//    Reader r(to_string(3));
//    r.readOutputs(SEEEDS);
    ////--------------

    clock_t total0, total1;
    total0=clock();

    for (int instance = 2; instance < 3; ++instance) {
        for (unsigned int seed: SEEEDS) {
            srand(seed);

            clock_t t0, t1, t2, timeToBest;

            t0 = clock();
            Reader r(to_string(instance));
            vector<int> currentList = NODES_6;

            cout << "For instance " << instance << " seed " << seed << endl;
            ProblemInstance *problemInstance;

            problemInstance = r.readInputFile(100.0); // temperature

            auto *bestSolution = new Solution(problemInstance, seed);
            auto *lastSolution = new Solution(problemInstance, seed);

            auto *reorderNodes = new ReorderNodes(0); // 0->reordena todos, #n reordena n nodos por ruta
            auto *removeNodes = new RemoveNodes();
            auto *addNodes = new AddNodes();

            int better(0);
            int nobetter(0);
            int nofix(0);
            int fix(0);
            int newD(0);

            int aux(0);
            for (int i: problemInstance->qualities) {
                if (i == 0) {
                    aux += 1;
                }
            }

            bool doBreak(false);

            for (int j = 0; j < 2000; ++j) {
                cout << "----------------------------->Reset " << j << endl;

                auto *currentSolution = new Solution(problemInstance, seed);

                if (newD == 5 or newD == 0) {
                    if (aux == problemInstance->getNumberOfQualities()) {
                        currentSolution->changeDemands();
                        newD = 1;
                        cout << "new demands: "<< endl;
                        for (int i: currentSolution->unsatisfiedDemand){
                            cout << i << endl;
                        }
                    }
                } else {
                    currentSolution->unsatisfiedDemand = lastSolution->unsatisfiedDemand;
                    currentSolution->newDemands = lastSolution->newDemands;
                    newD += 1;
                }


                auto *construction = new Construction(NEIGHBORHOOD, currentSolution);

                construction->feasibleSolution(currentSolution);

                lastSolution->resetSolution(*currentSolution);

                cout << "***construccion**" << endl;

                if (construction->construct) {
                    for (int i = 0; i < 2000; ++i) {
//                        cout << "Iter " << i << " for reset " << j << endl;

                        removeNodes->breakDemands(currentSolution);

                        addNodes->movement(currentSolution);

                        if (addNodes->fix) {
                            fix++;

                            reorderNodes->movement(currentSolution);

                            removeNodes->movement(currentSolution);

                            if (currentSolution->getTotalBenefit() > bestSolution->getTotalBenefit()) {
                                bestSolution->resetSolution(*currentSolution);

                                cout << "***mejora**********************************************************" << endl;
                                for (int k: currentSolution->unsatisfiedDemand){
                                    if(k>0){
                                        doBreak = true;
                                        break;
                                    }
                                }
                                cout << endl;

                                t2 = clock();
                                timeToBest = ((double) (t2 - t0)) / CLOCKS_PER_SEC;

                                better++;
                            } else {
                                nobetter++;
                            }
                            lastSolution->resetSolution(*currentSolution);
                        } else {
                            nofix++;
                            currentSolution->resetSolution(*lastSolution);
                        }
                        currentSolution->temperature = currentSolution->temperature * 0.99;
                    }
                }
                delete currentSolution;
                if (doBreak){
                    break;
                }
            }

            for (Route *route:bestSolution->routes) {
                route->resetRouteType();
            }


            cout << "******************finish ******************* " << endl;

            cout << "ben " << bestSolution->getTotalBenefit() << " better " << better << endl;

            t1 = clock();
            double ttime = ((double) (t1 - t0)) / CLOCKS_PER_SEC;
            cout << "Execution Time: " << ttime << endl;

            bestSolution->writeSolution(instance, seed, timeToBest, ttime);


            delete removeNodes;
            delete reorderNodes;
            delete lastSolution;
            delete bestSolution;
            delete problemInstance;


        }
        total1 = clock();
        double totaltime = ((double) (total1 - total0)) / CLOCKS_PER_SEC;
        cout << "Execution Time: " << totaltime << endl;
    }

    return 0;
}


//            cout << "better " << better << endl;
//            cout << "fix " << fix << endl;
//            cout << "no better " << nobetter<< endl;
//            cout << "no fix " << nofix << endl;

//            vector<int> checklist;
//            for(Route *ro:bestSolution->routes){
//                for(Trip *t: ro->trips){
//                    if(t->finalNode->getId() != 0){
//                        checklist.push_back(t->finalNode->getId());
//                    }
//                }
//            }
//            int no(0);
//            for (int i: currentList){
//                if (!(find(checklist.begin(), checklist.end(), i) != checklist.end())){ // true si esta presente
//                    no++;
//                }
//            }
//            cout << "faltan " << no << " nodos"<< endl;
//            if(checklist.size() > currentList.size()){
//                cout << "sobran " << checklist.size() - currentList.size() << " nodos" << endl;
//            }


            //    vector<int> currentList = FS_5;
            //    for(int i = 0; i < currentList.size()-1; ++i){
            //        if(currentList[i]!=0 or currentList[i+1]!=0){
            //            cout << problemInstance->distances[currentList[i]][currentList[i+1]] << endl;
            //        }
            //    }
