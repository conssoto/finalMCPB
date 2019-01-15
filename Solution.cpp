#include "Solution.h"

Solution::Solution(ProblemInstance *problemInstance, vector<bool> parameters){
    for(int i = 0; i < problemInstance->getNumberOfQualities(); ++i){
        this->recollected.push_back(0);
    }

    this->unsatisfiedDemand = problemInstance->qualities;
    this->newDemands = problemInstance->qualities;

    for (Truck *truck: problemInstance->trucks){
        Truck *copy = truck;
        addTruck(copy);
    }

    for (Node * node: problemInstance->nodes){
        Node *copy = node;
        this->addNode(copy);
    }

    setQualities(problemInstance->getNumberOfQualities());

    this->addRoute(1);

    this->problemInstance = problemInstance;

    this->plant = new Node(0, 0, 0);
    this->literCost = {0.03, 0.021, 0.009};
    this->kilometerCost = 1;
    this->parameters = parameters;

    this->temperature = problemInstance->temperature;
}

Solution::~Solution() {
//    cout << "Deleting Solution" << endl;
    this->recollected.clear();
    this->recollected.shrink_to_fit();

    this->unsatisfiedDemand.clear();
    this->unsatisfiedDemand.shrink_to_fit();

    this->newDemands.clear();
    this->newDemands.shrink_to_fit();

    this->unusedTrucks.clear();
    this->unusedTrucks.shrink_to_fit();

    this->unvisitedNodes.clear();
    this->unvisitedNodes.shrink_to_fit();

    this->nodesXQuality.clear();
    this->nodesXQuality.shrink_to_fit();

    for (Route *route:this->routes) {
        delete route;
    }
    this->routes.clear();
    this->routes.shrink_to_fit();

    this->problemInstance = nullptr;

//    delete (this->plant);
//    this->plant = nullptr;

    this->literCost.clear();
    this->literCost.shrink_to_fit();

    this->parameters.clear();
    this->parameters.shrink_to_fit();
}

Solution::Solution(const Solution &s2){

    recollected = s2.recollected;
    unsatisfiedDemand = s2.unsatisfiedDemand;
    newDemands = s2.newDemands;

    for (Truck *truck: s2.unusedTrucks){
        Truck *copy = truck;
        unusedTrucks.push_back(copy);
    }

    for (Node *node: s2.unvisitedNodes){
        Node *copy = node;
        unvisitedNodes.push_back(copy);
    }

    nodesXQuality = s2.nodesXQuality;

    for (Route *route: s2.routes){
        auto *copy = new Route(*route);
        routes.push_back(copy);
    }

    problemInstance = s2.problemInstance;

    plant = new Node(0, 0, 0);
    literCost = s2.literCost;
    kilometerCost = s2.kilometerCost;

    temperature = s2.temperature;
}

void Solution::resetSolution(const Solution &s2) {

    this->recollected = s2.recollected;
    this->unsatisfiedDemand = s2.unsatisfiedDemand;
    this->newDemands = s2.newDemands;

    this->unusedTrucks.clear();
    this->unusedTrucks.shrink_to_fit();
    for (Truck *truck: s2.unusedTrucks) {
        Truck *copy = truck;
        this->unusedTrucks.push_back(copy);
    }

    this->unvisitedNodes.clear();
    this->unvisitedNodes.shrink_to_fit();
    for (Node *node: s2.unvisitedNodes) {
        Node *copy = node;
        this->unvisitedNodes.push_back(copy);
    }

    this->nodesXQuality = s2.nodesXQuality;

    for (Route *route:this->routes) {
        delete route;
    }
    this->routes.clear();
    this->routes.shrink_to_fit();

    for (Route *route: s2.routes) {
        Route *copy = new Route(*route);
        this->routes.push_back(copy);
    }
    this->temperature = s2.temperature;
}

