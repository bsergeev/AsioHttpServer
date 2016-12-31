# Simple asynchronous HTTP 1.0 server based on non-Boost ASIO  

Based on Christopher M. Kohlhoff (chris@kohlhoff.com) ASIO C++11 http/server example:  
  https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server
  
You have to manually clone ASIO repo, https://github.com/chriskohlhoff/asio.git, (I didn't bother to set up submodules) and, if you don't want to modify my CMake file, place it at the same level as this repo, i.e. it woul be located at `..\asio` from this repo.

Currently, there's a bug in ASIO, asio/include/asio/detail/consuming_buffers.hpp(104), fixed in a pull request:  
  https://github.com/chriskohlhoff/asio/pull/136  
You'll have to patch ASIO in order to run this server.

To build ASIO without Boost, a preprocessor variable `ASIO_STANDALONE=1` should be defined (this is already done in CMake file).

## Building on Windows and Linux
It is recommended to build out-of-source, e.g. execute the following from the source directory:
```
mkdir build
cd build
cmake -G "generator" ..
```
where `"generator"` can be `"Visual Studio 14 2015 Win64"` or `"Unix Makefiles"`.  
Then, build it  
*on Windows*:  
either load _AsioSampleHttpSever.sln_ in Visual Studio 2015 or build from VS2015 x64 command line:  
```  
msbuild AsioSampleHttpSever.sln  
```
*on Linux*:
```  
make  
```
On Linux, if building directly with CLang 4.0 succeeds:  
```  
clang -std=c++14 -I../asio/asio/include/ -DASIO_STANDALONE=1 -DLOG -stdlib=libc++ -lc++ -lc++abi -lpthread -o server *.cpp  
```
but CMake generated build fails, specify that CLang should be used instead of GCC by exporting the following variables (you may want to add them to your ~/.bashrc):
```  
export CC=clang  
export CXX=clang++  
```