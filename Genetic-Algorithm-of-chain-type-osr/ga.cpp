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
    vector<Individual> population(population_size);

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

                // ���U�ȥi�A�Ȫ��ϰ��
                int regionCount = 0;
                for (int r = 0; r < regionNum; ++r)
                    if (parameters.serviceRegion[idx][r] == 1) regionCount++;

                if (regionCount == 2) {
                    // �ȹ�u��ϫȤ�v���@�P�ҪO
                    if (!twoRegionCustomerBank.count(customerId)) {
                        // �Ĥ@���J��G�M�w�@�� serviceArea �P rotation�A�ؼҪO
                        int assignedServiceArea = rand() % 2 + 1; // 1 or 2
                        vector<Gene> templ;
                        templ.reserve(cargoCount);
                        for (int c = 0; c < cargoCount; ++c) {
                            Gene g;
                            g.cargoId     = c + 1;
                            g.customerId  = customerId;
                            g.serviceArea = assignedServiceArea;
                            g.rotation    = rand() % 6 + 1;
                            templ.push_back(g);
                        }
                        twoRegionCustomerBank[customerId] = std::move(templ);
                    }
                    // ����J��P�U�ȡG�����ƻs�ҪO�]�T�O�@�P�^
                    const auto& templ = twoRegionCustomerBank[customerId];
                    ind.chromosome.insert(ind.chromosome.end(), templ.begin(), templ.end());
                } else {
                    // �D��ϫȤ�G�����ҪO�A�C���U�ۥͦ��]�ӧA�쥻�W�h�^
                    int assignedServiceArea = 1; // �u�� 1 �ө� 0 �ӥi�A�ȰϮɩT�w�� 1�]�A���W�h�^
                    // �p�G�A�Q�G�� regionCount >= 2 �~�H���A�������䤣�|�i�ӡF�O�d�� 1 �Y�i

                    for (int c = 0; c < cargoCount; ++c) {
                        Gene gene;
                        gene.cargoId     = c + 1;
                        gene.customerId  = customerId;
                        gene.serviceArea = assignedServiceArea;
                        gene.rotation    = rand() % 6 + 1; // �i�C�����P
                        ind.chromosome.push_back(gene);
                    }
                }
            }
        }
        population.push_back(ind);
    }

    return population;
}