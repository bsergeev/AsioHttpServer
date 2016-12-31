#include "connectionmanager.h"

namespace http {
namespace server {

void ConnectionManager::start(std::shared_ptr<Connection> c)
{
    m_connections.insert(c);
    c->start();
}

void ConnectionManager::stop(std::shared_ptr<Connection> c)
{
    m_connections.erase(c);
    c->stop();
}

void ConnectionManager::stop_all()
{
    for (auto c : m_connections) {
        c->stop();
    }
    m_connections.clear();
}

} // namespace server
} // namespace http
