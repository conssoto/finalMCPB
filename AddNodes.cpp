#include "AddNodes.h"

AddNodes::AddNodes():currentType(1), fix(true){};

AddNodes::~AddNodes(){
    this->unfilledRoutes.clear(); // se borran todos los punteros, las rutas se borran en la solucion
    this->unfilledRoutes.shrink_to_fit();

    this->currentRoute = nullptr;
}


void AddNodes::deleteOptions(vector<Trip *> options){
    for (int i = 1; i < options.size(); ++i){
        delete options[i]; // se borran los trips menos el primero
    }
    options.clear(); // se borran todos los punteros
    options.shrink_to_fit();
}


Trip *AddNodes::roulette(Route *route, Solution *solution) {
    int beta = solution->random_int_number(0, (int)route->trips.size()-1);
    return route->trips[beta];
}


Trip *AddNodes::getBestOption(Trip *trip, Route *route, Solution *solution){
    vector<Trip *> options;
    Node *currentNode = trip->initialNode;
    Node *nextNode = trip->finalNode;
    for (int i = 0; i < solution->nodesXQuality[route->getTypeIndex()]; ++i) {
        Node *option = solution->unvisitedNodes[i];
//        if (find(this->tabuList.begin(), this->tabuList.end(), option) == this->tabuList.end()) {
        if (route->fitsInTruck(option)) {
            auto fakeTrip = solution->fakeTrip(currentNode, option, nextNode, route);
            options.push_back(fakeTrip);
        }
//        }
    }
    if (!options.empty()) {
        sort(options.begin(), options.end(), sortByBenefit);
        deleteOptions(options); //TODO elimina punteros y trips?
        return options[0];
    }
}


int AddNodes::getInsertPosition(Route *route, Trip *selectedTrip){
    for(int i = 0; i < route->trips.size(); ++i){
        if(route->trips[i]->initialNode == selectedTrip->initialNode){
            return i;
        }
    }
}


void AddNodes::nodeAdding(Route *route, Solution *solution) {
    while (!route->isFull()) { // agrega nodos mientras existan vecinos.
        Trip *selectedPlace = roulette(route, solution);
        Trip *selectedTrip = getBestOption(selectedPlace, route, solution);
        double delta = selectedTrip->benefit; //TODO considera la mejor opcion de leche -la del camion, no lo que realmente sera tras el blending
        if (delta > 0) {
            solution->insertTrip(route, getInsertPosition(route, selectedTrip), selectedTrip->finalNode);
            solution->updateSolution(selectedTrip->finalNode, true);
            cout << "->added node " << selectedTrip->finalNode->getId() << " P: "
                 << selectedTrip->finalNode->getProduction() << " D: "
                 << selectedTrip->distance << " to route " << route->getId() << endl;//TODO es ese?
        } else {
            double r = solution->random_number(0.0, 1.0);
            double pEval = exp(delta / 100.0);
            if (pEval > r) {
                solution->insertTrip(route, getInsertPosition(route, selectedTrip), selectedTrip->finalNode);
                solution->updateSolution(selectedTrip->finalNode, true);
                cout << "->added node (NO BEN) " << selectedTrip->finalNode->getId() << " P: "
                     << selectedTrip->finalNode->getProduction() << " D: "
                     << selectedTrip->distance << " to route " << route->getId() << endl; //TODO es ese?
//                route->printTrips();
            } else {
                cout << "no se agrega nada" << endl;
            }
        }
        solution->resetRouteFull();
        this->unfilledRoutes = solution->getUnfilledRoutes();
        reverse(this->unfilledRoutes.begin(), this->unfilledRoutes.end()); // recorrerla de atras para adelante.
        if (this->unfilledRoutes.empty()) {
            this->stopCriteria = true;
        } else {
            this->currentRoute = this->unfilledRoutes[0];
        }
    }
}


void AddNodes::movement(Solution *solution) {
//    cout << endl << "AddNodes:" << endl;
    this->fix = true;
    this->currentRoute = nullptr;
    this->unfilledRoutes = solution->getUnfilledRoutes();
    reverse(this->unfilledRoutes.begin(), this->unfilledRoutes.end());
    this->stopCriteria = true;
    if(!this->unfilledRoutes.empty()){
        this->currentRoute = this->unfilledRoutes[0];
        this->stopCriteria = false;
    }
    while(!this->stopCriteria){
        nodeAdding(this->currentRoute, solution);
    }
    if (solution->getUnsatisfiedType() != -1) {
        this->fix = false;
    }
}
