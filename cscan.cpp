#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "cscan.h"

using namespace std;

vector<Step> runCSCAN(DiskData data)
{
    vector<Step> steps;

    int current  = data.initial_head;
    int disk_max = data.disk_size - 1;

    bool goRight = !(data.direction == "left" ||
                     data.direction == "Left" ||
                     data.direction == "LEFT");

    cout << "\n--- C-SCAN Disk Scheduling ---\n";
    cout << "Head starts at : " << current << "\n";
    cout << "Direction      : " << data.direction << "\n\n";

    int step_num = 1;

    if (goRight)
    {
        vector<int> rightReqs, leftReqs;

        for (int r : data.requests)
        {
            if (r >= current) rightReqs.push_back(r);
            else leftReqs.push_back(r);
        }

        sort(rightReqs.begin(), rightReqs.end());   // FIXED
        sort(leftReqs.begin(), leftReqs.end());     // FIXED

        for (int target : rightReqs)
        {
            Step s;
            s.from = current;
            s.to = target;
            s.distance = abs(target - current);

            cout << "Step " << step_num++
                 << ": " << s.from << " -> " << s.to
                 << " (seek: " << s.distance << ")\n";

            steps.push_back(s);
            current = target;
        }

        if (!leftReqs.empty())
        {
            if (current != disk_max)
            {
                Step toEnd;
                toEnd.from = current;
                toEnd.to = disk_max;
                toEnd.distance = abs(disk_max - current);

                cout << "Step " << step_num++
                     << ": " << toEnd.from << " -> " << toEnd.to
                     << " (seek: " << toEnd.distance << ") [GO END]\n";

                steps.push_back(toEnd);
                current = disk_max;
            }

            Step wrap;
            wrap.from = disk_max;
            wrap.to = 0;
            wrap.distance = disk_max;   // FIXED

            cout << "Step " << step_num++
                 << ": " << wrap.from << " -> " << wrap.to
                 << " (seek: " << wrap.distance << ") [WRAP]\n";

            steps.push_back(wrap);
            current = 0;

            for (int target : leftReqs)
            {
                Step s;
                s.from = current;
                s.to = target;
                s.distance = abs(target - current);

                cout << "Step " << step_num++
                     << ": " << s.from << " -> " << s.to
                     << " (seek: " << s.distance << ")\n";

                steps.push_back(s);
                current = target;
            }
        }
    }
    else
    {
        vector<int> leftReqs, rightReqs;

        for (int r : data.requests)
        {
            if (r <= current) leftReqs.push_back(r);
            else rightReqs.push_back(r);
        }

        sort(leftReqs.begin(), leftReqs.end(), greater<int>());
        sort(rightReqs.begin(), rightReqs.end(), greater<int>());

        for (int target : leftReqs)
        {
            Step s;
            s.from = current;
            s.to = target;
            s.distance = abs(target - current);

            cout << "Step " << step_num++
                 << ": " << s.from << " -> " << s.to
                 << " (seek: " << s.distance << ")\n";

            steps.push_back(s);
            current = target;
        }

        if (!rightReqs.empty())
        {
            if (current != 0)
            {
                Step toStart;
                toStart.from = current;
                toStart.to = 0;
                toStart.distance = abs(current - 0);

                cout << "Step " << step_num++
                     << ": " << toStart.from << " -> 0"
                     << " (seek: " << toStart.distance << ") [GO START]\n";

                steps.push_back(toStart);
                current = 0;
            }

            Step wrap;
            wrap.from = 0;
            wrap.to = disk_max;
            wrap.distance = disk_max;   // FIXED

            cout << "Step " << step_num++
                 << ": 0 -> " << disk_max
                 << " (seek: " << wrap.distance << ") [WRAP]\n";

            steps.push_back(wrap);
            current = disk_max;

            for (int target : rightReqs)
            {
                Step s;
                s.from = current;
                s.to = target;
                s.distance = abs(target - current);

                cout << "Step " << step_num++
                     << ": " << s.from << " -> " << s.to
                     << " (seek: " << s.distance << ")\n";

                steps.push_back(s);
                current = target;
            }
        }
    }

    return steps;
}