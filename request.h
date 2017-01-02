#pragma once

#include <string>
#include <vector>

namespace http {
namespace server {

// A request received from a client
struct Request
{
    std::string method;
    std::string uri;
    int         http_version_major = -1;
    int         http_version_minor = -1;

    struct Header {
        // Headers are parsed char-by-char in RequestParser::consume(), thus this only constructor
        explicit Header(char n) : name(1, n) {}
        Header() = delete;
    //data:
        std::string name;
        std::string value;
    };
    std::vector<Header> headers;
};

} // namespace server
} // namespace http