void Solution::changeDemands(){
    vector<int> totalProductionByType(this->recollected.size(), 0);
    vector<int> totalLiterBenefitByType(this->recollected.size(), 0);
    vector<int> newDemands(this->recollected.size(), 0);
    int totalLiterBenefit(0);
    for(Node *node: problemInstance->nodes){
        totalProductionByType[node->getTypeIndex()] += node->getProduction();
        totalLiterBenefitByType[node->getTypeIndex()] += (int)(node->getProduction()*this->literCost[node->getTypeIndex()]);
        totalLiterBenefit += (int)(node->getProduction()*this->literCost[node->getTypeIndex()]);
    }
    for(Truck *t: problemInstance->trucks){ //problema, quedan camiones
        double beta = this->random_number(0.0, 100.0);
        double choiceProbability(0.0);
        for ( int i =0 ; i < totalLiterBenefitByType.size() ; ++i) {
            if (beta > choiceProbability) {
                choiceProbability += totalLiterBenefitByType[i] * 100.0 / totalLiterBenefit;
            }
            if ((beta <= choiceProbability) or (stoi(to_string(choiceProbability)) == 100)) {
                newDemands[i] += (int)(0.7*t->getTotalCapacity()); //TODO funciona(?)
                totalProductionByType[i] -=t->getTotalCapacity();
                totalLiterBenefitByType[i] -=(int)(t->getTotalCapacity()*this->literCost[i]);
                totalLiterBenefit -= (int)(t->getTotalCapacity()*this->literCost[i]);
                if(totalProductionByType[i] < t->getTotalCapacity()){
                    totalLiterBenefit -= (totalLiterBenefitByType[i]);
                    totalProductionByType.erase(totalProductionByType.begin() + i);
                    totalLiterBenefitByType.erase(totalLiterBenefitByType.begin() + i);
                }
                break;
            }
        }
    }
    this->unsatisfiedDemand = newDemands;
    this->newDemands = newDemands;
}

double Solution::random_number(double min, double max){
    thread_local static mt19937 mt(1008);
    thread_local static uniform_real_distribution<double> dist;
    using pick = uniform_real_distribution<double>::param_type;
    return dist(mt, pick(min, max));
}

int Solution::random_int_number(int min, int max){
    thread_local static mt19937 mt(1008);
    thread_local static uniform_int_distribution<int> dist;
    using pick = uniform_int_distribution<int>::param_type;
    return dist(mt, pick(min, max));
}

int Solution::getBlendingType(int currentRouteTypeIndex) {
    int unsatisfiedTypeIndex(getUnsatisfiedType(currentRouteTypeIndex));
    if (unsatisfiedTypeIndex != -1) {
        if (unsatisfiedTypeIndex >= currentRouteTypeIndex) {
            return unsatisfiedTypeIndex;
        }
    }
    return currentRouteTypeIndex; //es el tipo actual si no hay unsatisfiedDemand o este es mayor que el tipo de la ruta
}

double Solution::getTotalBenefit() {
    double totalDistance(0);
    for (Route *r: this->routes) {
        totalDistance += r->distance;
    }

    vector<int> totalMilk = convertMilk(false);
    reverse(totalMilk.begin(), totalMilk.end());

    double suma(0);
    for (int i = 0; i < totalMilk.size(); ++i) {
        suma += totalMilk[i] * this->literCost[i];
    }

    return suma-totalDistance;
}

void Solution::setQualities(double size) {
    vector<int> aux(size, 0);
    for (Node *node: this->unvisitedNodes) {
        aux[node->getTypeIndex()]++;
    }
    for (int i = 0; i < size + 1; ++i) {
        int init = 0;
        if (i != 0) {
            this->nodesXQuality.push_back(accumulate(&aux[0], &aux[i], init));
        }
    }
}

vector<Route *> Solution::getUnfilledRoutes() {
    vector<Route *> unfilledRoutes;
    for (Route *route: this->routes) {
        if (!route->isFull()) {
            unfilledRoutes.push_back(route);
        }
    }
    return unfilledRoutes;
}

