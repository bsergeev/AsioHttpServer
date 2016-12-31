#include "connection.h"

#include "connectionmanager.h"
#include "requesthandler.h"

#include <utility>
#include <vector>
#ifdef LOG
#include <iostream>
#endif

namespace http {
namespace server {

Connection::Connection(asio::ip::tcp::socket&& socket,
                       ConnectionManager&      manager, 
                       RequestHandler&         handler)
  : m_socket(std::move(socket)),
    m_connectionMgr(manager),
    m_requestHandler(handler)
{
}

void Connection::start()
{
    do_read();
}

void Connection::stop()
{
    m_socket.close();
}

void Connection::do_read()
{
    auto self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_buffer),
        [this, self](std::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec)
            {
                RequestParser::ParseResult result;
                std::tie(result, std::ignore) = m_requestParser.parse(
                    m_request, m_buffer.data(), m_buffer.data() + bytes_transferred);

                if (result == RequestParser::ParseResult::good)
                {
                    #ifdef LOG
                    std::cout << "Processed request:\n\tmethod: " << m_request.method
                        << "\n\turi: " << m_request.uri
                        << "\n\tHTTP version: " << m_request.http_version_major 
                        << "." << m_request.http_version_minor
                        << "\n\theaders:";
                    for (auto h : m_request.headers) {
                        std::cout << "\n\t\t" << h.name << "\t: " << h.value;
                    }
                    std::cout << std::endl;
                    #endif

                    m_requestHandler.handle_request(m_request, m_reply);
                    do_write();
                } else if (result == RequestParser::ParseResult::bad) {
                    m_reply = Reply::stock_reply(Reply::Status::bad_request);
                    do_write();
                } else {
                    do_read();
                }
            } else if (ec != asio::error::operation_aborted) {
                m_connectionMgr.stop(shared_from_this());
            }
        });
}

void Connection::do_write()
{
    auto self(shared_from_this());
    asio::async_write(m_socket, m_reply.to_buffers(),
        [this, self](std::error_code ec, std::size_t)
        {
            if (!ec) // Initiate graceful connection closure
            {
                asio::error_code ignored_ec;
                m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ignored_ec);
            }
            if (ec != asio::error::operation_aborted)
            {
                m_connectionMgr.stop(shared_from_this());
            }
        });
}

} // namespace server
} // namespace http
