#include "requesthandler.h"

#include "reply.h"
#include "request.h"

#include <fstream>
#include <map>
#include <sstream>
#include <string>

namespace http {
namespace server {

RequestHandler::RequestHandler(const std::string& doc_root)
    : m_doc_root(doc_root)
{
}

void RequestHandler::handle_request(const Request& request, Reply& reply)
{
    // Decode url to path.
    std::string request_path;
    if (!url_decode(request.uri, request_path)) {
        reply = Reply::stock_reply(Reply::Status::bad_request);
        return;
    }

    // Request path must be absolute and not contain "..".
    if (request_path.empty() || request_path[0] != '/'
     || request_path.find("..") != std::string::npos)
    {
        reply = Reply::stock_reply(Reply::Status::bad_request);
        return;
    }

    // If path ends in slash (i.e. is a directory) then add "index.html".
    if (request_path[request_path.size() - 1] == '/') {
        request_path += "index.html";
    }

    // Determine the file extension.
    std::size_t last_slash_pos = request_path.find_last_of("/");
    std::size_t last_dot_pos = request_path.find_last_of(".");
    std::string extension;
    if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos) {
        extension = request_path.substr(last_dot_pos + 1);
    }

    // Open the file to send back.
    std::string full_path = m_doc_root + request_path;
    std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        reply = Reply::stock_reply(Reply::Status::not_found);
        return;
    }

    // Fill out the reply to be sent to the client.
    reply.status = Reply::Status::ok;
    char buf[512];
    while (is.read(buf, sizeof(buf)).gcount() > 0) {
        reply.content.append(buf, is.gcount());
    }
    reply.headers["Content-Length"] = std::to_string(reply.content.size());

    // Convert an extension to a mime type
    auto extension_to_mimeType = [](const std::string& extension) -> std::string {
        using namespace std::string_literals;
        static const std::map<std::string, std::string> mappings = {
              { "gif"s,  "image/gif"s  },
              { "htm"s,  "text/html"s  },
              { "html"s, "text/html"s  },
              { "jpg"s,  "image/jpeg"s },
              { "png"s,  "image/png"s  }
            };
        auto found = mappings.find(extension);
        return (found != mappings.end())? found->second : "text/plain"s;
    };
    reply.headers["Content-Type"] = extension_to_mimeType(extension);
}

//static
bool RequestHandler::url_decode(const std::string& in, std::string& out)
{
    out.clear();
    out.reserve(in.size());
    for (std::size_t i = 0; i < in.size(); ++i)
    {
        if (in[i] == '%')
        {
            if (i + 3 <= in.size())
            {
                int value = 0;
                std::istringstream is(in.substr(i + 1, 2));
                if (is >> std::hex >> value) {
                    out += static_cast<char>(value);
                    i += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (in[i] == '+') {
            out += ' ';
        } else {
            out += in[i];
        }
    }
    return true;
}

} // namespace server
} // namespace http
