#include "Node.h"

Node::Node(int id, int production, int type): id(id), production(production), type(type) {}

Node::~Node() = default;


int Node::getId() const { return this->id; }

int Node::getProduction() const { return this->production; }

int Node::getType() const { return this->type; }

int Node::getTypeIndex() const { return this->type - 1; }

void Node::printAll() {
    cout << "Node:  " << this->getId() << "\tT: " << this->getType() << "\tP: " << this->getProduction() << endl;
}

bool sortByType(const Node *n1, const Node *n2){ return n1->getType() < n2->getType(); }
