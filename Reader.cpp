#include "Reader.h"

Reader::Reader(string option): filePath("Instances/instancia" + string(option) + ".mcsb") {}

Reader::~Reader() { cout << "Deleting File Reader" << endl; }


ProblemInstance *Reader::readInputFile(double temperature) {
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
    problemInstance = this->createInstance(temperature);

    this->findDef("Q:=");
    this->readTrucks(problemInstance);

    this->findDef("P:=");
    this->readQuality(problemInstance);

    this->findDef(":=");
    this->readNodes(problemInstance);

    this->findDef(":=");
    this->readDistances(problemInstance);

    input.close();

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

ProblemInstance *Reader::createInstance(double temperature) {
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
    auto *problemInstance = new ProblemInstance(matrix_size, temperature); //genera el espacio para la matriz
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

void Reader::readOutputs(vector<unsigned int> seeds, size_t beta, int gamma, int epsilon, bool corte) {
    ofstream myfile;
    string name = "Results/resumen.txt";
    myfile.open(name);

    myfile << "Numero de iteraciones: " << "2500 reset x 2000 iter" << "\n";
    myfile << "Numero de semillas: " << seeds.size() << "\n";
    myfile << "Parámetros: " <<  beta << "," << gamma << "," << epsilon << "\n";
    myfile << "Corte: " << corte << "\n";
    myfile << "\n";
    myfile << "\n";


    for (int instance = 1; instance < 7; ++instance) {
        vector<double> timesToBest, totalTimes, FOs;
        double objective(0);
        for (unsigned int seed: seeds) {
//            ifstream input2;
            string filePath2 = "Results/resultados_i-" + to_string(instance) + "_s-" + to_string(seed) + ".txt";
//            string filePath2= "Results/resultados_i-3_s-1505.txt";
            input.open(filePath2);

            if (!input.is_open()) {
                cerr << "Could not open the file: " << filePath2 << std::endl;
                exit(EXIT_FAILURE);
            }

            cout << "Reading: " << filePath2 << endl;

//            string line;
//            getline(input2, line);
//            cout << "line: " << line << endl;

            this->findDef("ejecucion:");
            double totalTime;
            input >> totalTime;
            totalTimes.push_back(totalTime);

            this->findDef("mejor:");
            double timeToBest;
            input >> timeToBest;
            timesToBest.push_back(timeToBest);

            this->findDef("F.O.");
            this->findDef("=");
            double FO;
            input >> FO;
            FOs.push_back(FO);


            this->findDef("Objetivo");
            this->findDef("=");
            input >> objective;

            input.close();
        }

        myfile << "\n";
        myfile << "\n";
        myfile << "\n";
        myfile << "Instancia: "<< instance << "\n";
        myfile << "\n";

        double promTotalTimes(0);
        for(double t: totalTimes) {
            promTotalTimes += t;
        }
        myfile << "Tiempo total: "<< promTotalTimes << "\n";
        myfile << "Tiempo total promedio por ejecucion: "<< promTotalTimes/totalTimes.size() << "\n";

        double promTimesToBest(0);
        for(double t: timesToBest) {
            promTimesToBest += t;
        }
        myfile << "Tiempo promedio al mejor: "<< promTimesToBest/timesToBest.size() << "\n";
        myfile << "\n";

        double promFOs(0);
        for(double t: FOs) {
            promFOs += t;
        }
        myfile << "FO promedio: "<< promFOs/FOs.size() << "\n";
        myfile << "FO maximo: " << *max_element(begin(FOs), end(FOs)) << '\n';
        myfile << "FO minimo: " << *min_element(begin(FOs), end(FOs)) << '\n';
        myfile << "Objetivo: " << objective << '\n';
        myfile << "Cercania: " << 100*(*max_element(begin(FOs), end(FOs))-objective)/objective << '\n';
        myfile << "\n";

        myfile << "Todas las FOs: " << '\n';
        for(double t: FOs) {
            myfile << t << " ";
        }
        myfile << '\n';
        myfile << '\n';

        myfile << "Todos los tiempos totales: " << '\n';
        for(double t: totalTimes) {
            myfile << t << " ";
        }
        myfile << '\n';
        myfile << '\n';

        myfile << "Todos los tiempos al mejor: " << '\n';
        for(double t: timesToBest) {
            myfile << t << " ";
        }
        timesToBest.clear();
        timesToBest.shrink_to_fit();

        totalTimes.clear();
        totalTimes.shrink_to_fit();

        FOs.clear();
        FOs.shrink_to_fit();
    }
    myfile << '\n';
}
