# include <iostream>
#include <vector>
#include <chrono>
#include "data.h"
#include "data.cpp"
#include "ga.cpp"

using namespace std;

int main(){
    srand(time(NULL));
    Data parameters;
    readParameters("customerInfo.csv", "goods.csv", "serviceArea.csv", "routes.csv", parameters); 
    printData(parameters); 
    vector<Individual> population = initializePopulation(populationSize, parameters);

    for (int i = 0; i < population.size(); ++i) {
        cout << "======== Individual " << i + 1 << " ========\n";
        for (size_t j = 0; j < population[i].chromosome.size(); ++j) {
            const Gene& g = population[i].chromosome[j];
            cout << "Gene " << j + 1 
                 << " | Customer: " << g.customerId
                 << " | CargoID: "  << g.cargoId 
                 << " | ServiceArea: " << g.serviceArea
                 << " | Rotation: " << g.rotation << "\n";
        }
        cout << endl;
    }
    return 0;
}
