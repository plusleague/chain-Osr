# include <iostream>
#include <vector>
#include <chrono>
#include "data.h"
#include "data.cpp"
#include "ga.cpp"

using namespace std;

int main(){

    srand(time(NULL));

    // Ū��
    Data parameters;
    readParameters("customerInfo.csv", "goods.csv", "serviceArea.csv", "routes.csv", parameters);
    
    // �s�X & ��l����ͦ�
    vector<Individual> population = initializePopulation(populationSize, parameters);

    // �إ߳f����Ӫ�A��K�i��ѽX�M�f���˸�����
    auto cargoLookUp = createCargoLookup(parameters);

    // �ѽX
    decodePopulation(population,parameters,cargoLookUp);

    return 0;
}