void Solution::addNode(Node *node){this->unvisitedNodes.push_back(node);}

void Solution::addTruck(Truck *truck){this->unusedTrucks.push_back(truck);}

void Solution::addRoute(int type) {
    if (!this->unusedTrucks.empty()) {
        int routeId((int)this->routes.size() + 1);
        Truck *truck = this->getNextTruck();
        auto route = new Route(routeId, truck, type);
        this->routes.push_back(route);
        this->removeTruck(truck);
    }
}

void Solution::addTrip(Trip *trip, Route *route) {
    route->trips.push_back(trip);
}

void Solution::insertNode(Node *node) {
    this->unvisitedNodes.insert(this->unvisitedNodes.begin() + this->nodesXQuality[node->getTypeIndex()], node);
    for (int i = node->getTypeIndex(); i < this->nodesXQuality.size(); ++i) {
        this->nodesXQuality[i] += 1;
    }
}

void Solution::insertTrip(Route *route, int index, Node *node) {
    Trip *modifiedNode = route->trips[index];
    route->distance -= modifiedNode->distance;
    Trip *newTrip = this->newTrip(modifiedNode->initialNode, node, route);

    modifiedNode->initialNode = node;
    modifiedNode->distance = this->problemInstance->getDistance(modifiedNode->initialNode, modifiedNode->finalNode);
    modifiedNode->setBenefit(this->calculateBenefit(modifiedNode, route->getTypeIndex()));

    route->distance += modifiedNode->distance + newTrip->distance;
    route->remainingCapacity -= newTrip->finalNode->getProduction();

    route->trips.insert(route->trips.begin() + index, newTrip);
}

double Solution::calculateBenefit(Trip *trip, int typeIndex) { //TODO no es el beneficio
    return this->literCost[getBlendingType(typeIndex)] * trip->finalNode->getProduction() -
           this->kilometerCost * trip->distance;
}

Trip *Solution::newTrip(Node *node1, Node *node2, Route *route) {
    int distance(problemInstance->getDistance(node1, node2));
    auto trip = new Trip(node1, node2, distance);
    trip->setBenefit(calculateBenefit(trip, route->getTypeIndex()));
    trip->setRouteId(route->getId());
    return trip;
}

Trip *Solution::fakeTrip(Node *node1, Node *node2, Node *node3, Route *route) {
    int d1(problemInstance->getDistance(node1, node2));
    int d2(problemInstance->getDistance(node2, node3));
    int d3(problemInstance->getDistance(node1, node3));
    auto trip = new Trip(node1, node2, d1 + d2 - d3);
    trip->setBenefit(calculateBenefit(trip, route->getTypeIndex()));
    trip->setRouteId(route->getId());
    return trip;
}

int Solution::getCurrentType() { //si el ultimo es cero? devuelve -1
    for(int i: this->unsatisfiedDemand) {
        if (i > 0) {
            return this->routes.back()->getType();
        }
    }
    return -1; //si el ultimo tipo ya se suplio, devuelve -1
}

int Solution::getUnsatisfiedType(int from) { //TODO esta bien?
    for (int i = from ; i < this->unsatisfiedDemand.size() ; ++i) {
        if (this->unsatisfiedDemand[i] > 0) {
            return i;
        }
    }
    return -1; //si el ultimo tipo ya se suplio, devuelve -1
}

Node *Solution::getCurrentNode() {
    if (this->routes.back()->trips.empty()) {
        return this->plant;
    }
    return routes.back()->trips.back()->finalNode;
}

// entrega el camion mas grande
Truck *Solution::getNextTruck() {
    return *max_element(this->unusedTrucks.begin(), this->unusedTrucks.end());
}

void Solution::deleteRoutes(){
    for (Route *route:this->routes) {
        delete route;
    }
    this->routes.clear();
    this->routes.shrink_to_fit();
}

