# Asynchronous multithreaded HTTP 1.0 server based on non-Boost ASIO  
This repo uses submodules, _asio_, _GSL_, and _catch_. The easiest way to clone it is:  
```
git clone --recursive https://git.autodesk.com/sergeeb/AsioMtHttpServer.git
```
  
This code started with HTTP server example from Chapter 5 of _Boost.Asio C++ Network Programming Cookbook_ book by Dmytro Radchuk.    
It was converted from Boost ASIO to C++11 ASIO and extensively refactored.  
  
In general, to use ASIO without Boost, a preprocessor variable `ASIO_STANDALONE=1` should be defined (this is already done in CMake file).
  
Your code that does something useful, e.g. answers REST API call, goes into purpose::Evaluate() in src/evaluator.cpp.  
  
To test PUT request with some file, `file_name`, execute the following (on Windows, from Bash):
```
curl --header "custom: header" --upload-file file_name http://127.0.0.1:8080 -v
```

## Building on Windows and Linux
To build the server, run `build.bat` (with optional configuration, Debug or Release) on Windows or `bash.sh` on Linux.  
  
These scripts perform out-of-source build by executing the following commands from the source directory:
```
mkdir build
cd build
cmake -G "generator" ..
```
where `"generator"` is either `"Visual Studio 14 2015 Win64"` or `"Unix Makefiles"`.  
Then, *on Windows*, the solution is built from VS2015 x64 command line:  
```  
msbuild AsioHttpSever.sln  
```
You can also manually load _AsioHttpSever.sln_ in Visual Studio 2015 to debug it.  
  
*On Linux*, the script simly executes:
```  
make  
```
Finally, the build script runs tests from _test_ folder.  

## Troubleshooting
On Linux, if building directly with CLang 4.0 succeeds:  
```  
clang -std=c++14 -I3rdParty/asio/asio/include/ -I3rdParty/GSL/ -DASIO_STANDALONE=1 -DDO_LOG -stdlib=libc++ -lc++ -lc++abi -lpthread -o server src/*.cpp  
```
but CMake generated build fails, specify that CLang should be used instead of GCC by exporting the following variables (you may want to add them to your ~/.bashrc):
```  
export CC=clang  
export CXX=clang++  
```