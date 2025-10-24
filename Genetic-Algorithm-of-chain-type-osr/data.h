// data.h
#ifndef DATA_H
#define DATA_H
#include <vector>
#include <string>
#include <string>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using namespace std;
const int N = 13;
const int Customer = 12;
const int regionNum = 4;
const int selfOwnedTruck = regionNum;
const int rentedTruck = regionNum;
const int populationSize = 1;
const int maxGenerations = 10000;
const double crossoverRate = 0.8;
const double mutationRate = 0.1;
const double eliteRatio = 0.4;

struct Cargo{
    int customerId;
    int cargoId;
    int volume;
    int lwh[3];
    int orientation[6];
    int fragility;
};

struct Data{
    int cargoNumber[Customer];
    int totalVolume[Customer];
    int serviceRegion[Customer][regionNum];
    vector <Cargo> cargoInformation;
    vector <int> route[regionNum];
};

struct Gene {
    int customerId;
    int cargoId;
    int routeArea;   
    int undecodedServiceArea;
    int decodedServiceArea = 0;       
    int undecodedRotation; 
    int decodedRotation = 0;       
    int position[3] = { -1, -1, -1 };
};

struct Truck{
    int truckId;
    int length = 300;
    int width = 170;
    int height = 165;
    int loadedVolume;
    vector<Gene> assignedCargo; 
};

struct Individual {
    vector<Gene> chromosome;
    Truck selfOwnedTrucks[regionNum + 1];
    vector<Truck> rentedTrucks;
    double fitness = 0.0;
    double rentedVehicleLoadingCost = 0.0;
    double maxVolumeDifferenceOfEachCar = 0.0;
};

void readParameters(const string& customerInfo, const string& goods, const string& serviceArea, const string& routes, Data& parameter);
void printData(const Data& data);
void printChromosomeInfo(const Individual& indiv);
#endif