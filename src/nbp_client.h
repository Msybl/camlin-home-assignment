#ifndef NBP_CLIENT_H
#define NBP_CLIENT_H

#include <string>
#include "../third_party/json.hpp"

using json = nlohmann::json;

#define CACHE_DURATION_SEC  3600

// Fetch exchange rate from NBP API
double fetchNBPRate(const std::string& currency);

// Fetch Table C which contains "Ask" rates
std::map<std::string, double> fetchAllNBPRates();

// Cache class for NBP rates
class NBPRateCache {
public:
    std::map<std::string, double> rates;
    time_t last_updated;
    int cache_duration_sec;

    NBPRateCache() : last_updated(0), cache_duration_sec(CACHE_DURATION_SEC) {}

    bool isExpired() const;
};

#endif // NBP_CLIENT_H