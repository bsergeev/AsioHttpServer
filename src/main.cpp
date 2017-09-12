// Asynchronous multithreaded HTTP 1.0 server based on C++11 (non-Boost) ASIO.

// This code started with HTTP server example from Chapter 5 of 
// `Boost.Asio C++ Network Programming Cookbook` book by Dmytro Radchuk.
// It was converted from Boost ASIO to C++11 ASIO and extensively refactored.

// To make this server do whatever task you need it to, add code to 
// Connection::processRequest.

// To build the server with CLang (without running CMake, so that Windows and
// Linux build can coexist), execute the following from the root of the repo:
// clang -std=c++14 -I3rdParty/asio/asio/include/ -I3rdParty/GSL/ -DASIO_STANDALONE=1 -DDO_LOG -stdlib=libc++ -lc++ -lc++abi -lpthread -o build/server src/*.cpp


#include "connection.h"
#include "log.h"

#include <asio.hpp>

#include <atomic>
#include <thread>
#ifdef WIN32
#include <crtdbg.h>
#endif

using namespace std::string_literals;

#ifdef DO_LOG
  std::mutex gMutex;
#endif

//------------------------------------------------------------------------------
class Acceptor {
public:
    Acceptor(asio::io_service& ios, unsigned short port_num)
        : m_ioService(ios),
          m_acceptor(m_ioService, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)),
          m_isStopped(false)
    {
        LOG("Listening on port " << port_num << std::endl);
    }

    void Start() {
        m_acceptor.listen();
        initAccept();
    }

    // Stop accepting incoming connection requests.
    void Stop() {
        m_isStopped.store(true);
        LOG("Stopping..."<< std::endl);
    }

private:
    // IMPORTANT: Notice how, as soon as a new HTTP request gets accepted and 
    //            the callback of async_accept() gets invoked, it fires up
    //            a new Connection and then (non-recursively) calls itself
    //            to start waiting for the next HTTP request. Slick!
    void initAccept() 
    {
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ioService));
        m_acceptor.async_accept(*sock.get(),
            [this, sock](const std::error_code& e) {
                if (!e) {
                    // The new Connection object will destroy itself in Connection::processRequest()
                    (new Connection(sock))->ProcessRequest();
                } else {
                    LOG("Error occurred[4]: "<< e.value() <<", \""<< e.message() <<"\"\n");
                }

                // Init next async accept operation, if acceptor has not been stopped yet.
                if (!m_isStopped.load()) {
                    initAccept();
                } else { // Stop accepting incoming connections and free allocated resources.
                    m_acceptor.close();
                }
            });
    }

//data:
    asio::io_service&       m_ioService;
    asio::ip::tcp::acceptor m_acceptor;
    std::atomic<bool>       m_isStopped;
}; // class Acceptor
//------------------------------------------------------------------------------
class Server {
public:
    Server(unsigned short port_num, size_t thread_pool_size) 
        : m_work(new asio::io_service::work(m_ioService)),
          m_acceptor(new Acceptor(m_ioService, port_num))
    {
        assert(thread_pool_size > 0);
        LOG("Starting "<< thread_pool_size <<" worker threads\n");
        m_threadPool.reserve(thread_pool_size);
        for (size_t i = 0; i < thread_pool_size; ++i) {
            m_threadPool.push_back(std::make_unique<std::thread>([this](){ m_ioService.run(); }));
        }
        m_acceptor->Start();
    }

    void Stop() 
    {
        m_acceptor->Stop();
        m_ioService.stop();
        for (auto& th : m_threadPool) {
            th->join();
        }
    }

private:
    asio::io_service                          m_ioService;
    std::unique_ptr<asio::io_service::work>   m_work;
    std::unique_ptr<Acceptor>                 m_acceptor;
    std::vector<std::unique_ptr<std::thread>> m_threadPool;
}; // class Server
//------------------------------------------------------------------------------
#ifndef TESTING
int main(int, char*[])
{
    // To enable detection of heap corruption and memory leaks, define HEAP_CHECKING.
    // Note: this will substantially slow down the Debug executable!
    //#define HEAP_CHECKING
    #if defined(_DEBUG) && defined(WIN32) && defined(HEAP_CHECKING)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
    #endif

    constexpr unsigned short port_num = 8080;
    try {
        const size_t hardware_threads = std::thread::hardware_concurrency();
        const size_t thread_pool_size = (hardware_threads)? 2*hardware_threads : 2;
        Server srv(port_num, thread_pool_size);

        LOG("Press ENTER to exit!\n");
        while(true) { auto c = std::cin.get(); if (c==69) break; }

        srv.Stop();
    } catch (const std::exception& e) {
        LOG("Error occurred[5]: "<< e.what() << std::endl);
    }

    return 0;
}
#endif
