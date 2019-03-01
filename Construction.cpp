#include "Construction.h"

Construction::Construction(size_t alpha, Solution *solution):alpha(alpha),totalProduction(0), currentType(1), currentNode(solution->plant), currentRoute(solution->routes.back()), construct(true){};

Construction::~Construction(){
    cout<< "Deleting construction" << endl;

    for (Trip *t:this->neighborhood) {
        delete t;
    }
    this->neighborhood.clear();
    this->neighborhood.shrink_to_fit(); //TODO borrar la ruta??

    this->currentNode = nullptr;
    this->currentRoute = nullptr;
}

void Construction::deleteNeighborhood(){
    for (Trip *trip:this->neighborhood) {
        delete trip;
    }
    this->neighborhood.clear();
    this->neighborhood.shrink_to_fit();
}

vector<Trip *> Construction::getOptions(Solution *solution,  bool resize) {
    vector<Trip *> options;
    for (int i = 0; i < solution->nodesXQuality[this->currentType - 1]; ++i) {
        Node *option = solution->unvisitedNodes[i];
        if (this->currentRoute->fitsInTruck(option)) {
            options.push_back(solution->newTrip(this->currentNode, option, this->currentRoute));
        }
    }
    if (resize) {
        if (options.size() > this->alpha) {
            sort(options.begin(), options.end(), sortByDistance);
            options.resize(this->alpha);
        }
    }
    return options;
}

void Construction::setNeighborhood(Solution *solution, bool repairing, bool corte) {
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
            options = getOptions(solution, true);
        }
    }
    else{ //esta construyendo
        this->currentNode = solution->getCurrentNode();
        this->currentRoute = solution->routes.back();
        this->currentType = solution->getCurrentType();

        //si volvio a la planta, por que ya se lleno el camion, se crea nueva ruta.
        if (this->currentNode == solution->plant && !this->currentRoute->trips.empty()) {
            solution->addRoute(this->currentType);
            this->currentRoute = solution->routes.back();
            options = getOptions(solution, corte); //primer nodo true con corte
        }
        else{
            options = getOptions(solution, true);
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

Trip *Construction::roulette(Solution *solution) {
    setTotalProduction();
    if (this->totalProduction == 0) { //volver a la planta
        return this->neighborhood[0];
    }
    double beta = solution->random_number(0.0, 100.0);
    double choiceProbability(0);
    for (Trip *trip: this->neighborhood) {
        if (beta > choiceProbability) {
            choiceProbability += trip->finalNode->getProduction() * 100.0 / this->totalProduction;
        }
        if ((beta <= choiceProbability) or (stoi(to_string(choiceProbability))==100)) { //TODO eliminar los trips no hacerlo aca, hacerlo cuando termine la iteracion
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

void Construction::feasibleSolution(Solution *solution, bool corte) {
    /// Fase 1: agregar nodo favoreciendo producion dentro de los mas cercanos.
//    cout << endl << "----FASE 1----" << endl;

    while (solution->getUnsatisfiedType(0) != -1) {
        setNeighborhood(solution, false, corte);
        Trip *selectedTrip = roulette(solution);
        solution->addTrip(selectedTrip, this->currentRoute);
        solution->stepUpdateSolution(selectedTrip, this->currentRoute, false);

        if (solution->unusedTrucks.empty() && !solution->routes.back()->trips.empty() &&
            solution->routes.back()->trips.back()->finalNode == solution->plant) {
            break; // iniciar fase 2
        }
    }

    /// Fase 2: reparacion, agregar nodos a los camiones con espacio.
//    cout << endl <<  " -----FASE 2-----" << endl;
    this->construct = true;
    while (solution->getUnsatisfiedType(0) != -1) {
        setNeighborhood(solution, true, corte);
        if (construct) {
            Trip *selectedTrip = roulette(solution);
            solution->addTrip(selectedTrip, this->currentRoute);
            solution->stepUpdateSolution(selectedTrip, this->currentRoute, true);
        } else {
            break;
        }
    }
    for (Route *r: solution->routes) {
        if (r->trips.empty()) {
            Trip *selectedTrip = solution->newTrip(solution->plant, solution->plant, r);
            solution->addTrip(selectedTrip, r);
            solution->stepUpdateSolution(selectedTrip, r, true);
        }
    }
//    //random. <- segun cuantos queden(?) <- ojo que 3 siempre quedan
//    for(Truck *truck: solution->unusedTrucks){
//        solution->addRoute(1); //no importa por que al ser largo 1,? se le asigna un tipo random.
//        Route *currentRoute=solution->routes.back();
//        Trip *toPlant = solution->newTrip(solution->plant, solution->plant, currentRoute);
//        solution->addTrip(toPlant, currentRoute);
//        solution->stepUpdateSolution(toPlant, currentRoute, true);
//    }
    this->updateIds(solution->routes); //TODO sirve>
//    solution->printAll();
}