void Solution::removeTruck(Truck *truck) {
    auto it = find(this->unusedTrucks.begin(), this->unusedTrucks.end(), truck);
    long index = std::distance(this->unusedTrucks.begin(), it);
    this->unusedTrucks.erase(this->unusedTrucks.begin() + index);
}

void Solution::removeNode(Node *node) {
    auto it = find(this->unvisitedNodes.begin(), this->unvisitedNodes.end(), node);
    long index = std::distance(this->unvisitedNodes.begin(), it);
    this->unvisitedNodes.erase(this->unvisitedNodes.begin() + index);
    for (int i = node->getTypeIndex(); i < this->nodesXQuality.size(); ++i) {
        if (nodesXQuality[i] != 0) {
            this->nodesXQuality[i] -= 1;
        }
    }
}

void Solution::removeTrip(int tripIndex, Route *route) {
    Trip *currentTrip = route->trips[tripIndex]; // 0-x
    Trip *nextTrip = route->trips[tripIndex + 1];// x-0

    route->distance -= currentTrip->distance + nextTrip->distance;//0

    nextTrip->initialNode = currentTrip->initialNode; //x-0 -> 0-0
    nextTrip->distance = this->problemInstance->getDistance(nextTrip->initialNode, nextTrip->finalNode);//0
    nextTrip->setBenefit(this->calculateBenefit(nextTrip, route->getTypeIndex()));//0

    route->distance += nextTrip->distance;//0
    route->remainingCapacity += currentTrip->finalNode->getProduction();//0

    route->trips.erase(route->trips.begin() + tripIndex); // borrar 0-x
};


int Solution::getDemandSubtraction(int dda, int prod) {
    if (dda > 0) {
        if (dda - prod > 0) {
            return 0;
        } else {
            return -(dda - prod);
        }
    } else {
        return prod;
    }
}

void Solution::addBackToPlant(int i, Node *currentNode, Route *currentRoute, bool pos, bool repairing){
    if (currentRoute->trips.back()->finalNode != this->plant){
        if (this->unsatisfiedDemand[i] < 0 and pos) { // vuelve a planta si ya suplio la demanda
            Trip *toPlant = newTrip(currentNode, this->plant, currentRoute);
            currentRoute->distance += toPlant->distance;
            addTrip(toPlant, currentRoute);
            int actual(getUnsatisfiedType(0));
            if ((!repairing) and (actual != -1)) { // nueva ruta para el tipo de dda no satisfecho
                addRoute(actual+1);
            }
        }
    }
}

void Solution::updateDemands(int currentTypeIndex, Node *currentNode, Route *currentRoute, int production, bool repairing) {
    int subtracting = production;
    bool pos(false);
    for (int i = currentTypeIndex; i < this->unsatisfiedDemand.size(); ++i) {
        int aux(getDemandSubtraction(this->unsatisfiedDemand[i], subtracting));
        if (this->unsatisfiedDemand[i] > 0) {
            pos = true;
        }

        this->unsatisfiedDemand[i] -= subtracting;
        subtracting = aux;
        addBackToPlant(i, currentNode, currentRoute, pos, repairing);
    }
}

void Solution::resetDemands() {
    this->unsatisfiedDemand = this->newDemands;
    fill(this->recollected.begin(), this->recollected.end(), 0);
    for (Route *route: this->routes) {
        int type(0);
        int routeProd(0);
        for (Trip *trip: route->trips){
            routeProd += trip->finalNode->getProduction();
            if (type < trip->finalNode->getType()){
                type = trip->finalNode->getType();
            }
            this->recollected[trip->finalNode->getTypeIndex()] += trip->finalNode->getProduction();
        }
        int subtracting = routeProd;
        for (int i = type-1; i < this->unsatisfiedDemand.size(); ++i) {
            int aux(getDemandSubtraction(this->unsatisfiedDemand[i], subtracting));
            this->unsatisfiedDemand[i] -= subtracting;
            subtracting = aux;
        }
    }
}

