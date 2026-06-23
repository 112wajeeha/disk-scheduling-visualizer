#pragma once
#include <string>
#include <vector>
#include "structures.h"

void exportToJson(DiskData data, vector<Step> steps, int total_seek, string algorithm);