#include "ReorderNodes.h"

ReorderNodes::ReorderNodes(int ri):randomIterations(ri){};

ReorderNodes::~ReorderNodes()=default;

void ReorderNodes::removeTrip(int tripIndex, Route *route, Solution *solution) {
    this->bestIndex = tripIndex;
    this->node = route->trips[tripIndex]->finalNode;
    this->bestDistance = route->trips[tripIndex]->distance + route->trips[tripIndex + 1]->distance;

    solution->removeTrip(tripIndex, route);

    this->bestDistance -= route->trips[tripIndex]->distance;
};

void ReorderNodes::setBest(Route *route, ProblemInstance *problemInstance) {
    for (int i = 1; i < route->trips.size(); ++i) {
        int a(problemInstance->getDistance(route->trips[i]->initialNode, this->node));
        int b(problemInstance->getDistance(this->node, route->trips[i]->finalNode));
        if (a + b - route->trips[i]->distance <= this->bestDistance and i != this->bestIndex) {
            this->bestDistance = a + b - route->trips[i]->distance;
            this->bestIndex = i;
            break;
        }
    }
}

void ReorderNodes::movement(Solution *solution) {
//    cout << endl <<  "ReorderNodes: " << endl;
    for (Route *route: solution->routes) {
        if(this->randomIterations != 0){
            for (int i = 0; i < this->randomIterations ; ++i) {
                int index = solution->random_int_number(0, (int)route->trips.size()-1);
                removeTrip(index, route, solution);
                setBest(route, solution->problemInstance);
                solution->insertTrip(route, this->bestIndex, this->node);
            }
        }
        else{
            for (int i = 0; i < route->trips.size() - 1; ++i) {
                removeTrip(i, route, solution);
                setBest(route, solution->problemInstance);
                solution->insertTrip(route, this->bestIndex, this->node);
            }
        }
    }
}

void ReorderNodes::printRoute(Route *route){
    cout << "Total distance: " << route->distance << endl;
    for(Trip *trip: route->trips){
        cout << trip->initialNode->getId() << "-" << trip->finalNode->getId() << ":" << trip->distance << "     ";
    }
    cout << endl;
}

