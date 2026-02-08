#ifndef AUTH_H
#define AUTH_H

#include <string>
#include "../third_party/httplib.h"

// Authenticate request and return user_id
// Returns empty string if authentication fails
std::string authenticateRequest(const httplib::Request& req, httplib::Response& res);

#endif