#include "Construction.h"

Construction::Construction(size_t alpha, Solution *solution):alpha(alpha),totalProduction(0), currentType(1), currentNode(solution->plant), currentRoute(solution->routes.back()), construct(true){};

Construction::~Construction(){
    cout<< "Deleting construction" << endl;

    for (Trip *t:this->neighborhood) {
        delete t;
    }
    this->neighborhood.clear();
    this->neighborhood.shrink_to_fit(); //TODO borrar la ruta??
}

bool Construction::fitsInTruck(Route *route, Node *node){
    return route->remainingCapacity >= node->getProduction();
}

void Construction::deleteNeighborhood(){
    for (Trip *trip:this->neighborhood) {
        delete trip;
    }
    this->neighborhood.clear();
    this->neighborhood.shrink_to_fit();
}

vector<Trip *> Construction::getOptions(Solution *solution, int currentType, Node *currentNode, bool resize) {
    vector<Trip *> options;
    for (int i = 0; i < solution->nodesXQuality[currentType - 1]; ++i) {
        Node *option = solution->unvisitedNodes[i];
        if (fitsInTruck(solution->routes.back(), option)) {
            options.push_back(solution->newTrip(currentNode, option, solution->routes.back()));
        }
    }
    if (resize){
        if (options.size() > this->alpha) {
            sort(options.begin(), options.end(), sortByDistance);
            options.resize(this->alpha);
        }
    }
    return options;
}

void Construction::setNeighborhood(Solution *solution, bool repairing) {
    vector<Trip *> options;
    if (repairing){
        vector<Route *> unfilledRoutes(solution->getUnfilledRoutes());
        if(unfilledRoutes.empty()){
            this->construct = false;
        }
        if(this->construct){
            this->currentRoute =unfilledRoutes.back();
            this->currentNode = this->currentRoute->trips.back()->finalNode;
            this->currentType = this->currentRoute->getType();

            if(this->currentNode == solution->plant){

                this->currentRoute->distance -= this->currentRoute->trips.back()->distance;
                this->currentRoute->trips.pop_back(); //saco la vuelta a la planta
                this->currentNode = this->currentRoute->trips.back()->finalNode;
            }
            options = getOptions(solution, this->currentType, this->currentNode, true);
        }
    }
    else{
        this->currentNode = solution->getCurrentNode();
        this->currentRoute = solution->routes.back();
        this->currentType = solution->getUnsatisfiedType();

        //si volvio a la planta, por que ya se lleno el camion, se crea nueva ruta.
        if (this->currentNode == solution->plant && !this->currentRoute->trips.empty()) {
            solution->addRoute(this->currentType);
            this->currentRoute = solution->routes.back();
            options = getOptions(solution, this->currentType, this->currentNode, solution->parameters[0]);
        }
        else{
            options = getOptions(solution, this->currentType, this->currentNode, true);
        }
    }
    if (options.empty()) {
        this->currentRoute->setFull();
        options.push_back(solution->newTrip(this->currentNode, solution->plant, this->currentRoute));
    }
    this->neighborhood = options;
}

void Construction::setTotalProduction(){
    this->totalProduction = 0;
    for(Trip *trip: this->neighborhood) { //siempre hay al menos uno, la planta
        this->totalProduction += trip->finalNode->getProduction();
    }
}

Trip *Construction::roulette() {
    setTotalProduction();
    if (this->totalProduction == 0) { //volver a la planta
        return this->neighborhood[0];
    }
    int beta = rand() % 101;
    double choiceProbability(0);
    for (Trip *trip: this->neighborhood) {
        if (beta > stoi(to_string(choiceProbability))) {
            choiceProbability += trip->finalNode->getProduction() * 100.0 / this->totalProduction;
        }
        if ((beta <= stoi(to_string(choiceProbability))) or (stoi(to_string(choiceProbability))==100)) { //TODO eliminar los trips no hacerlo aca, hacerlo cuando termine la iteracion
            auto *selectedTrip = new Trip(*trip);
            deleteNeighborhood();
            return selectedTrip;
        }
    }
}

void Construction::updateIds(vector<Route *> routes){ // TODO para que se usan?
    for (Route *r: routes){
        int routeId(r->getId());
        for (Trip *t: r->trips){
            t->setRouteId(routeId);
        }
    }
}

void Construction::feasibleSolution(Solution *solution) {
    /// Fase 1: agregar nodo favoreciendo producion dentro de los mas cercanos.
//    cout << endl << "----FASE 1----" << endl;
    while (solution->getUnsatisfiedType() != -1) {
        setNeighborhood(solution, false);
        Trip *selectedTrip = roulette();
        if (this->currentRoute->trips.empty()){

        }
        solution->addTrip(selectedTrip, this->currentRoute);
        solution->stepUpdateSolution(selectedTrip, this->currentRoute, false);

        if (solution->unusedTrucks.empty() && !solution->routes.back()->trips.empty() &&
            solution->routes.back()->trips.back()->finalNode == solution->plant) {
            break; // iniciar fase 2
        }
    }
//    solution->printAll();

    /// Fase 2: reparacion, agregar nodos a los camiones con espacio.
//    cout << endl <<  " -----FASE 2-----" << endl;
    this->construct = true;
    while (solution->getUnsatisfiedType() != -1) {
        setNeighborhood(solution, true);
        if(construct){
            Trip *selectedTrip = roulette();
            solution->addTrip(selectedTrip, this->currentRoute);
            solution->stepUpdateSolution(selectedTrip, this->currentRoute, true);
        }
        else{
            break;
        }
    }
    this->updateIds(solution->routes);
//    solution->printAll();
}
