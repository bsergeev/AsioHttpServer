#include "evaluator.h"
#include "log.h"

namespace purpose {

using namespace std::string_literals;

// Generate some data to return to the caller.
std::pair<http::StatusCode, std::string>
Evaluate(const std::string& requested_resource,
         const std::map<std::string, std::string>& request_headers,
         const std::string& request_body)
{
    LOG("Requested: \""<< requested_resource <<"\"\n");
    LOG("Request headers:\n");
    for (auto& it : request_headers) {
        LOG("\t" << it.first << ": " << it.second << std::endl);
    }
    if (!request_body.empty()) {
        LOG("Request body size: "<< request_body.size() << std::endl);
    }

    // Some silly code to populate `reply` string
    static size_t calls = 0;
    std::string reply = "Called "s + std::to_string(++calls) + " times"s;

    return std::pair<http::StatusCode, std::string>(http::StatusCode::OK, std::move(reply));
}

} // namespace purpose