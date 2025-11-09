# include <iostream>
#include <vector>
#include <chrono>
#include "data.h"
#include "data.cpp"
#include "ga.cpp"

using namespace std;

int main(){

    srand(time(NULL));

    // 讀檔
    Data parameters;
    readParameters("customerInfo.csv", "goods.csv", "serviceArea.csv", "routes.csv", parameters);
    
    // 編碼 & 初始母體生成
    vector<Individual> population = initializePopulation(populationSize, parameters);

    // 建立貨物對照表，方便進行解碼和貨物裝載對應
    auto cargoLookUp = createCargoLookup(parameters);

    // 解碼
    decodePopulation(population,parameters,cargoLookUp);
    evaluateFitness(population[0],parameters);
    cout << population[0].fitness[0] << endl;
    cout << population[0].fitness[1]; 
    for (int i = 1; i <= regionNum; ++i) {
        cout << "Truck " << i << " cargos:\n";
        for (const auto& g : population[0].selfOwnedTrucks[i].assignedCargo) {
            cout << "  Customer: " << g.customerId
                << " CargoID: " << g.cargoId
                << " Position: (" << g.position[0] << ", "
                                << g.position[1] << ", "
                                << g.position[2] << ")\n";
        }
    }

    return 0;
}
