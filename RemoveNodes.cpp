#include "RemoveNodes.h"

RemoveNodes::RemoveNodes():totalBenefit(0){};

RemoveNodes::~RemoveNodes(){
    this->deletableTripsIndex.clear();
    this->deletableTripsIndex.shrink_to_fit();

    this->benefits.clear();
    this->benefits.shrink_to_fit();
};

int RemoveNodes::getCuttingDemand(int typeIndex, Solution *solution) {
    int minDemand(solution->unsatisfiedDemand[typeIndex]);
    for (int i = typeIndex; i < solution->unsatisfiedDemand.size(); ++i) {
        if (solution->unsatisfiedDemand[i] < 0 and solution->unsatisfiedDemand[i] > minDemand) {
            minDemand = solution->unsatisfiedDemand[i];
        }
    }
    return minDemand;
}

double RemoveNodes::getDeletableBenefit(Trip *deletableTrip, Trip *nextTrip, Route *route, Solution *solution) {
    double deletableProd(
            solution->literCost[route->getTypeIndex()] *
            deletableTrip->finalNode->getProduction());
    double deletableDistance(solution->kilometerCost * (deletableTrip->distance + nextTrip->distance));
    double newDistance(solution->kilometerCost *
                       (solution->problemInstance->getDistance(deletableTrip->initialNode,
                                                               nextTrip->finalNode)));
    return -(deletableProd - deletableDistance + newDistance); //sacarlo implica esto
}

void RemoveNodes::setDeletable(Route *route, Solution *solution) {
    this->deletableTripsIndex.clear();
    this->benefits.clear();
    double cuttingDemand(getCuttingDemand(route->getTypeIndex(), solution));
    for (int i = 0; i < route->trips.size() - 1; ++i) {
        Trip *deletableTrip = route->trips[i];
        if (cuttingDemand < 0 and deletableTrip->finalNode->getProduction() < -cuttingDemand) {
            Trip *nextTrip = route->trips[i + 1];
            double deletableBenefit = getDeletableBenefit(deletableTrip, nextTrip, route, solution);
            if(deletableBenefit > 0.0){ //vale la pena sacarlo
                this->deletableTripsIndex.push_back(i); //beneficio de sacarlo
                this->benefits.push_back(deletableBenefit); // >0 conviene sacarlo
            }
        }
    }
}

void RemoveNodes::breakDemands(Solution *solution, int gamma) { // hasta romper alguna, no se quita en todas las rutas
//    cout << endl << "Break Demands:" << endl;
    bool stopCritera(false);
    int n(0);
    int stop(0);
    while (!stopCritera) {
        int deleteRouteIndex = solution->random_int_number(0, (int) solution->routes.size() - 1); // size-1 es el index
        Route *route(solution->routes[deleteRouteIndex]);
        if ((int) route->trips.size() != 1) {
            int deleteTripIndex = solution->random_int_number(0, (int) route->trips.size() -
                                                             2);// cualquier trip menos el ultimo
            double delta = getDeletableBenefit(route->trips[deleteTripIndex], route->trips[deleteTripIndex + 1], route,
                                               solution);
            if (delta > 0) { // si sacarlo genera un beneficio, se elimina.
                n++;
//                cout << "->break node " << route->trips[deleteTripIndex]->finalNode->getId() << " P: "
//                     << route->trips[deleteTripIndex]->finalNode->getProduction() << " D: "
//                     << route->trips[deleteTripIndex]->distance << " T: "
//                     << route->trips[deleteTripIndex]->finalNode->getType() << " from route " << route->getId() << endl;
//                route->printAll();
                solution->updateSolution(route->trips[deleteTripIndex]->finalNode, false);
                solution->removeTrip(deleteTripIndex, route);
                solution->resetDemands();
            } else {
                double r = solution->random_number(0.0, 1.0);
                double pEval = exp(delta / solution->temperature);
                if (pEval > r) {
                    n++;
//                    cout << "->break node (NO BEN) " << route->trips[deleteTripIndex]->finalNode->getId() << " P: "
//                         << route->trips[deleteTripIndex]->finalNode->getProduction() << " D: "
//                         << route->trips[deleteTripIndex]->distance << " T: "
//                         << route->trips[deleteTripIndex]->finalNode->getType() << " from route " << route->getId()
//                         << endl;
//                    route->printAll();
                    solution->updateSolution(route->trips[deleteTripIndex]->finalNode, false);
                    solution->removeTrip(deleteTripIndex, route);
                    solution->resetDemands();
                } else {
//                    cout << "no se saca nada" << endl;
                }
            }
            for (int d: solution->unsatisfiedDemand) { //si rompe alguna, para.
                if (d > 0 and n >= gamma) { // <- elimine tres al menos! or (n >= solution->routes.size()*2)
                    stopCritera = true;
                }
            }
        } else{
            stop++;
        }
        if(stop > (int) solution->routes.size()*5){
            stopCritera = true;
        }
    }
    solution->resetRouteFull();
}

void RemoveNodes::movement(Solution *solution) {
//    cout << endl <<"RemoveNodes:" << endl;
    for (Route *route: solution->routes) {
        if ((int) route->trips.size() > 2) { // no dejar la ruta vacia, todos loscami
            bool stopCritera(false);
            while (!stopCritera) { // mientras tenga nodos que sacar, saca
                setDeletable(route, solution);
                if (!this->benefits.empty()) {//si no esta vacio
                    int selectedIndex = roulette(solution);
                    int deleteIndex(this->deletableTripsIndex[selectedIndex]);
                    solution->updateSolution(route->trips[deleteIndex]->finalNode, false);
                    solution->removeTrip(deleteIndex, route);
                    solution->resetDemands();
                } else {
                    stopCritera = true;
                }
                if((int) route->trips.size() > 2){
                    stopCritera = true;
                }
            }
        }
    }
}

void RemoveNodes::setTotalBenefit(){
    this->totalBenefit = 0;
    for(double b: this->benefits) {
        this->totalBenefit += b;
    }
}

int RemoveNodes::roulette(Solution *solution) {
    setTotalBenefit();
    double beta = solution->random_number(0.0, 100.0);
    double choiceProbability(0.0);
    for ( int i =0 ; i < this->benefits.size() ; ++i) {
        if (beta > choiceProbability) {
            choiceProbability += this->benefits[i] * 100.0 / this->totalBenefit;
        }
        if ((beta <= choiceProbability) or (stoi(to_string(choiceProbability)) == 100)) {
            return i;
        }
    }
}
