#pragma once

namespace http 
{
    enum class StatusCode {
        OK                  = 200,
        BadRequest          = 400,
        NotFound            = 404,
        RequestTooLarge     = 413,
        ServerError         = 500,
        NotImplemented      = 501,
        HttpVerNotSupported = 505
    };
};