void Solution::resetRouteFull() {
    for(Route *r: this->routes){ //si ninguno cabe, ie es full,
        r->full = true;
        for (int i = 0; i < this->nodesXQuality[r->getTypeIndex()]; ++i) {
            Node *option = this->unvisitedNodes[i];
            if (r->fitsInTruck(option)) {
                r->full = false;
            }
        }
    }
}

void Solution::updateSolution(Node *node, bool add) {
    if(add){
        removeNode(node);
        this->recollected[node->getTypeIndex()] += node->getProduction();
    }
    else{
        insertNode(node);
        this->recollected[node->getTypeIndex()] -= node->getProduction();

    }
    resetDemands();
}

void Solution::stepUpdateSolution(Trip *trip, Route *currentRoute, bool repairing) {
    currentRoute->distance += trip->distance;
    if (trip->finalNode != this->plant ) {
        int tripProduction(trip->finalNode->getProduction());
        this->recollected[trip->finalNode->getTypeIndex()] += tripProduction;
        currentRoute->remainingCapacity -= tripProduction;
        if (currentRoute->remainingCapacity == 0) {
            currentRoute->setFull();
        }
        updateDemands(currentRoute->getTypeIndex(), trip->finalNode, currentRoute, tripProduction, repairing);
        removeNode(trip->finalNode);
    }
}

void Solution::printAll() {
    cout << endl;
    cout << "**Solution**" << endl;

    double totalRecollected(0);
    cout << "recollected x quality: " << endl;
    for (int r: this->recollected) {
        totalRecollected += r;
        cout << r << endl;
    }

    cout << "Demand satisfaction: " << endl;
    for (int d: this->unsatisfiedDemand) {
        cout << d << endl;
    }

    double totalDemand(0);
    for (int q: this->newDemands) {
        totalDemand += q;
    }
    cout << "milk excess: " << totalRecollected - totalDemand << endl;

    cout << "# Unused trucks: " << this->unusedTrucks.size() << endl;
    for (Truck *t: this->unusedTrucks) {
        t->printAll();
    }

    cout << "# Unvisited nodes: " << this->unvisitedNodes.size() << endl;
    for (Node *n: this->unvisitedNodes) {
        n->printAll();
    }

    cout << "# nodes x quality: " << endl;
    for (int q: this->nodesXQuality) {
        cout << q << endl;
    }

    double totalDistance(0);
    for (Route *r: this->routes) {
        totalDistance += r->distance;
    }
    cout << "Distance cost: " << this->kilometerCost * totalDistance << endl;

    cout << "Milk cost: " << endl;
    double suma(0);
    for (int i = 0; i < this->recollected.size(); ++i) {
        suma += this->recollected[i] * this->literCost[i];
        cout << this->recollected[i] * this->literCost[i] << endl;
    }
    cout << "Total Milk cost: " << suma << endl;

    cout << "Total Benefit: " << to_string(getTotalBenefit())<< endl;

    for (Route *r: this->routes) {
        cout << endl;
        r->printAll();
    }
}

char Solution::getType(int i){
    if(i == 0){
        return 'C';
    }
    if(i == 1){
        return 'B';
    }
    if(i == 2){
        return 'A';
    }
}

