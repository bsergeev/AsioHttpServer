#pragma once

#include "connection.h"
#include "connectionmanager.h"
#include "requesthandler.h"

#include <asio.hpp>

#include <string>

namespace http {
namespace server {

class Server
{
public:
    Server           (const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // Listen on the specified TCP address/port and serve files from the given directory
    Server(const std::string& address, const std::string& port, const std::string& doc_root);

    // Run the server's io_context loop.
    void run();

private:
    // Perform an asynchronous accept operation.
    void do_accept();

    // Wait for a request to stop the server.
    void do_await_stop();

    //data:
    asio::io_context        m_io_context; // io_context to perform asynchronous operations
    asio::signal_set        m_signals;    // signal_set to register for process termination notifications
    asio::ip::tcp::acceptor m_acceptor;   // acceptor to listen for incoming connections

    ConnectionManager m_connectionMgr;    // connection manager which owns all live connections
    RequestHandler    m_requestHandler;   // handler for all incoming requests
};

} // namespace server
} // namespace http
