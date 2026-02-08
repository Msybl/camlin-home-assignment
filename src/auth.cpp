#include "auth.h"
#include <map>
#include "../third_party/json.hpp"

using json = nlohmann::json;

const std::map<std::string, std::string> VALID_API_KEYS = {
    {"key-123", "user1"},
    {"key-456", "user2"},
    {"key-789", "user3"}
};

std::string authenticateRequest(const httplib::Request& req, httplib::Response& res) {
    // Check if X-API-Key header exists
    if (!req.has_header("X-API-Key")) {
        res.status = 401;
        json error_response;
        error_response["error"] = "Missing API key";
        error_response["message"] = "Please provide X-API-Key header";
        res.set_content(error_response.dump(2), "application/json");
        return "";
    }
    
    std::string api_key = req.get_header_value("X-API-Key");
    
    // Check API key
    auto iterator = VALID_API_KEYS.find(api_key);
    if (iterator == VALID_API_KEYS.end()) {
        res.status = 401;
        json error_response;
        error_response["error"] = "Invalid API key";
        error_response["message"] = "Valid keys: key-123, key-456, key-789";
        res.set_content(error_response.dump(2), "application/json");
        return "";
    }
    
    // Return user_id corresponding to the API key
    return iterator->second;
}