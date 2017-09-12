#pragma once

// Thread safe logging macro
#ifndef DO_LOG
  #define LOG(x)
#else
  #include <iostream>
  #include <mutex>
  #include <thread>
  extern std::mutex gMutex;
  #define LOG(x) do{ gMutex.lock(); std::cout <<"["<<std::this_thread::get_id()<<"] "<< x; gMutex.unlock(); }while(false)
#endif
