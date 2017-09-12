#include "connection.h"
#include "evaluator.h"
#include "log.h"

#include <asio.hpp>
#include <gsl/gsl_util>

#include <algorithm>
#include <iostream>
#include <map>
#include <tuple>

using namespace std::string_literals;

void Connection::ProcessRequest() 
{
  asio::async_read_until(*m_sock.get(), m_requestStream, "\r\n",
    [this](const std::error_code& e, std::size_t bytes_transferred) 
    {
      // Use GSL `finally` to call sendResponse(status_code, resource_size)
      // at the very end. This must always be done, as sendResponse deletes
      // this Connection object.
      http::StatusCode status_code = http::StatusCode::ServerError;
      size_t         resource_size = 0; // processRequest() returns the value
      auto sendResponseCaller = gsl::finally([this, &status_code, &resource_size](){ 
                                               sendResponse(status_code, resource_size); 
                                             });
      if (e) {
        if (e == asio::error::eof) {
          LOG("Read all" << std::endl);
        } else {
          LOG("Error occurred[1]: "<< e.value() <<", \""<< e.message() <<"\" ("
              << bytes_transferred << " bytes transferred)" << std::endl);
        }
        status_code = (e == asio::error::not_found)? http::StatusCode::RequestTooLarge // no delimiter in the request
                                                   : http::StatusCode::BadRequest;     // on any other error
        return; // sendResponse(status_code, resource_size) gets called
      }

      std::istream req_stream(&m_requestStream);

      std::string method, resource, version;
      std::tie   (method, resource, version) = Connection::ParseRequestLine(req_stream);

      // Verify that everything is supported
      if (method.compare("GET"s) != 0 && method.compare("PUT"s) != 0) { // Unsupported method
        status_code = http::StatusCode::NotImplemented;
      } else if (version.compare("HTTP/1.1"s) != 0) { // non-supported HTTP version or bad request
        status_code = http::StatusCode::HttpVerNotSupported;
      }
      else // request looks ok => read the request headers
      {
        // Note that sendResponseCaller gets moved into a new variable, _, so that
        // this Connection destruction doesn't happen on asio::async_read_until() call,
        // which returns immediately, but at the end of its callback execution. 
        asio::async_read_until(*m_sock.get(), m_requestStream, "\r\n\r\n",
          [this, resource, &status_code, &resource_size, _=std::move(sendResponseCaller)]
          (const std::error_code& e, std::size_t /*bytes_transferred*/) mutable {
            if (e) {
              LOG("Error occurred[2]: "<< e.value() <<", \""<< e.message() <<"\"\n");
              status_code = (e == asio::error::not_found)? http::StatusCode::RequestTooLarge // no delimiter in the request
                                                         : http::StatusCode::BadRequest;     // on any other error
              return; // sendResponse(status_code, resource_size) gets called
            }

            std::istream req_stream(&m_requestStream);
            auto request_headers = ParseRequestHeaders(req_stream);

            // Read request body, if present
            int body_size = 0;
            if (request_headers.count("content-length") == 1 
             && (body_size = std::stoi(request_headers.at("content-length"))) > 0) 
            {
              LOG("Request body content-length: " << body_size << std::endl);
              auto buf = std::make_shared<std::vector<char>>(body_size);
              asio::async_read(*m_sock.get(), asio::buffer(*buf, body_size),
                [this, resource, buf, body_size, request_headers, &status_code, &resource_size, _=std::move(_)]
                (const asio::error_code& e, std::size_t s) mutable {
                  if (e || s < body_size) {
                    LOG("Error occurred[3]: "<< e.value() <<", \""<< e.message() << std::endl);
                    status_code = http::StatusCode::BadRequest;
                  } else {
                    const std::string request_body{ buf->data(), buf->data()+body_size };
                    std::tie(status_code, resource_size) = processRequest(resource, request_headers, request_body);
                  }
                });
            } else { // no request body
              std::tie(status_code, resource_size) = processRequest(resource, request_headers, "");
            }
          });
      }
      // sendResponse(status_code, resource_size) gets called
    });
}

//static 
std::tuple<std::string, std::string, std::string> Connection::ParseRequestLine(std::istream& request_stream)
{
    std::string  line;
    std::getline(request_stream, line, '\r');
    request_stream.get(); // skip '\n'

    std::string method, resource, version;
    std::istringstream(line) >> method >> resource >> version;
    return std::tuple<std::string, std::string, std::string>(std::move(method), std::move(resource), std::move(version));
}

// ParseRequestHeaders converts all the names of the request headers to lower case.
//static
std::map<std::string, std::string> Connection::ParseRequestHeaders(std::istream& request_stream)
{
    std::map<std::string, std::string> request_headers;
    while (!request_stream.eof()) {
        std::string  header_name;
        std::getline(request_stream, header_name, ':');
        std::transform(header_name.begin(), header_name.end(), header_name.begin(), ::tolower);
        if (!request_stream.eof()) {
            std::string  header_value;
            std::getline(request_stream, header_value, '\r');
            request_stream.get(); // skip '\n'
            request_headers[header_name] = header_value;
        }
    }
    return std::move(request_headers);
}


