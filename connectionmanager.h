#pragma once

#include "connection.h"

#include <memory>
#include <set>

namespace http {
namespace server {

// Manages open connections so that they may be cleanly stopped on the server shut down.
class ConnectionManager
{
public:
    ConnectionManager           (const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    ConnectionManager() = default;

    // Add the specified connection to the manager and start it.
    void start(std::shared_ptr<Connection> c);

    // Stop the specified connection.
    void stop(std::shared_ptr<Connection> c);

    // Stop all connections.
    void stop_all();

private:
    std::set<std::shared_ptr<Connection>> m_connections; // managed connections
};

} // namespace server
} // namespace http
