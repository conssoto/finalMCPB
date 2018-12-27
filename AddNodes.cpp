#include "AddNodes.h"

AddNodes::AddNodes():currentType(1), fix(true){};

AddNodes::~AddNodes(){
    for (Trip *t: this->neighborhood){
        delete t; // se borran los trips menos el primero
    }
    this->neighborhood.clear(); // se borran todos los punteros
    this->neighborhood.shrink_to_fit();
}

bool AddNodes::fitsInTruck(Route *route, Node *node){
    return route->remainingCapacity >= node->getProduction();
}

void AddNodes::deleteNeighborhood(){
    for (Trip *trip:this->neighborhood) {
        delete trip;
    }
    this->neighborhood.clear();
    this->neighborhood.shrink_to_fit();
}

void AddNodes::deleteOptions(vector<Trip *> options, int aux = 1){
    for (int i = aux; i < options.size(); ++i){
        delete options[i]; // se borran los trips menos el primero
    }
    options.clear(); // se borran todos los punteros
    options.shrink_to_fit();
}

void AddNodes::setTabuList(vector <Node *> tabuList){
    this->tabuList.clear();
    for (Node *node: tabuList){
        Node *copy = node;
        this->tabuList.push_back(copy);
    }
}

Trip *AddNodes::getNeighbor(Solution *solution, Route *route, Trip *trip) {
    vector<Trip *> options;
    Trip *selectedTrip = nullptr;
    Node *currentNode = trip->initialNode;
    Node *nextNode = trip->finalNode;
    for (int i = 0; i < solution->nodesXQuality[route->getTypeIndex()]; ++i) {
        Node *option = solution->unvisitedNodes[i];
        if(find(this->tabuList.begin(), this->tabuList.end(), option) == this->tabuList.end()) {
            if (fitsInTruck(route, option)) {
                auto fakeTrip = solution->fakeTrip(currentNode, option, nextNode, route);
                if (benefitOnly) {
                    if (fakeTrip->benefit > 0) {
                        options.push_back(fakeTrip);
                    } else {
                        delete fakeTrip;
                    }
                } else {
                    options.push_back(fakeTrip);
                }
            }
        }
    }
    if (!options.empty()) {
        sort(options.begin(), options.end(), sortByBenefit);
        selectedTrip = options[0]; //puntero apunta al trip
        deleteOptions(options); //TODO elimina punteros y trips?
    }
    // TODO lista tabu?
    return selectedTrip;
}

void AddNodes::setNeighborhood(Solution *solution, Route *route) {
    this->neighborhood.clear();
    vector<Trip *> options;
    for (Trip *trip: route->trips) {
        Trip *selectedTrip = getNeighbor(solution, route, trip);
        if (selectedTrip) {
            options.push_back(selectedTrip);
        }
    }
    this->neighborhood = options;
}

void AddNodes::setTotalBenefit() {
    this->totalBenefit = 0;
    if (benefitOnly) {
        for (Trip *trip: this->neighborhood) {
            this->totalBenefit += trip->benefit;
        }
    } else {
        for (Trip *trip: this->neighborhood) {
            this->totalBenefit += -1 / trip->benefit;
        }
    }
}

Trip *AddNodes::roulette() {
    setTotalBenefit();
    int beta = rand() % 101;
    double choiceProbability(0);
    int aux(0);
    for (Trip *trip: this->neighborhood) {
        if (beta > stoi(to_string(choiceProbability))) {
            if (benefitOnly) {
                choiceProbability += trip->benefit * 100.0 / this->totalBenefit;
            } else {
                choiceProbability += -1 / trip->benefit * 100.0 / this->totalBenefit;
            }
        }
        if ((beta <= stoi(to_string(choiceProbability))) or (stoi(to_string(choiceProbability)) == 100)) {
            auto *selectedTrip = new Trip(*trip);
            deleteNeighborhood();
            deleteOptions(this->neighborhood, aux);
            return selectedTrip;
        }
    }
}

void AddNodes::setInsertPosition(Route *route, Trip *selectedTrip){
    for(int i = 0; i < route->trips.size(); ++i){
        if(route->trips[i]->initialNode == selectedTrip->initialNode){
            this->insertPosition = i;
            break;
        }
    }
};

void AddNodes::nodeAdding(Route *route, Solution *solution){
    bool stopCritera(false);
    while (!stopCritera) { // mientras tenga nodos que sacar, saca
        setNeighborhood(solution, route);
        if (!this->neighborhood.empty()) {
            Trip *selectedTrip = roulette();
            setInsertPosition(route, selectedTrip);
            solution->insertTrip(route, this->insertPosition, selectedTrip->finalNode);
            solution->updateSolution(selectedTrip->finalNode, true);
            cout << "->added node " << selectedTrip->finalNode->getId() << " P: " << selectedTrip->finalNode->getProduction() << " D: " << selectedTrip->distance << endl;
//            route->printTrips();
            delete selectedTrip;
        }
        else {
            stopCritera = true;
        }
        if (!this->benefitOnly and solution->unsatisfiedDemand[route->getTypeIndex()] <= 0) {
            stopCritera = true;
        }
    }
}

void AddNodes::movement(Solution *solution){
//    cout << endl << "AddNodes:" << endl;
    this->fix = true;
    for(Route *route: solution->routes) { // agregar todos los que beneficien
        this->benefitOnly= true;
        nodeAdding(route, solution);
    }
    for (int i = 0; i < solution->unsatisfiedDemand.size(); ++i) { //agrega los que no benefician para satisfacer
        if (solution->unsatisfiedDemand[i] > 0) {
            int j = i;
            while (solution->unsatisfiedDemand[i] > 0){
                for(Route *route: solution->routes) {
                    if(route->getTypeIndex() == j){
                        this->benefitOnly = false;
                        nodeAdding(route, solution);
                    }
                }
                j--;
                if(j < 0 and solution->unsatisfiedDemand[i] > 0){ // no se pudo arreglar
                    cout << "no Fix" << endl;

                    this->fix = false;
                    break;
                }
            }
        }
    }
}
