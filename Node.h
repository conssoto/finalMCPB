#pragma once
#include <iostream>

using namespace std;

class Node {
private:
    int id;
    int production;
    int type;

public:
    Node(int id, int production, int type);
    ~Node();

    int getId() const;
    int getProduction() const;
    int getType() const;
    int getTypeIndex() const;

    void printAll();

    bool operator==(const Node& node) const{return this->getId() == node.getId();}
    bool operator!=(const Node& node) const{ return !operator==(node);}
};

bool sortByType(const Node *n1, const Node *n2);