vector<int> Solution::convertMilk(bool print) {
    int aux(0);
    for (int i: problemInstance->qualities) {
        if (i == 0) {
            aux += 1;
        }
    }
    vector<int> recollectedMilk = this->recollected;
    reverse(recollectedMilk.begin(), recollectedMilk.end());

    if (aux == problemInstance->getNumberOfQualities()) { //dar vuelta
        if (print) {
            for (int i = 0; i < recollectedMilk.size(); ++i) {
                cout << recollectedMilk[i] << " de leche " << getType(i) << " se usa como leche "
                     << getType(i) << " en la planta" << endl;
            }
        }
        return recollectedMilk;
    } else {
        vector<int> inPlant;
        vector<int> dda = this->newDemands;
        vector<int> totalMilk;

        for (int i = 0; i < problemInstance->getNumberOfQualities(); ++i) {
            totalMilk.push_back(0);
        }
        reverse(dda.begin(), dda.end());
        for (int i = 0; i < dda.size(); ++i) {
            for (int j = 0; j < recollectedMilk.size(); ++j) {
                if (dda[i] > 0) {
                    if (recollectedMilk[j] > 0) {
                        int diff = recollectedMilk[j] - dda[i];
                        if (diff >= 0) {
                            if (i == j) {
                                if (print) {
                                    cout << recollectedMilk[j] << " de leche " << getType(i) << " se usa como leche "
                                         << getType(j) << " en la planta" << endl;
                                }
                                totalMilk[j] += recollectedMilk[j];
                                recollectedMilk[j] = 0;
                                break;
                            } else {
                                recollectedMilk[j] -= dda[i];
                                if (print) {
                                    cout << dda[i] << " de leche " << getType(j) << " se usa como leche " << getType(i)
                                         << " en la planta" << endl;
                                }
                                totalMilk[i] += dda[i];
                                break;
                            }
                        } else {
                            if (print) {
                                cout << recollectedMilk[j] << " de leche " << getType(i) << " se usa como leche "
                                     << getType(j) << " en la planta" << endl;
                            }
                            totalMilk[j] += recollectedMilk[j];
                            recollectedMilk[j] -= dda[i];
                            dda[i] = -recollectedMilk[j];
                            recollectedMilk[j] = 0;
                        }
                    }
                } else {
                    break;
                }
            }
        }
        return totalMilk;
    }
}

void Solution::printSolution() {
    cout << "Cantidad mínima a recoger (litros): " << endl;
    for (int d: this->problemInstance->qualities) {
        cout << d << endl;
    }

    cout << "Cantidad ficticia a recoger (litros): " << endl;
    for (int d: this->newDemands) {
        cout << d << endl;
    }

    cout << "Flota Camiones: " << endl;
    cout << "Nro. Cam.: " << this->problemInstance->trucks.size() << endl;
    cout << "Capacidad (litros): " << this->problemInstance->trucks[0]->getTotalCapacity() << endl;

    cout << "Beneficio unitario ($/litro)" << endl;
    for (double i : this->literCost) {
        cout << i << endl;
    }

    cout << endl << "Resultados:" << endl;

    for(Route *r: this->routes){
        cout << endl << "Muestra Ruta Ordenada " << r->getId() << ":" << endl;
        for (Trip *t: r->trips){
            cout << " "<< t->finalNode->getId() << " ";
        }
    }
    cout << endl;

    double totalDistance(0);
    for (Route *r: this->routes) {
        totalDistance += r->distance;
    }

    vector<int> litersxtype(3, 0); //TODO a la mala el 3
    for (Route *r: this->routes) {
        int liters = this->problemInstance->trucks[r->truck->getId() - 1]->getTotalCapacity() - r->remainingCapacity;
        cout << liters
             << " litros de leche " << r->type << " llegan en Camion " << r->truck->getId() << endl;
        litersxtype[r->getTypeIndex()] += liters;
    }

    for (int i =0 ; i < this->recollected.size(); i++) {
        cout << "En total llegan " << litersxtype[i] << " litros leche tipo " << i << " la planta" << endl;
    }
    this->recollected = litersxtype; // en algun momento lo calcula mal

    vector<int> totalMilk = convertMilk(true);
    reverse(totalMilk.begin(), totalMilk.end());

    double suma(0);
    for (int i = 0; i < totalMilk.size(); ++i) {
        suma += totalMilk[i] * this->literCost[i];
    }

    cout << "COSTO_TPTE = " << totalDistance << endl;
    cout << "INGRESO_LECHE = " << to_string(suma) << endl;

    cout << "F.O. = " << to_string(getTotalBenefit()) << endl;
}

