#include "utils.h"
#include <cmath>

double roundTo2Decimals(double value) {
    return std::round(value * 100.0) / 100.0;
}