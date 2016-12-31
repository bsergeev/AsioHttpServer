#include "reply.h"

namespace http {
namespace server {

namespace status_strings {

    const std::string ok           = "HTTP/1.0 200 OK\r\n";
    const std::string created      = "HTTP/1.0 201 Created\r\n";
    const std::string accepted     = "HTTP/1.0 202 Accepted\r\n";
    const std::string no_content   = "HTTP/1.0 204 No Content\r\n";
    const std::string multiple_choices  = "HTTP/1.0 300 Multiple Choices\r\n";
    const std::string moved_permanently = "HTTP/1.0 301 Moved Permanently\r\n";
    const std::string moved_temporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
    const std::string not_modified = "HTTP/1.0 304 Not Modified\r\n";
    const std::string bad_request  = "HTTP/1.0 400 Bad Request\r\n";
    const std::string unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
    const std::string forbidden    = "HTTP/1.0 403 Forbidden\r\n";
    const std::string not_found    = "HTTP/1.0 404 Not Found\r\n";
    const std::string internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
    const std::string not_implemented = "HTTP/1.0 501 Not Implemented\r\n";
    const std::string bad_gateway  = "HTTP/1.0 502 Bad Gateway\r\n";
    const std::string service_unavailable = "HTTP/1.0 503 Service Unavailable\r\n";

    asio::const_buffer to_buffer(Reply::Status status)
    {
        switch (status)
        {
        case Reply::Status::ok:
            return asio::buffer(ok);
        case Reply::Status::created:
            return asio::buffer(created);
        case Reply::Status::accepted:
            return asio::buffer(accepted);
        case Reply::Status::no_content:
            return asio::buffer(no_content);
        case Reply::Status::multiple_choices:
            return asio::buffer(multiple_choices);
        case Reply::Status::moved_permanently:
            return asio::buffer(moved_permanently);
        case Reply::Status::moved_temporarily:
            return asio::buffer(moved_temporarily);
        case Reply::Status::not_modified:
            return asio::buffer(not_modified);
        case Reply::Status::bad_request:
            return asio::buffer(bad_request);
        case Reply::Status::unauthorized:
            return asio::buffer(unauthorized);
        case Reply::Status::forbidden:
            return asio::buffer(forbidden);
        case Reply::Status::not_found:
            return asio::buffer(not_found);
        case Reply::Status::internal_server_error:
            return asio::buffer(internal_server_error);
        case Reply::Status::not_implemented:
            return asio::buffer(not_implemented);
        case Reply::Status::bad_gateway:
            return asio::buffer(bad_gateway);
        case Reply::Status::service_unavailable:
            return asio::buffer(service_unavailable);
        default:
            return asio::buffer(internal_server_error);
        }
    }
} // namespace status_strings

namespace misc_strings {
    const char name_value_separator[] = { ':', ' ' };
    const char crlf[] = { '\r', '\n' };
} // namespace misc_strings

std::vector<asio::const_buffer> Reply::to_buffers()
{
  std::vector<asio::const_buffer> buffers;
  buffers.push_back(status_strings::to_buffer(status));
  for (const auto& h : headers)
  {
    buffers.push_back(asio::buffer(h.first));
    buffers.push_back(asio::buffer(misc_strings::name_value_separator));
    buffers.push_back(asio::buffer(h.second));
    buffers.push_back(asio::buffer(misc_strings::crlf));
  }
  buffers.push_back(asio::buffer(misc_strings::crlf));
  buffers.push_back(asio::buffer(content));
  return buffers;
}

namespace stock_replies {

const char ok[] = "";
const char created[] =
  "<html>"
  "<head><title>Created</title></head>"
  "<body><h1>201 Created</h1></body>"
  "</html>";
const char accepted[] =
  "<html>"
  "<head><title>Accepted</title></head>"
  "<body><h1>202 Accepted</h1></body>"
  "</html>";
const char no_content[] =
  "<html>"
  "<head><title>No Content</title></head>"
  "<body><h1>204 Content</h1></body>"
  "</html>";
const char multiple_choices[] =
  "<html>"
  "<head><title>Multiple Choices</title></head>"
  "<body><h1>300 Multiple Choices</h1></body>"
  "</html>";
const char moved_permanently[] =
  "<html>"
  "<head><title>Moved Permanently</title></head>"
  "<body><h1>301 Moved Permanently</h1></body>"
  "</html>";
const char moved_temporarily[] =
  "<html>"
  "<head><title>Moved Temporarily</title></head>"
  "<body><h1>302 Moved Temporarily</h1></body>"
  "</html>";
const char not_modified[] =
  "<html>"
  "<head><title>Not Modified</title></head>"
  "<body><h1>304 Not Modified</h1></body>"
  "</html>";
const char bad_request[] =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const char unauthorized[] =
  "<html>"
  "<head><title>Unauthorized</title></head>"
  "<body><h1>401 Unauthorized</h1></body>"
  "</html>";
const char forbidden[] =
  "<html>"
  "<head><title>Forbidden</title></head>"
  "<body><h1>403 Forbidden</h1></body>"
  "</html>";
const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";
const char internal_server_error[] =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";
const char not_implemented[] =
  "<html>"
  "<head><title>Not Implemented</title></head>"
  "<body><h1>501 Not Implemented</h1></body>"
  "</html>";
const char bad_gateway[] =
  "<html>"
  "<head><title>Bad Gateway</title></head>"
  "<body><h1>502 Bad Gateway</h1></body>"
  "</html>";
const char service_unavailable[] =
  "<html>"
  "<head><title>Service Unavailable</title></head>"
  "<body><h1>503 Service Unavailable</h1></body>"
  "</html>";

std::string to_string(Reply::Status status)
{
  switch (status)
  {
  case Reply::Status::ok:
    return ok;
  case Reply::Status::created:
    return created;
  case Reply::Status::accepted:
    return accepted;
  case Reply::Status::no_content:
    return no_content;
  case Reply::Status::multiple_choices:
    return multiple_choices;
  case Reply::Status::moved_permanently:
    return moved_permanently;
  case Reply::Status::moved_temporarily:
    return moved_temporarily;
  case Reply::Status::not_modified:
    return not_modified;
  case Reply::Status::bad_request:
    return bad_request;
  case Reply::Status::unauthorized:
    return unauthorized;
  case Reply::Status::forbidden:
    return forbidden;
  case Reply::Status::not_found:
    return not_found;
  case Reply::Status::internal_server_error:
    return internal_server_error;
  case Reply::Status::not_implemented:
    return not_implemented;
  case Reply::Status::bad_gateway:
    return bad_gateway;
  case Reply::Status::service_unavailable:
    return service_unavailable;
  default:
    return internal_server_error;
  }
}
} // namespace stock_replies

//static
Reply Reply::stock_reply(Reply::Status status)
{
  Reply reply;
  reply.status = status;
  reply.content = stock_replies::to_string(status);
  reply.headers["Content-Length"] = std::to_string(reply.content.size());
  reply.headers["Content-Type"  ] = "text/html";
  return reply;
}

} // namespace server
} // namespace http
