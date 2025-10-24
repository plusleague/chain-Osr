// �B�zGA�����{���A�]�A�s�X�B�ѽX�B�p��A���׵���
#include "data.h"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <utility>

using namespace std;

// ���ͪ�l�V����A�]�t�s�X
vector<Individual> initializePopulation(const int population_size, const Data& parameters) {

    // 1. ���ͦ���l�ظs�AIndividual���O�N���O�@���V����A�ظs���|�� populationSize �ӬV����
    vector<Individual> population;
    for (int i = 0; i < population_size; ++i) {
        Individual ind;
        vector <int> remembered;
        unordered_map<int, vector<Gene>> twoRegionCustomerBank;
        // �̷ӨC�Ӱϰ쪺���u���ǫإ߰�]�ǦC
        for (int region = 0; region < regionNum; ++region) {
            const auto& route = parameters.route[region];
            for (int customerId : route) {
                if (customerId <= 0) continue;
                int idx = customerId - 1;

                int cargoCount = parameters.cargoNumber[idx];
                if (cargoCount <= 0) cargoCount = 1;

                // �p����U�ȥi�A�Ȫ��ϰ��
                int regionCount = 0;
                for (int r = 0; r < regionNum; ++r)
                    if (parameters.serviceRegion[idx][r] == 1) regionCount++;

                if (regionCount == 2) {
                    // ����U�ȡG�ҪO�T�O�@�P�A�����J�e�n�мg routeArea
                    if (!twoRegionCustomerBank.count(customerId)) {
                        int assignedServiceArea = rand() % 2 + 1; // 1 or 2
                        vector<Gene> templ;
                        templ.reserve(cargoCount);
                        for (int c = 0; c < cargoCount; ++c) {
                            Gene g;
                            g.cargoId     = c + 1;
                            g.customerId  = customerId;
                            g.undecodedServiceArea = assignedServiceArea;
                            g.undecodedRotation    = rand() % 6 + 1;
                            g.routeArea   = -1;                // ���d�šA���J�ɦA�� region �]�w
                            templ.push_back(g);
                        }
                        twoRegionCustomerBank[customerId] = std::move(templ);
                    }
                    // ���J�ɧ� routeArea �]����e region+1
                    const auto& templ = twoRegionCustomerBank[customerId];
                    for (auto g : templ) {
                        g.routeArea = region + 1;             // �� ����G�ӷ����u�ϰ�
                        ind.chromosome.push_back(g);
                    }
                } else {
                    // �D����U�ȡG�{���ͦ��ê����] routeArea
                    int assignedServiceArea = 1; // �A���W�h
                    for (int c = 0; c < cargoCount; ++c) {
                        Gene gene;
                        gene.cargoId     = c + 1;
                        gene.customerId  = customerId;
                        gene.undecodedServiceArea = assignedServiceArea;
                        gene.undecodedRotation    = rand() % 6 + 1;
                        gene.routeArea   = region + 1;        // �� ����G�ӷ����u�ϰ�
                        ind.chromosome.push_back(gene);
                    }
                }
            }
        }
        population.push_back(ind);
    }
    return population;
}

// �i��A�Ȱϰ쪺�ѽX
void decodeServiceArea(Individual &indiv, const Data &parameters) {
    for (auto &gene : indiv.chromosome) {
        int customerIdx = gene.customerId - 1; //�o�O���F����parameters�����x�}�榡

        vector<int> feasible_regions;
        // ���o�Ȥ᪺�i��A�Ȱϰ�
        for (int area = 0; area < regionNum; area++) {
            if (parameters.serviceRegion[customerIdx][area] == 1) {
                feasible_regions.push_back(area + 1); // �ഫ�� {1, 2, 3}
            }
        }

        // �ƧǽT�O�Ѥp��j
        sort(feasible_regions.begin(), feasible_regions.end());

        if (gene.undecodedServiceArea == 1) {
            // �i��ϰ쥲�w�u���@�ӡA�����ϥ�
            gene.decodedServiceArea = feasible_regions[0];
        } else {
            int idx = gene.undecodedServiceArea - 1; // �s�X�ȱq1�}�l
            if (idx < feasible_regions.size()) {
                gene.decodedServiceArea = feasible_regions[idx];
            } else {
                cerr << "customer " << gene.customerId << " area wrong" << endl;
            }
        }    
    }
    // �ѽX���ϰ��A�i�H���D�Ȥ�u�����A�Ȱϰ�A���U�ӹ�U�ϰ줣�ݩ�Ӹ��u���Ȥ�i��R���A�d�U�u�ݩ�Ӱϰ쪺
    indiv.chromosome.erase(
        remove_if(
            indiv.chromosome.begin(),
            indiv.chromosome.end(),
            [](const Gene& g) {
                return g.routeArea != g.decodedServiceArea;
            }),
        indiv.chromosome.end()
    );
}

void decodeCargoRotation(Individual &indiv, const Data &parameters,const unordered_map<int, unordered_map<int, Cargo>> &cargoLookup) {
    for (auto &gene : indiv.chromosome) {
        // ���T�z�L customerId �M cargoId ���f����T
        const Cargo &cargo = cargoLookup.at(gene.customerId).at(gene.cargoId);
        vector<int> feasibleOrientations;
        for (int ori = 0; ori < 6; ori++) {
            if (cargo.orientation[ori] == 1) {
                feasibleOrientations.push_back(ori + 1);  // ��V1~6
            }
        }

        int orientationCount = feasibleOrientations.size();
        if (orientationCount == 0) {
            cerr << "error " << gene.customerId << "cargo " << gene.cargoId 
            << " no feasible orientation" << endl;
            continue;
        }
        // �̷ӧA�쥻���ѽX�W�h
        int decodedIndex = (gene.undecodedRotation % orientationCount);
        gene.decodedRotation = feasibleOrientations[decodedIndex];
    }
}

// �إ߳f��������
unordered_map<int, unordered_map<int, Cargo>> createCargoLookup(const Data &parameters) {
    unordered_map<int, unordered_map<int, Cargo>> cargoLookup;
    for (const auto &cargo : parameters.cargoInformation) {
        cargoLookup[cargo.customerId][cargo.cargoId] = cargo;
    }
    return cargoLookup;
}

void decodePopulation(vector<Individual>& decodedPopulation, const Data &parameters, const unordered_map<int, unordered_map<int, Cargo>>& cargoLookup){
   for (int i = 0; i < decodedPopulation.size(); ++i) {
        decodeServiceArea(decodedPopulation[i], parameters);
        decodeCargoRotation(decodedPopulation[i], parameters, cargoLookup);
    }
}