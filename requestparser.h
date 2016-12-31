#pragma once

#include <tuple>

namespace http {
namespace server {

struct Request;

// Parser for incoming requests.
class RequestParser
{
public:
    // Reset to initial parser state.
    void reset() { m_state = ParseState::method_start; }


    enum class ParseResult { 
        good,         // complete request has been parsed
        bad,          // the data is invalid
        indeterminate // more data is required
    };

    // The InputIterator return indicates how much of the input has been consumed.
    template <typename InputIterator>
    std::tuple<ParseResult, InputIterator> parse(Request& req, InputIterator begin, InputIterator end)
    {
        while (begin != end) {
            ParseResult result = consume(req, *begin++);
            if (result == ParseResult::good || result == ParseResult::bad) {
                return std::make_tuple(result, begin);
            }
        }
        return std::make_tuple(ParseResult::indeterminate, begin);
    }

private:
    // Process the next character of input.
    ParseResult consume(Request& req, char input);

    static bool is_char   (char c); // is a byte is an HTTP character
    static bool is_ctl    (char c); // is a byte an HTTP control character
    static bool is_special(char c); // is a byte defined as an HTTP special character
    static bool is_digit  (char c);

    enum class ParseState
    {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3
    };
    ParseState m_state = ParseState::method_start;
};

} // namespace server
} // namespace http
