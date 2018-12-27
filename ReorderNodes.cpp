#include "ReorderNodes.h"

ReorderNodes::ReorderNodes()=default;

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
        for (int i = 0; i < route->trips.size() - 1; ++i) {

            removeTrip(i, route, solution);
            setBest(route, solution->problemInstance);
            solution->insertTrip(route, this->bestIndex, this->node);
        }
    }
//    cout << endl;
}

void ReorderNodes::printRoute(Route *route){
    cout << "Total distance: " << route->distance << endl;
    for(Trip *trip: route->trips){
        cout << trip->initialNode->getId() << "-" << trip->finalNode->getId() << ":" << trip->distance << "     ";
    }
    cout << endl;
}

