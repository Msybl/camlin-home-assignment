#include <iostream>
#include "third_party/json.hpp"

using json = nlohmann::json;

int main() {
    json obj;
    obj["currency"] = "USD";
    obj["amount"] = 100;
    
    std::cout << obj.dump(2) << std::endl;
    
    return 0;
}
