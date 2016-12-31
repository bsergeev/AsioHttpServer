// A simple asynchronous HTTP 1.0 server implementing a clean shutdown by 
// canceling all outstanding asynchronous operations. 
// Based on Christopher M. Kohlhoff (chris@kohlhoff.com) ASIO C++11 http/server example:
// https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server

#include "server.h"
#include <iostream>
#ifdef WIN32
#include <crtdbg.h>
#endif

int main(int argc, char* argv[])
{
    // To enable detection of heap corruption and memory leaks, define FORMIT_HEAP_CHECKING.
    // Note: this will substantially slow down the Debug executable!
    //#define HEAP_CHECKING
    #if defined(_DEBUG) && defined(WIN32) && defined(HEAP_CHECKING)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
    #endif

    try
    {
        if (argc != 4) {
            std::cerr << "Usage: server <address> <port> <doc_root>\n";
            std::cerr << "  For IPv4, try:\n\t0.0.0.0 80 .\n";
            std::cerr << "  For IPv6, try:\n\t0::0 80 .\n";
            return 1;
        }

        // Initialize the server, then run it until stopped.
        http::server::Server{ argv[1], argv[2], argv[3] }.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