// If you need to serve files, add the following code to processRequest():
//
//#include <fstream>
// std::string doc_root = ".."s; // some root, perhaps from settings
// std::string full_path = doc_root + requested_resource;
// std::ifstream is(full_path.c_str(), std::ios::in | std::ios::binary);
// if (is) {
//     reply.clear();
//     char buf[512];
//     while (is.read(buf, sizeof(buf)).gcount() > 0) {
//        reply.append(buf, is.gcount());
//     }
//     http_code = http::StatusCode::OK;
// } else {
//     http_code = http::StatusCode::NotFound;
// }
std::pair<http::StatusCode, size_t>
Connection::processRequest(const std::string& requested_resource,
                           const std::map<std::string, std::string>& request_headers,
                           const std::string& request_body)
{
    LOG("Processing request for \"" << requested_resource << "\"\n");
        
    http::StatusCode http_code = http::StatusCode::ServerError;
    size_t resource_size_bytes = 0;
    std::string reply;
    if (requested_resource.compare("/favicon.ico") == 0) {
        reply = "AAABAAEAEBAAAAEAGABoAwAAFgAAACgAAAAQAAAAIAAAAAEAGAAAAAAAAAAAABMLAAATCwAAAAAAAAAAAAD"
            "///////////////////////////////////////////////////////////////////8AAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////8A"
            "AAAAAAAcHBz///////////8AAAAAAAD///////////8cHBwAAAAAAAD///////8AAAAAAAAAAABXV1f///9LS0s"
            "AAAAAAABLS0v///+NjY0AAAAAAAAAAAD///////8AAAAAAAAAAAAAAAD///////////////////////8AAAAAAA"
            "AAAAAAAAD///////8AAAAAAAAAAAAAAACjo6P///8AAAAAAAD////U1NQAAAAAAAAAAAAAAAD///////8AAAAAA"
            "AAAAAAAAAAqKir///+FhYWFhYX///9BQUEAAAAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAADY2Nj4+Pj5"
            "+fnY2NgAAAAAAAAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAAB3d3f///////+NjY0AAAAAAAAAAAAAAAA"
            "AAAD///////8AAAAAAAAAAAAAAAAAAAAAAAC5ubm5ubkAAAAAAAAAAAAAAAAAAAAAAAD///////8AAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAAAAD///////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/"
            "//////////////////////////////////////////////////////////////////8AAAjNAAAc6wAAuL8AANp"
            "cAABgPgAAJdwAAG2AAACDVwAAoMQAAA/6AACPygAA+bkAACJWAAADywAAF7EAAAUp";
        http_code = http::StatusCode::OK;
        m_responseHeaders += "Content-Type: "s + "image/x-icon"s + "\r\n"s;
        m_responseHeaders += "Content-Encoding: "s + "base64"s + "\r\n"s;
    } else { // do something useful
        std::tie(http_code, reply) = purpose::Evaluate(requested_resource, request_headers, request_body);
    }

    if (http_code == http::StatusCode::OK) {
        m_resourceBuffer.reset(new char[resource_size_bytes = reply.length()]);
        memcpy(m_resourceBuffer.get(), reply.data(), resource_size_bytes);
        m_responseHeaders += "content-length: "s + std::to_string(resource_size_bytes) + "\r\n";
    }
    return std::pair<http::StatusCode, size_t>(http_code, resource_size_bytes);
}


// IMPORTANT: sendResponse must always be the last Connection method called and
//            it must be called once, as it destroys this Connection object!
void Connection::sendResponse(http::StatusCode response_status_code, size_t resource_size_bytes)
{
    auto deleteThis = gsl::finally([this](){ delete this; });

    m_sock->shutdown(asio::ip::tcp::socket::shutdown_receive);

    static const std::map<http::StatusCode, const std::string> http_status_table = {
        { http::StatusCode::OK                 , "200 OK"s },
        { http::StatusCode::BadRequest         , "400 Bad Request"s },
        { http::StatusCode::NotFound           , "404 Not Found"s },
        { http::StatusCode::RequestTooLarge    , "413 Request Entity Too Large"s },
        { http::StatusCode::ServerError        , "500 Server Error"s },
        { http::StatusCode::NotImplemented     , "501 Not Implemented"s },
        { http::StatusCode::HttpVerNotSupported, "505 HTTP Version Not Supported"s }
    };
    if (http_status_table.count(response_status_code) == 0) {
        response_status_code = http::StatusCode::ServerError; // we know it's in http_status_table
    }
    m_responseStatusLine = "HTTP/1.1 "s+ http_status_table.at(response_status_code) +"\r\n";
    m_responseHeaders += "\r\n";

    std::vector<asio::const_buffer> response_buffers;
    response_buffers.push_back(asio::buffer(m_responseStatusLine));
    response_buffers.push_back(asio::buffer(m_responseHeaders));
    if (resource_size_bytes > 0) {
        response_buffers.push_back(asio::buffer(m_resourceBuffer.get(), resource_size_bytes));
    }

    asio::async_write(*m_sock.get(), response_buffers,
        [this, _=std::move(deleteThis)](const std::error_code& e, std::size_t bytes_transferred) mutable {
            if (e) {
                LOG("Error occurred[4]: "<< e.value() <<", \""<< e.message() <<"\"\n");
            } else {
                LOG("Successfully returned "<< bytes_transferred <<" bytes"<< std::endl);
            }
            m_sock->shutdown(asio::ip::tcp::socket::shutdown_both);
            // `delete this` happens on exit
        });
}
