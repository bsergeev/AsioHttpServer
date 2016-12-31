#pragma once

#include "reply.h"
#include "request.h"
#include "requestparser.h"

#include <asio.hpp>

#include <array>
#include <memory>

namespace http {
namespace server {

class ConnectionManager;
class RequestHandler;

// Represents a single connection from a client.
class Connection : public std::enable_shared_from_this<Connection>
{
public:
  Connection           (const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  // Construct a connection with the given socket.
  explicit Connection(asio::ip::tcp::socket&& socket,
                      ConnectionManager&      manager, 
                      RequestHandler&         handler);

  // Start the first asynchronous operation for the connection.
  void start();

  // Stop all asynchronous operations associated with the connection.
  void stop();

private:
  // Perform an asynchronous read operation.
  void do_read();

  // Perform an asynchronous write operation.
  void do_write();

//data:
  asio::ip::tcp::socket m_socket;

  ConnectionManager& m_connectionMgr;
  RequestHandler&    m_requestHandler;
  RequestParser      m_requestParser;

  std::array<char, 8192> m_buffer; // buffer for the incoming data, 8 kB
  Request       m_request;         // the incoming request
  Reply         m_reply;           // reply to be sent back to the client
};

} // namespace server
} // namespace http
