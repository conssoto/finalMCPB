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

void RemoveNodes::setDeletable(Route *route, Solution *solution) {
    this->deletableTripsIndex.clear();
    this->benefits.clear();
    for (int i = 0; i < route->trips.size() - 1; ++i) {
        Trip *deletableTrip = route->trips[i];
        double cuttingDemand(getCuttingDemand(route->getTypeIndex(), solution));
        if (cuttingDemand < 0 and deletableTrip->finalNode->getProduction() < -cuttingDemand) {
            Trip *nextTrip = route->trips[i + 1];
            double deletableProd(
                    solution->literCost[deletableTrip->finalNode->getTypeIndex()] *
                    deletableTrip->finalNode->getProduction());
            double deletableDistance(solution->kilometerCost * (deletableTrip->distance + nextTrip->distance));
            double newDistance(solution->kilometerCost *
                               (solution->problemInstance->getDistance(deletableTrip->initialNode,
                                                                             nextTrip->finalNode)));
            if (deletableProd - deletableDistance < -newDistance) {
                this->deletableTripsIndex.push_back(i);
                this->benefits.push_back(-deletableProd + deletableDistance - newDistance);
            }
        }
//        }
    }
}

void RemoveNodes::breakDemands(Solution *solution) { // hasta rom,per alguna, no se quita en todas las rutas
//    cout << endl << "Break Demands:" << endl;
    this->tabuList.clear();
    bool stopCritera(false);
    while (!stopCritera) {
        int deleteRouteIndex = (rand() % (int)(solution->routes.size()-1)); // cualquier ruta
        Route *route(solution->routes[deleteRouteIndex]);
        int deleteTripIndex(0);
        if (route->trips.size() != 2) { //TODO deberia poder borrar si son solo dos
            deleteTripIndex = (rand() % (int)(route->trips.size()-2)); // cualquier trip menos el ultimo
        }
        if (route->trips[deleteTripIndex]->finalNode != solution->plant) {
            cout << "->break node " << route->trips[deleteTripIndex]->finalNode->getId() << " P: " << route->trips[deleteTripIndex]->finalNode->getProduction() << " D: " << route->trips[deleteTripIndex]->distance << " T: " << route->trips[deleteTripIndex]->finalNode->getType() << endl;
            this->tabuList.push_back(route->trips[deleteTripIndex]->finalNode);
            solution->updateSolution(route->trips[deleteTripIndex]->finalNode, false);
            solution->removeTrip(deleteTripIndex, route);
            solution->resetDemands();
        }
        for (int d: solution->unsatisfiedDemand) { //si rompe alguna, para.
            if (d < -100000){
                cout << "*****************************************************************************" << endl;
            }
            if (d > 0) {
                stopCritera = true;
            }
        }
    }
}

void RemoveNodes::movement(Solution *solution){
//    cout << endl <<"RemoveNodes:" << endl;

    for (Route *route: solution->routes){
        bool stopCritera(false);
        while(!stopCritera){ // mientras tenga nodos que sacar, saca
            setDeletable(route, solution); //si no esta vacio
            if (!this->benefits.empty()){
                int selectedIndex = roulette();
                int deleteIndex(this->deletableTripsIndex[selectedIndex]);
                cout << "->deleted node " << route->trips[deleteIndex]->finalNode->getId() << " P: " << route->trips[deleteIndex]->finalNode->getProduction() << " D: " << route->trips[deleteIndex]->distance << " T: " << route->trips[deleteIndex]->finalNode->getType() <<  endl;
                solution->updateSolution(route->trips[deleteIndex]->finalNode, false);
                solution->removeTrip(deleteIndex, route);
                solution->resetDemands();
            }
            else{
                stopCritera = true;
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

int RemoveNodes::roulette() {
    setTotalBenefit();
    int beta = rand() % 101;
    double choiceProbability(0);
    for ( int i =0 ; i < this->benefits.size() ; ++i) {
        if (beta > stoi(to_string(choiceProbability))) {
            choiceProbability += this->benefits[i] * 100.0 / this->totalBenefit;
        }
        if ((beta <= stoi(to_string(choiceProbability))) or (stoi(to_string(choiceProbability)) == 100)) {
            return i;
        }
    }
}
