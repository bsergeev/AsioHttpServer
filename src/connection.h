#pragma once

#include "constants.h"

#include <asio.hpp>
#include <map>
#include <string>
#include <utility>

class Connection 
{
public:
    Connection(std::shared_ptr<asio::ip::tcp::socket> sock) :
        m_sock(sock),
        m_requestStream(8192)
    {}

    void ProcessRequest();

    static std::tuple<std::string, std::string, std::string> ParseRequestLine   (std::istream& request_stream);
    static std::map<std::string, std::string>                ParseRequestHeaders(std::istream& request_stream);

private:
    std::pair<http::StatusCode, size_t>
    processRequest(const std::string& requested_resource, 
                   const std::map<std::string, std::string>& request_headers,
                   const std::string& request_body);

    void sendResponse(http::StatusCode response_status_code, size_t resource_size_bytes);

//data:
    std::shared_ptr<asio::ip::tcp::socket> m_sock;
    asio::streambuf  m_requestStream;

    // The following are class variables (rather than local variables in sendResponse) to have
    // sufficient lifespan for use in "asio::async_write(*m_sock.get(), response_buffers..."
    std::string             m_responseStatusLine; 
    std::string             m_responseHeaders;
    std::unique_ptr<char[]> m_resourceBuffer;
};
