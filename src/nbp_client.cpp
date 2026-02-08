#include "nbp_client.h"
#include <curl/curl.h>
#include <iostream>
#include "utils.h"

// Callback function for libcurl to write data
static size_t WriteCallback(char* data, size_t size, size_t nmemb, std::string* response_data)
{
    if (response_data == nullptr) {
        return 0;
    }
    
    response_data->append(data, size * nmemb);
    return size * nmemb;
}

double fetchNBPRate(const std::string& currency) {
    std::string url = "https://api.nbp.pl/api/exchangerates/rates/a/" + currency + "/?format=json";
    std::string response_data;
    char errorBuffer[CURL_ERROR_SIZE];
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return -1.0;
    }
    
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        
    CURLcode result;
    result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if(result != CURLE_OK) {
        std::cerr << "Failed to set URL: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write function: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write data: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    // Perform the request
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "CURL request failed: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return -1.0;
    }

    curl_easy_cleanup(curl);

    // Parse JSON response
    try {
        json nbp_response = json::parse(response_data);
        double rate = nbp_response["rates"][0]["mid"];
        
        std::cout << "Fetched rate for " << currency << ": " << roundTo2Decimals(rate) << " PLN" << std::endl;
        return rate;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return -1.0;
    }
}

std::map<std::string, double> fetchAllNBPRates() {
    std::map<std::string, double> rates;
    
    // Use NBP Table C endpoint for "Ask" prices
    std::string url = "https://api.nbp.pl/api/exchangerates/tables/c/?format=json";
    std::string response_data;
    char errorBuffer[CURL_ERROR_SIZE];
    
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return rates;
    }
    
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    
    CURLcode result;
    result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    if (result != CURLE_OK) {
        std::cerr << "Failed to set URL: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return rates;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write function: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return rates;
    }

    result = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    if (result != CURLE_OK) {
        std::cerr << "Failed to set write data: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return rates;
    }

    // Perform the request
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        std::cerr << "CURL request failed: " << errorBuffer << std::endl;
        curl_easy_cleanup(curl);
        return rates;
    }
    
    curl_easy_cleanup(curl);

    try {
        json nbp_response = json::parse(response_data);
        
        // Table C returns an array: rates[0]["rates"]
        for (auto& rate : nbp_response[0]["rates"]) {
            std::string code = rate["code"];
            double ask_price = rate["ask"];
            rates[code] = ask_price;
        }
        
        std::cout << "Fetched " << rates.size() << " Ask rates from NBP Table C" << std::endl;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }
    
    return rates;
}