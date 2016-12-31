#pragma once

#include <string>

namespace http {
namespace server {

struct Reply;
struct Request;

// The common handler for all incoming requests.
class RequestHandler
{
public:
   RequestHandler           (const RequestHandler&) = delete;
   RequestHandler& operator=(const RequestHandler&) = delete;

    // Construct with a directory containing files to be served.
    explicit RequestHandler(const std::string& doc_root);

    // Handle a request and produce a reply.
    void handle_request(const Request& req, Reply& rep);

private:
    std::string m_doc_root; // the directory containing the files to be served

    // Perform URL-decoding on a string. Returns false if the encoding was invalid.
    static bool url_decode(const std::string& in, std::string& out);
};

} // namespace server
} // namespace http
