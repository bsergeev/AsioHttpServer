# Simple asynchronous HTTP 1.0 server based on non-Boost ASIO  

Based on Christopher M. Kohlhoff (chris@kohlhoff.com) ASIO C++11 http/server example:  
  https://github.com/chriskohlhoff/asio/tree/master/asio/src/examples/cpp11/http/server
  
Instead of the original ASIO, https://github.com/chriskohlhoff/asio.git, this repo uses my fork, https://github.com/bsergeev/asio.git, which includes a fix for a bug in ASIO, asio/include/asio/detail/consuming_buffers.hpp(104), fixed in my pull request:  
  https://github.com/chriskohlhoff/asio/pull/136  
If you were to use the original ASIO, you would have to patch it in order to run this server.

To build ASIO without Boost, a preprocessor variable `ASIO_STANDALONE=1` should be defined (this is already done in CMake file).
  
## Building on Windows and Linux
To build the server, run `build.bat` (with optional configuration, Debug or Release) on Windows or `bash.sh` on Linux.  
  
For manual build, iIt is recommended to build out-of-source, e.g. execute the following from the source directory:
```
mkdir build
cd build
cmake -G "generator" ..
```
where `"generator"` can be `"Visual Studio 14 2015 Win64"` or `"Unix Makefiles"`.  
Then, build it  
*on Windows*:  
either load _AsioHttpSever.sln_ in Visual Studio 2015 or build from VS2015 x64 command line:  
```  
msbuild AsioHttpSever.sln  
```
*on Linux*:
```  
make  
```
  
## Troubleshooting
On Linux, if building directly with CLang 4.0 succeeds:  
```  
clang -std=c++14 -I3rdParty/asio/asio/include/ -DASIO_STANDALONE=1 -DLOG -stdlib=libc++ -lc++ -lc++abi -lpthread -o server *.cpp  
```
but CMake generated build fails, specify that CLang should be used instead of GCC by exporting the following variables (you may want to add them to your ~/.bashrc):
```  
export CC=clang  
export CXX=clang++  
```