#pragma once

#include <asio.hpp>

#include <map>
#include <string>
#include <vector>

namespace http {
namespace server {

// A reply to be sent to a client.
struct Reply
{
  enum class Status 
  {
      ok                    = 200,
      created               = 201,
      accepted              = 202,
      no_content            = 204,
      multiple_choices      = 300,
      moved_permanently     = 301,
      moved_temporarily     = 302,
      not_modified          = 304,
      bad_request           = 400,
      unauthorized          = 401,
      forbidden             = 403,
      not_found             = 404,
      internal_server_error = 500,
      not_implemented       = 501,
      bad_gateway           = 502,
      service_unavailable   = 503
  }; 

  Status                             status;
  std::map<std::string, std::string> headers; // headers to be included in the reply
  std::string                        content; // the content to be sent in the reply

  // Convert the reply into a vector of buffers. The buffers do not own the
  // underlying memory blocks, therefore the reply object must remain valid and
  // not be changed until the write operation has completed.
  std::vector<asio::const_buffer> to_buffers();

  // Get a stock reply.
  static Reply stock_reply(Status status);
};

} // namespace server
} // namespace http
