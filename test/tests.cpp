#include "../src/connection.h"
#include "../src/constants.h"
#include "../src/evaluator.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <map>
#include <sstream>

using std::string;

#ifdef DO_LOG
std::mutex gMutex;
#endif

SCENARIO("Parsing HTTP request", "[request]"){
    GIVEN("a request"){
        const string s = "GET / HTTP/1.1\r\n"
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
            std::stringstream req_stream(s);
            string method, resource, http_version;
            std::tie   (method, resource, http_version) = Connection::ParseRequestLine(req_stream);
            THEN("Method should be GET") {
                REQUIRE(method.compare("GET") == 0);
            }
            THEN("Requested resource should be /") {
                REQUIRE(resource.compare("/") == 0);
            }
            THEN("HTTP version should be 1.1") {
                REQUIRE(http_version.compare("HTTP/1.1") == 0);
            }
            const std::map<string, string> headers = Connection::ParseRequestHeaders(req_stream);
            THEN("There should be 8 request headers") {
                REQUIRE(headers.size() == 8);
                REQUIRE(headers.count("host") == 1);
                REQUIRE(headers.count("connection") == 1);
                REQUIRE(headers.count("cache-control") == 1);
                REQUIRE(headers.count("upgrade-insecure-requests") == 1);
                REQUIRE(headers.count("user-agent") == 1);
                REQUIRE(headers.count("accept") == 1);
                REQUIRE(headers.count("accept-encoding") == 1);
                REQUIRE(headers.count("accept-language") == 1);
            }
        }
    }
}

SCENARIO("Generating HTTP response", "[response]")
{
    GIVEN("a request") {
        const string                   requested_resource;
        const std::map<string, string> request_headers;
        const string                   request_body;
        WHEN("generate a response") {
            http::StatusCode http_code = http::StatusCode::ServerError;
            string           reply;
            std::tie(http_code, reply) = purpose::Evaluate(requested_resource, request_headers, request_body);
            THEN("HTTP code should be 200") {
                REQUIRE(http_code == http::StatusCode::OK);
            }
            THEN("reply should be non-empty") {
                REQUIRE(reply.length() > 0);
            }
        }
    }
}
