#include "data.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// 處理讀參數相關資料

void readGoodsCSV(const string& filename, Data& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open goods file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        Cargo c;

        getline(ss, value, ','); c.customerId = stoi(value);
        getline(ss, value, ','); c.cargoId = stoi(value);
        getline(ss, value, ','); c.volume = stoi(value);
        getline(ss, value, ','); c.lwh[0] = stoi(value);
        getline(ss, value, ','); c.lwh[1] = stoi(value);
        getline(ss, value, ','); c.lwh[2] = stoi(value);
        for (int i = 0; i < 6; ++i) {
            getline(ss, value, ','); c.orientation[i] = stoi(value);
        }
        getline(ss, value, ','); c.fragility = stoi(value);

        data.cargoInformation.push_back(c);
    }
    file.close();
}

// 讀取服務區域
void readServiceAreaCSV(const string& filename, Data& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open service area file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        int customer_id;

        getline(ss, value, ',');
        customer_id = stoi(value);

        for (int i = 0; i < regionNum; ++i) {
            getline(ss, value, ',');
            data.serviceRegion[customer_id - 1][i] = stoi(value);
        }
    }
    file.close();
}

// 讀取貨物數量
void readCustomerInfoCSV(const string& filename, Data& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open cargo count file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        int customer_id, count, totalVolume;

        getline(ss, value, ',');
        customer_id = stoi(value);
        getline(ss, value, ',');
        count = stoi(value);
        getline(ss, value, ',');
        totalVolume = stoi(value);

        data.cargoNumber[customer_id - 1] = count;
        data.totalVolume[customer_id - 1] = totalVolume;
    }
    file.close();
}

void readRouteToCSV(const string& filename, Data& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Cannot open cargo count file: " << filename << endl;
        return;
    }
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        int region;
        vector <int> route;
        getline(ss, value, ',');
        region = stoi(value);
        while (getline(ss, value, ',')) {
            if (value.empty()) continue;  // 空欄略過
            int node = stoi(value);
            if (node > 0) route.push_back(node); // 忽略 depot
        }
        if (region >= 0 && region < regionNum)
            data.route[region] = route;
        else
            cerr << "Invalid region index: " << region << endl;
    }
    file.close();
}
void readParameters(const string& customerInfo, const string& goods, const string& serviceArea, const string& routes, Data& parameter) {
    readCustomerInfoCSV(customerInfo, parameter);
    readGoodsCSV(goods, parameter);
    readServiceAreaCSV(serviceArea, parameter);
    readRouteToCSV(routes, parameter);
}

void printData(const Data& data) {
    cout << "===== Cargo Info =====" << endl;
    for (size_t i = 0; i < data.cargoInformation.size(); ++i) {
        cout << "Cargo " << i << ": ";
        cout << "CustomerId=" << data.cargoInformation[i].customerId
            << ", volume=" << data.cargoInformation[i].volume << endl;
    }

    cout << "\n===== Customer Cargo Number & Volume =====" << endl;
    for (int i = 0; i < Customer; ++i) {
        cout << "Customer " << i
            << " | CargoNum=" << data.cargoNumber[i]
            << " | TotalVol=" << data.totalVolume[i]
            << endl;
    }

    cout << "\n===== Service Region Assignment =====" << endl;
    for (int i = 0; i < Customer; ++i) {
        cout << "Customer " << i << ": ";
        for (int r = 0; r < regionNum; ++r) {
            cout << data.serviceRegion[i][r] << " ";
        }
        cout << endl;
    }

    cout << "\n===== Routes =====" << endl;
    for (int r = 0; r < regionNum; ++r) {
        if (data.route[r].empty()) continue;
        cout << "Region " << r + 1 << ": ";
        for (int node : data.route[r]) {
            cout << node << " ";
        }
        cout << endl;
    }
    cout << "=================================================" << endl;
}
