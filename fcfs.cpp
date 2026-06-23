#include <iostream>
#include <vector>
#include <cmath>
#include "fcfs.h"

using namespace std;

vector<Step> runFCFS(DiskData data)
{
    vector<Step> steps;
    int current = data.initial_head;

    cout << "\n--- FCFS Disk Scheduling ---\n";
    cout << "Head starts at : " << current << "\n";
    cout << "Direction      : " << data.direction << "\n\n";

    for (int i = 0; i < (int)data.requests.size(); i++)
    {
        Step s;
        s.from     = current;
        s.to       = data.requests[i];
        s.distance = abs(s.to - s.from);

        cout << "Step " << i + 1 << ": "
             << s.from << " -> " << s.to
             << "  (seek: " << s.distance << ")\n";

        steps.push_back(s);
        current = s.to;
    }

    return steps;
}