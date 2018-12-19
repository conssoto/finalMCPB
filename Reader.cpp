#include "Reader.h"

Reader::Reader(string option): filePath("Instances/instancia" + string(option) + ".mcsb") {}

Reader::~Reader() { cout << "Deleting File Reader" << endl; }


ProblemInstance *Reader::readInputFile() {
    ProblemInstance *problemInstance;

    string filePath = this->filePath;
    input.open(filePath);

    if (!input.is_open()) {
        cerr << "Could not open the file: " << filePath << std::endl;
        exit(EXIT_FAILURE);
    }

    string line;
    getline(input, line);
    cout << "Reading: " << filePath << endl;

    vector<string> data;

    this->findDef("Nodos:=");
    problemInstance = this->createInstance();

    this->findDef("Q:=");
    this->readTrucks(problemInstance);

    this->findDef("P:=");
    this->readQuality(problemInstance);

    this->findDef(":=");
    this->readNodes(problemInstance);

    this->findDef(":=");
    this->readDistances(problemInstance);

    return problemInstance;
}


void Reader::findDef(string def) {
    string word;
    while (true) {
        input >> word;
        if (word.find(def) != std::string::npos) {
            break;
        }
    }
}

ProblemInstance *Reader::createInstance() {
    string node;
    int matrix_size(1); //Considerando la planta
    input >> node;
    while (true) {
        input >> node;
        matrix_size++;
        if (node.find(';') != std::string::npos) {
            break;
        }
    }
    auto *problemInstance = new ProblemInstance(matrix_size); //genera el espacio para la matriz
    return problemInstance;
}

void Reader::readTrucks(ProblemInstance *problemInstance) {
    string id;
    int totalCapacity;
    while (true) {
        input >> id;
        if (id.find(';') != std::string::npos) {
            break;
        }
        input >> totalCapacity;
        auto *truck = new Truck(stoi(id), totalCapacity);
        problemInstance->addTruck(truck);
    }
}

void Reader::readQuality(ProblemInstance *problemInstance) {
    string dda;
    while (true) {
        input >> dda;
        if (dda.find(';') != std::string::npos) {
            break;
        }
        problemInstance->addQuality(stoi(dda));
    }
}

void Reader::readNodes(ProblemInstance *problemInstance) {
    string id;
    int production;
    int type;
    input >> id >> production; //planta
    while (true) {
        input >> id;
        if (id.find(';') != std::string::npos) {
            problemInstance->sortNodes();
            break;
        }
        input >> production >> type;
        auto *node = new Node(stoi(id), production, type);
        problemInstance->addNode(node);
    }
}

void Reader::readDistances(ProblemInstance *problemInstance) {
    string id;
    string distance;
    while (true) {
        int count = 0;
        input >> id;
        if (id.find(';') != std::string::npos) {
            break;
        }
        while (count <= problemInstance->getNumberOfNodes()) {
            input >> distance;
            if (id == "80") {
                id = "0";
            }
            if (distance == ".") {
                distance = "0";
            }
            problemInstance->setDistance(stoi(id), count, stoi(distance));
            count++;
        }
    }
}
