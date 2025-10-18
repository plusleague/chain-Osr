// 處理GA相關程式，包括編碼、解碼、計算適應度等等
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

// 產生初始染色體，包含編碼
vector<Individual> initializePopulation(const int population_size, const Data& parameters) {

    // 1. 先生成初始種群，Individual分別代表的是一條染色體，種群內會有 populationSize 個染色體
    vector<Individual> population(population_size);

    for (int i = 0; i < population_size; ++i) {
        Individual ind;
        vector <int> remembered;
        unordered_map<int, vector<Gene>> twoRegionCustomerBank;
        // 依照每個區域的路線順序建立基因序列
        for (int region = 0; region < regionNum; ++region) {
            const auto& route = parameters.route[region];
            for (int customerId : route) {
                if (customerId <= 0) continue;
                int idx = customerId - 1;

                int cargoCount = parameters.cargoNumber[idx];
                if (cargoCount <= 0) cargoCount = 1;

                // 該顧客可服務的區域數
                int regionCount = 0;
                for (int r = 0; r < regionNum; ++r)
                    if (parameters.serviceRegion[idx][r] == 1) regionCount++;

                if (regionCount == 2) {
                    // 僅對「兩區客戶」做一致模板
                    if (!twoRegionCustomerBank.count(customerId)) {
                        // 第一次遇到：決定一次 serviceArea 與 rotation，建模板
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
                    // 後續遇到同顧客：直接複製模板（確保一致）
                    const auto& templ = twoRegionCustomerBank[customerId];
                    ind.chromosome.insert(ind.chromosome.end(), templ.begin(), templ.end());
                } else {
                    // 非兩區客戶：不做模板，每次各自生成（照你原本規則）
                    int assignedServiceArea = 1; // 只有 1 個或 0 個可服務區時固定為 1（你的規則）
                    // 如果你想：當 regionCount >= 2 才隨機，那此分支不會進來；保留為 1 即可

                    for (int c = 0; c < cargoCount; ++c) {
                        Gene gene;
                        gene.cargoId     = c + 1;
                        gene.customerId  = customerId;
                        gene.serviceArea = assignedServiceArea;
                        gene.rotation    = rand() % 6 + 1; // 可每次不同
                        ind.chromosome.push_back(gene);
                    }
                }
            }
        }
        population.push_back(ind);
    }

    return population;
}