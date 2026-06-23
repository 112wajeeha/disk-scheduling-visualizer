#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "scan.h"

using namespace std;

vector<Step> runSCAN(DiskData data)
{
    vector<Step> steps;
    int current   = data.initial_head;
    int disk_max  = data.disk_size - 1;  // e.g. size=200 -> tracks 0..199
    bool goRight  = (data.direction == "right" || data.direction == "Right" ||
                     data.direction == "RIGHT");

    // Separate requests into left and right of current head
    vector<int> left, right;
    for (int r : data.requests)
    {
        if (r < current)
            left.push_back(r);
        else
            right.push_back(r);
    }

    // Sort: left descending (closest first going left), right ascending
    sort(left.begin(),  left.end(),  greater<int>());
    sort(right.begin(), right.end());

    cout << "\n--- SCAN Disk Scheduling ---\n";
    cout << "Head starts at : " << current << "\n";
    cout << "Direction      : " << data.direction << "\n\n";

    int step_num = 1;

    // Build the service order based on initial direction
    // First serve in the current direction, hit the boundary, then reverse
    if (goRight)
    {
        // Service requests to the RIGHT first
        for (int target : right)
        {
            Step s;
            s.from     = current;
            s.to       = target;
            s.distance = abs(s.to - s.from);
            cout << "Step " << step_num++ << ": "
                 << s.from << " -> " << s.to
                 << "  (seek: " << s.distance << ")\n";
            steps.push_back(s);
            current = target;
        }

        // Hit the right boundary (disk end) if there are requests on the left
        if (!left.empty())
        {
            Step boundary;
            boundary.from     = current;
            boundary.to       = disk_max;
            boundary.distance = abs(disk_max - current);
            cout << "Step " << step_num++ << ": "
                 << boundary.from << " -> " << boundary.to
                 << "  (seek: " << boundary.distance << ") [boundary]\n";
            steps.push_back(boundary);
            current = disk_max;

            // Now service requests on the LEFT (arm reverses)
            for (int target : left)
            {
                Step s;
                s.from     = current;
                s.to       = target;
                s.distance = abs(s.to - s.from);
                cout << "Step " << step_num++ << ": "
                     << s.from << " -> " << s.to
                     << "  (seek: " << s.distance << ")\n";
                steps.push_back(s);
                current = target;
            }
        }
    }
    else  // going LEFT initially
    {
        // Service requests to the LEFT first
        for (int target : left)
        {
            Step s;
            s.from     = current;
            s.to       = target;
            s.distance = abs(s.to - s.from);
            cout << "Step " << step_num++ << ": "
                 << s.from << " -> " << s.to
                 << "  (seek: " << s.distance << ")\n";
            steps.push_back(s);
            current = target;
        }

        // Hit the left boundary (track 0) if there are requests on the right
        if (!right.empty())
        {
            Step boundary;
            boundary.from     = current;
            boundary.to       = 0;
            boundary.distance = current;   // distance from current to 0
            cout << "Step " << step_num++ << ": "
                 << boundary.from << " -> " << boundary.to
                 << "  (seek: " << boundary.distance << ") [boundary]\n";
            steps.push_back(boundary);
            current = 0;

            // Now service requests on the RIGHT (arm reverses)
            for (int target : right)
            {
                Step s;
                s.from     = current;
                s.to       = target;
                s.distance = abs(s.to - s.from);
                cout << "Step " << step_num++ << ": "
                     << s.from << " -> " << s.to
                     << "  (seek: " << s.distance << ")\n";
                steps.push_back(s);
                current = target;
            }
        }
    }

    return steps;
}