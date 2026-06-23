#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <windows.h>
#include "structures.h"
#include "input_handler.h"
#include "fcfs.h"
#include "scan.h"
#include "cscan.h"
#include "export_json.h"

using namespace std;

void openFile(const string& filename)
{
    char fullPath[MAX_PATH];
    GetFullPathNameA(filename.c_str(), MAX_PATH, fullPath, NULL);
    string path(fullPath);
    for (char &c : path) if (c == '\\') c = '/';
    string url = "file:///" + path;
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

string stepsToJson(const vector<Step>& steps)
{
    string out = "[\n";
    for (int i = 0; i < (int)steps.size(); i++)
    {
        out += "      { \"from\": "    + to_string(steps[i].from)
             + ", \"to\": "           + to_string(steps[i].to)
             + ", \"distance\": "     + to_string(steps[i].distance) + " }";
        if (i < (int)steps.size() - 1) out += ",";
        out += "\n";
    }
    out += "    ]";
    return out;
}

void exportCompareJson(DiskData data,
                       vector<Step> fcfsSteps, int fcfsSeek,
                       vector<Step> scanSteps, int scanSeek,
                       vector<Step> cscanSteps, int cscanSeek)
{
    ofstream file("compare_result.json");
    if (!file.is_open()) { cout << "[ERROR] Could not write compare_result.json\n"; return; }

    // requests array string
    string reqs = "[";
    for (int i = 0; i < (int)data.requests.size(); i++) {
        reqs += to_string(data.requests[i]);
        if (i < (int)data.requests.size() - 1) reqs += ", ";
    }
    reqs += "]";

    file << "{\n";
    file << "  \"disk_size\": " << data.disk_size << ",\n";
    file << "  \"head\": "      << data.initial_head << ",\n";
    file << "  \"direction\": \"" << data.direction << "\",\n";
    file << "  \"requests\": "  << reqs << ",\n";

    file << "  \"FCFS\": { \"total_seek\": " << fcfsSeek
         << ", \"steps\": " << stepsToJson(fcfsSteps) << " },\n";

    file << "  \"SCAN\": { \"total_seek\": " << scanSeek
         << ", \"steps\": " << stepsToJson(scanSteps) << " },\n";

    file << "  \"CSCAN\": { \"total_seek\": " << cscanSeek
         << ", \"steps\": " << stepsToJson(cscanSteps) << " }\n";

    file << "}\n";
    file.close();
}

int main()
{
    int choice;
    DiskData current_data;
    bool is_data_loaded = false;

    while (true)
    {
        cout << "\n===================================\n";
        cout << "     DISK SCHEDULING SIMULATOR     \n";
        cout << "===================================\n";
        cout << "1. Enter Disk Data & Requests\n";
        cout << "2. Run FCFS Algorithm\n";
        cout << "3. Run SCAN Algorithm\n";
        cout << "4. Run C-SCAN Algorithm\n";
        cout << "5. Compare All Algorithms\n";
        cout << "6. Exit\n";
        cout << "===================================\n";
        cout << "Enter your choice (1-6): ";

        if (!(cin >> choice))
        {
            cout << "\n[Error] Please enter a number between 1 and 6!\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 1)
        {
            current_data   = getDiskInput();
            is_data_loaded = true;
            cout << "\n[SUCCESS] Data successfully saved & validated!\n";
        }
        else if (choice >= 2 && choice <= 4)
        {
            if (!is_data_loaded) { cout << "\n[WARNING] Please enter disk data first (Option 1)!\n"; continue; }

            vector<Step> result;
            string algo_name;

            if      (choice == 2) { result = runFCFS(current_data);  algo_name = "FCFS"; }
            else if (choice == 3) { result = runSCAN(current_data);  algo_name = "SCAN"; }
            else if (choice == 4) { result = runCSCAN(current_data); algo_name = "C-SCAN"; }

            if (!result.empty())
            {
                int total_seek = 0;
                for (auto s : result) total_seek += s.distance;
                cout << "\nTotal Seek Time : " << total_seek << "\n";
                exportToJson(current_data, result, total_seek, algo_name);
                cout << "[SUCCESS] result.json written.\n";
                cout << "Opening dashboard...\n";
                openFile("index.html");
                cout << "[INFO] Click 'Load result.json' in the browser.\n";
            }
        }
        else if (choice == 5)
        {
            if (!is_data_loaded) { cout << "\n[WARNING] Please enter disk data first (Option 1)!\n"; continue; }

            cout << "\n[INFO] Running all 3 algorithms...\n";

            vector<Step> fcfsSteps  = runFCFS(current_data);
            int fcfsSeek = 0; for (auto s : fcfsSteps)  fcfsSeek  += s.distance;
            cout << "[OK] FCFS  seek = " << fcfsSeek << "\n";

            vector<Step> scanSteps  = runSCAN(current_data);
            int scanSeek = 0; for (auto s : scanSteps)  scanSeek  += s.distance;
            cout << "[OK] SCAN  seek = " << scanSeek << "\n";

            vector<Step> cscanSteps = runCSCAN(current_data);
            int cscanSeek = 0; for (auto s : cscanSteps) cscanSeek += s.distance;
            cout << "[OK] C-SCAN seek = " << cscanSeek << "\n";

            exportCompareJson(current_data,
                              fcfsSteps,  fcfsSeek,
                              scanSteps,  scanSeek,
                              cscanSteps, cscanSeek);

            cout << "\n[SUCCESS] compare_result.json written.\n";
            cout << "Opening comparison dashboard...\n";
            cout << "[INFO] Click 'Load compare_result.json' in the browser.\n";
            openFile("compare.html");
        }
        else if (choice == 6)
        {
            cout << "\nExiting program. Goodbye!\n";
            break;
        }
        else
        {
            cout << "\n[Error] Invalid choice! Please select between 1 and 6.\n";
        }
    }
    return 0;
}