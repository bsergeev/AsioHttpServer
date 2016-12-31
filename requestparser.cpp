#include "requestparser.h"
#include "request.h"

namespace http {
namespace server {

RequestParser::ParseResult RequestParser::consume(Request& req, char input)
{
    switch (m_state)
    {
    case ParseState::method_start:
        if (!is_char(input) || is_ctl(input) || is_special(input)) {
            return ParseResult::bad;
        } else {
            m_state = ParseState::method;
            req.method.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::method:
        if (input == ' ') {
            m_state = ParseState::uri;
            return ParseResult::indeterminate;
        } else if (!is_char(input) || is_ctl(input) || is_special(input)) {
            return ParseResult::bad;
        } else {
            req.method.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::uri:
        if (input == ' ') {
            m_state = ParseState::http_version_h;
            return ParseResult::indeterminate;
        } else if (is_ctl(input)) {
            return ParseResult::bad;
        } else {
            req.uri.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::http_version_h:
        if (input == 'H') {
            m_state = ParseState::http_version_t_1;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_t_1:
        if (input == 'T') {
            m_state = ParseState::http_version_t_2;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_t_2:
        if (input == 'T') {
            m_state = ParseState::http_version_p;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_p:
        if (input == 'P') {
            m_state = ParseState::http_version_slash;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_slash:
        if (input == '/') {
            req.http_version_major = 0;
            req.http_version_minor = 0;
            m_state = ParseState::http_version_major_start;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_major_start:
        if (is_digit(input)) {
            req.http_version_major = req.http_version_major * 10 + input - '0';
            m_state = ParseState::http_version_major;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_major:
        if (input == '.') {
            m_state = ParseState::http_version_minor_start;
            return ParseResult::indeterminate;
        } else if (is_digit(input)) {
            req.http_version_major = req.http_version_major * 10 + input - '0';
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_minor_start:
        if (is_digit(input)) {
            req.http_version_minor = req.http_version_minor * 10 + input - '0';
            m_state = ParseState::http_version_minor;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::http_version_minor:
        if (input == '\r') {
            m_state = ParseState::expecting_newline_1;
            return ParseResult::indeterminate;
        } else if (is_digit(input)) {
            req.http_version_minor = req.http_version_minor * 10 + input - '0';
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::expecting_newline_1:
        if (input == '\n') {
            m_state = ParseState::header_line_start;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::header_line_start:
        if (input == '\r') {
            m_state = ParseState::expecting_newline_3;
            return ParseResult::indeterminate;
        } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
            m_state = ParseState::header_lws;
            return ParseResult::indeterminate;
        } else if (!is_char(input) || is_ctl(input) || is_special(input)) {
            return ParseResult::bad;
        } else {
            req.headers.emplace_back(input); // initialize name with its 1st char
            m_state = ParseState::header_name;
            return ParseResult::indeterminate;
        }

    case ParseState::header_lws:
        if (input == '\r') {
            m_state = ParseState::expecting_newline_2;
            return ParseResult::indeterminate;
        } else if (input == ' ' || input == '\t') {
            return ParseResult::indeterminate;
        } else if (is_ctl(input)) {
            return ParseResult::bad;
        } else {
            m_state = ParseState::header_value;
            req.headers.back().value.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::header_name:
        if (input == ':') {
            m_state = ParseState::space_before_header_value;
            return ParseResult::indeterminate;
        } else if (!is_char(input) || is_ctl(input) || is_special(input)) {
            return ParseResult::bad;
        } else {
            req.headers.back().name.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::space_before_header_value:
        if (input == ' ') {
            m_state = ParseState::header_value;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::header_value:
        if (input == '\r') {
            m_state = ParseState::expecting_newline_2;
            return ParseResult::indeterminate;
        } else if (is_ctl(input)) {
            return ParseResult::bad;
        } else {
            req.headers.back().value.push_back(input);
            return ParseResult::indeterminate;
        }

    case ParseState::expecting_newline_2:
        if (input == '\n') {
            m_state = ParseState::header_line_start;
            return ParseResult::indeterminate;
        } else {
            return ParseResult::bad;
        }

    case ParseState::expecting_newline_3:
        return ((input == '\n')? ParseResult::good : ParseResult::bad);

    default:
        return ParseResult::bad;
    }
}

bool RequestParser::is_char(char c)
{
    return c >= 0 && c <= 127;
}

bool RequestParser::is_ctl(char c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool RequestParser::is_special(char c)
{
    const bool special = (c == '(' || c == ')' || c == '<' || c == '>' || c == '@'
                       || c == ',' || c == ';' || c == ':' || c == '\\'|| c == '"'
                       || c == '/' || c == '[' || c == ']' || c == '?' || c == '='
                       || c == '{' || c == '}' || c == ' ' || c == '\t');
    return special;
}

bool RequestParser::is_digit(char c)
{
    return c >= '0' && c <= '9';
}

} // namespace server
} // namespace http
