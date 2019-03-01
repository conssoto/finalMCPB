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

//tamano vecindario
size_t BETA = 5;

//cantidad de nodos a eliminar
int GAMMA = 3;

// cantidad de nodos que se intentar insertar antes de recalentar
int EPSILON = 5;

// el primer nodo se selecciona:
// dentro de los beta primeros -> corte=true
// dentro de t.odo el vecindario -> corte=false
bool CORTE = true;

vector<unsigned int> SEEEDS = {1008, 1505, 494, 15, 937, 1801, 522, 7, 24, 116, 9574, 7121, 803, 347, 3795, 672, 49, 2647, 223, 236}; // {cut Neighborhood}

int main(int argc, char *argv[]) {

    if(not argv[2]){
        Reader r(to_string(3));
        r.readOutputs(SEEEDS);
    }
    else{
        int instance(atoi(argv[1]));
        int seed(atoi(argv[2]));

        srand(seed);

        clock_t t0, t1, t2, timeToBest;

        t0 = clock();
        Reader r(to_string(instance));

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
                }
            } else {
                currentSolution->unsatisfiedDemand = lastSolution->unsatisfiedDemand;
                currentSolution->newDemands = lastSolution->newDemands;
                newD += 1;
            }


            auto *construction = new Construction(BETA, currentSolution);

            construction->feasibleSolution(currentSolution, CORTE);

            lastSolution->resetSolution(*currentSolution);

            cout << "***construccion**" << endl;

            cout <<construction->construct << endl;

            if (construction->construct) {
                for (int i = 0; i < 2500; ++i) {

                    removeNodes->breakDemands(currentSolution, GAMMA);

                    addNodes->movement(currentSolution, EPSILON);

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

    return 0;
}
