#include "server.h"
#include <signal.h>
#include <utility>

namespace http {
namespace server {

Server::Server(const std::string& address, const std::string& port, const std::string& doc_root)
    : m_io_context(1),
      m_signals (m_io_context),
      m_acceptor(m_io_context),
      m_connectionMgr(),
      m_requestHandler(doc_root)
{
    // Register to handle the signals that indicate when the server should exit.
    // It is safe to register for the same signal multiple times in a program,
    // provided all registration for the specified signal is made through ASIO.
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
#if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
#endif
    do_await_stop();

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(m_io_context);
    asio::ip::tcp::endpoint endpoint = *resolver.resolve(address, port).begin();
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();

    do_accept();
}

void Server::run()
{
    // The io_context::run() call will block until all asynchronous operations
    // have finished. While the server is running, there is always at least one
    // asynchronous operation outstanding: the asynchronous accept call waiting
    // for new incoming connections.
    m_io_context.run();
}

void Server::do_accept()
{
    m_acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket)
        {
            // Check whether the server was stopped by a signal before this
            // completion handler had a chance to run.
            if (!m_acceptor.is_open()) {
                return;
            }

            if (!ec) {
                m_connectionMgr.start(std::make_shared<Connection>(std::move(socket),
                                                                   m_connectionMgr,
                                                                   m_requestHandler));
            }

            do_accept();
        });
}

void Server::do_await_stop()
{
    m_signals.async_wait(
        [this](std::error_code, int)
    {
        // The server is stopped by canceling all outstanding asynchronous operations.
        // Once all operations have finished, the io_context::run() will exit.
        m_acceptor.close();
        m_connectionMgr.stop_all();
    });
}

} // namespace server
} // namespace http
