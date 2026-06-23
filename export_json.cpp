#include <fstream>
#include <iostream>
#include "export_json.h"

using namespace std;

void exportToJson(DiskData data, vector<Step> steps, int total_seek, string algorithm)
{
    ofstream file("result.json");

    if (!file.is_open())
    {
        cout << "[ERROR] Could not write result.json\n";
        return;
    }

    file << "{\n";
    file << "  \"algorithm\": \""  << algorithm          << "\",\n";
    file << "  \"disk_size\": "    << data.disk_size      << ",\n";
    file << "  \"head\": "         << data.initial_head   << ",\n";
    file << "  \"direction\": \""  << data.direction      << "\",\n";

    // requests array
    file << "  \"requests\": [";
    for (int i = 0; i < (int)data.requests.size(); i++)
    {
        file << data.requests[i];
        if (i < (int)data.requests.size() - 1) file << ", ";
    }
    file << "],\n";

    // steps array
    file << "  \"steps\": [\n";
    for (int i = 0; i < (int)steps.size(); i++)
    {
        file << "    { \"from\": "     << steps[i].from
             << ", \"to\": "          << steps[i].to
             << ", \"distance\": "    << steps[i].distance << " }";
        if (i < (int)steps.size() - 1) file << ",";
        file << "\n";
    }
    file << "  ],\n";

    file << "  \"total_seek\": " << total_seek << "\n";
    file << "}\n";

    file.close();
}