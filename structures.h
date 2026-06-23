#pragma once
#include <vector>
#include <string>
using namespace std;

struct DiskData {
    int disk_size;
    int initial_head;
    string direction;
    vector<int> requests;
};

struct Step {
    int from;
    int to;
    int distance;
};