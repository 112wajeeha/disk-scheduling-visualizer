#include <iostream>
#include "structures.h"

using namespace std;

DiskData getDiskInput() {
    DiskData data;
    
    cout << "\n--- Enter Simulation Data ---\n";
    
    // 1. Disk Size Input with Text Validation
    while (true) {
        cout << "Enter Total Disk Size (e.g., 200): ";
        if (cin >> data.disk_size) {
            if (data.disk_size > 0) break;
            cout << "[Error] Disk size must be greater than 0!\n";
        } else {
            cout << "[Error] Please enter a valid number, not text!\n";
            cin.clear(); 
            cin.ignore(10000, '\n'); 
        }
    }
    
    // 2. Initial Head Position Input with Text Validation
    while (true) {
        cout << "Enter Initial Head Position (0 to " << data.disk_size - 1 << "): ";
        if (cin >> data.initial_head) {
            if (data.initial_head >= 0 && data.initial_head < data.disk_size) break;
            cout << "[Error] Head must be within disk boundaries!\n";
        } else {
            cout << "[Error] Please enter a valid number, not text!\n";
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }
    
    // 3. Direction Input
    while (true) {
        cout << "Enter Direction (left/right): ";
        cin >> data.direction;
        if (data.direction == "left" || data.direction == "right") break;
        cout << "[Error] Invalid direction! Type 'left' or 'right'.\n";
    }
    
    // 4. Requests Input with Text Validation
    int num_requests;
    while (true) {
        cout << "How many track requests do you want to enter? ";
        if (cin >> num_requests && num_requests > 0) break;
        cout << "[Error] Please enter a valid number of requests!\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    data.requests.clear();
    for (int i = 0; i < num_requests; i++) {
        int track;
        while (true) {
            cout << "Enter Track Request " << i + 1 << " (0 to " << data.disk_size - 1 << "): ";
            if (cin >> track) {
                if (track >= 0 && track < data.disk_size) {
                    data.requests.push_back(track);
                    break;
                }
                cout << "[Error] Track out of bounds!\n";
            } else {
                cout << "[Error] Please enter a valid number!\n";
                cin.clear();
                cin.ignore(10000, '\n');
            }
        }
    }
    
    return data;
}