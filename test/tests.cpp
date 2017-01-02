#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../request.h"
#include "../requestparser.h"
#include <string>
#include <tuple>

SCENARIO("Parsing HTTP request", "[request]"){
    GIVEN("a request"){
        std::string s = "GET / HTTP/1.1\r\n"
            "Host: localhost:8080\r\n"
            "Connection: keep-alive\r\n"
            "Cache-Control: max-age=0\r\n"
            "Upgrade-Insecure-Requests: 1\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
            "Accept-Encoding: gzip, deflate, sdch, br\r\n"
            "Accept-Language: en-US,en;q=0.8,ru;q=0.6\r\n"
            "\r\n";
        WHEN("parsed"){
            http::server::RequestParser parser;
            http::server::Request       request;
            http::server::RequestParser::ParseResult result;
            std::tie(result, std::ignore) = parser.parse(request, s.data(), s.data()+s.length());
            THEN("result should be ok") {
                REQUIRE(result == http::server::RequestParser::ParseResult::good);
            }
            THEN("method should be GET") {
                REQUIRE(request.method == "GET");
            }
            THEN("uri should be /") {
                REQUIRE(request.uri == "/");
            }
            THEN("HTTP version should be 1.1") {
                REQUIRE(request.http_version_major == 1);
                REQUIRE(request.http_version_minor == 1);
            }
            THEN("there should be 8 headers") {
                REQUIRE(request.headers.size() == 8);
            }
        }
    }
}

