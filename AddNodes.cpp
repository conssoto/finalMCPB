#include "AddNodes.h"

AddNodes::AddNodes():currentType(1), fix(true){};

AddNodes::~AddNodes(){
//    for (Trip *t: this->neighborhood){
//        delete t; // se borran los trips menos el primero
//    }
    this->neighborhood.clear(); // se borran todos los punteros
    this->neighborhood.shrink_to_fit();
}


void AddNodes::deleteOptions(vector<Trip *> options){
    for (int i = 1; i < options.size(); ++i){
        delete options[i]; // se borran los trips menos el primero
    }
    options.clear(); // se borran todos los punteros
    options.shrink_to_fit();
}


Trip *AddNodes::roulette(Route *route) {
    int beta = rand() % (int)route->trips.size();
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
//                cout << "options best fits" << endl;
            auto fakeTrip = solution->fakeTrip(currentNode, option, nextNode, route);
            options.push_back(fakeTrip);
            cout << "ben " << fakeTrip->benefit << endl;
        }
//        }
    }
    if (!options.empty()) {
        sort(options.begin(), options.end(), sortByBenefit);
        deleteOptions(options); //TODO elimina punteros y trips?
        cout << "selected" << options[0]->benefit << endl;
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
    cout << "node adding" << endl;
    cout << route->getId() << endl;
    cout << route->isFull() << endl;
    while (!route->isFull()) { // agrega nodos mientras existan vecinos.
        cout << "current route " << route->getId() << endl;
        Trip *selectedPlace = roulette(route);
        Trip *selectedTrip = getBestOption(selectedPlace, route, solution);
        double delta = selectedTrip->benefit; //TODO considera la mejor opcion de leche -la del camion, no lo que realmente sera tras el blending
        cout << "delta" << endl;
        cout << delta << endl;
        if (delta > 0) {
            solution->insertTrip(route, getInsertPosition(route, selectedTrip), selectedTrip->finalNode);
            solution->updateSolution(selectedTrip->finalNode, true);
            cout << "->added node " << selectedTrip->finalNode->getId() << " P: " << selectedTrip->finalNode->getProduction() << " D: "
                 << selectedTrip->distance << " to route " << route->getId() << endl;//TODO es ese?
        } else {
            double r = (double) (rand() % 101) / 100.0;
            double pEval = exp(delta / 100.0);
            cout << "delta < 0" << endl;
            cout << pEval << endl;
            cout << r << endl;
            if (pEval > r) {
                solution->insertTrip(route, getInsertPosition(route, selectedTrip), selectedTrip->finalNode);
                solution->updateSolution(selectedTrip->finalNode, true);
                cout << "->added node (NO BEN) " << selectedTrip->finalNode->getId() << " P: " << selectedTrip->finalNode->getProduction() << " D: "
                     << selectedTrip->distance << " to route " << route->getId() << endl; //TODO es ese?
//                route->printTrips();
            } else {
                cout << "no se agrega nada" << endl;
            }
        }
        solution->resetRouteFull();
        this->unfilledRoutes = solution->getUnfilledRoutes();
        if(this->unfilledRoutes.empty()){
            this->stopCriteria = true;
        }
        else{
            this->currentRoute = this->unfilledRoutes[0];
        }
        cout << "actualizado " << this->unfilledRoutes.size() << endl;
        for(Route *r: this->unfilledRoutes){
            cout << r->getId() << endl;
        }
    }
}


void AddNodes::movement(Solution *solution) {
//    cout << endl << "AddNodes:" << endl;
    this->fix = true;
    this->currentRoute = nullptr;
    this->unfilledRoutes = solution->getUnfilledRoutes();
    for(Route *r: this->unfilledRoutes){
        cout << r->getId() << endl;
    }
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

//    while (!this->unfilledRoutes.empty()) {
//        for (Route *route: this->unfilledRoutes) { // agregar todos los que beneficien
//            cout << "route" << route->getId() << endl;
//            nodeAdding(route, solution);
//            cout << "despues de node adding" << endl;
//        }
//        if (solution->getUnsatisfiedType() != -1) {
//            this->fix = false;
//        }
//        else{
//            cout << "fix " << solution->getUnsatisfiedType() << endl;
//        }
//    }
} //TODO cuando se acaban las opciones, unfilledRoutes es empty
