#include <iostream>
#include "../third_party/httplib.h"

int main() {
    std::cout << "Currency Wallet API" << std::endl;
    
    httplib::Server srv;
    
    srv.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello World!", "text/plain");
    });
    
    std::cout << "Server listening on port 8080" << std::endl;
    srv.listen("0.0.0.0", 8080);
    
    return 0;
}
