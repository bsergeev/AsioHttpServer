#include "constants.h"

#include <map>
#include <string>
#include <utility>

namespace purpose {

    std::pair<http::StatusCode, std::string>
    Evaluate(const std::string& requested_resource,
             const std::map<std::string, std::string>& request_headers,
             const std::string& request_body);
} // namespace purpose