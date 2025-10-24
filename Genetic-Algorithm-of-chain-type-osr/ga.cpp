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
    vector<Individual> population;
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

                // 計算該顧客可服務的區域數
                int regionCount = 0;
                for (int r = 0; r < regionNum; ++r)
                    if (parameters.serviceRegion[idx][r] == 1) regionCount++;

                if (regionCount == 2) {
                    // 兩區顧客：模板確保一致，但插入前要覆寫 routeArea
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
                            g.routeArea   = -1;                // 先留空，插入時再依 region 設定
                            templ.push_back(g);
                        }
                        twoRegionCustomerBank[customerId] = std::move(templ);
                    }
                    // 插入時把 routeArea 設為當前 region+1
                    const auto& templ = twoRegionCustomerBank[customerId];
                    for (auto g : templ) {
                        g.routeArea = region + 1;             // ★ 關鍵：來源路線區域
                        ind.chromosome.push_back(g);
                    }
                } else {
                    // 非兩區顧客：現場生成並直接設 routeArea
                    int assignedServiceArea = 1; // 你的規則
                    for (int c = 0; c < cargoCount; ++c) {
                        Gene gene;
                        gene.cargoId     = c + 1;
                        gene.customerId  = customerId;
                        gene.undecodedServiceArea = assignedServiceArea;
                        gene.undecodedRotation    = rand() % 6 + 1;
                        gene.routeArea   = region + 1;        // ★ 關鍵：來源路線區域
                        ind.chromosome.push_back(gene);
                    }
                }
            }
        }
        population.push_back(ind);
    }
    return population;
}

// 進行服務區域的解碼
void decodeServiceArea(Individual &indiv, const Data &parameters) {
    for (auto &gene : indiv.chromosome) {
        int customerIdx = gene.customerId - 1; //這是為了對應parameters內的矩陣格式

        vector<int> feasible_regions;
        // 取得客戶的可行服務區域
        for (int area = 0; area < regionNum; area++) {
            if (parameters.serviceRegion[customerIdx][area] == 1) {
                feasible_regions.push_back(area + 1); // 轉換為 {1, 2, 3}
            }
        }

        // 排序確保由小到大
        sort(feasible_regions.begin(), feasible_regions.end());

        if (gene.undecodedServiceArea == 1) {
            // 可行區域必定只有一個，直接使用
            gene.decodedServiceArea = feasible_regions[0];
        } else {
            int idx = gene.undecodedServiceArea - 1; // 編碼值從1開始
            if (idx < feasible_regions.size()) {
                gene.decodedServiceArea = feasible_regions[idx];
            } else {
                cerr << "customer " << gene.customerId << " area wrong" << endl;
            }
        }    
    }
    // 解碼完區域後，可以知道客戶真正的服務區域，接下來對各區域不屬於該路線的客戶進行刪除，留下只屬於該區域的
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
        // 正確透過 customerId 和 cargoId 找到貨物資訊
        const Cargo &cargo = cargoLookup.at(gene.customerId).at(gene.cargoId);
        vector<int> feasibleOrientations;
        for (int ori = 0; ori < 6; ori++) {
            if (cargo.orientation[ori] == 1) {
                feasibleOrientations.push_back(ori + 1);  // 方向1~6
            }
        }

        int orientationCount = feasibleOrientations.size();
        if (orientationCount == 0) {
            cerr << "error " << gene.customerId << "cargo " << gene.cargoId 
            << " no feasible orientation" << endl;
            continue;
        }
        // 依照你原本的解碼規則
        int decodedIndex = (gene.undecodedRotation % orientationCount);
        gene.decodedRotation = feasibleOrientations[decodedIndex];
    }
}

// 建立貨物對應表
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