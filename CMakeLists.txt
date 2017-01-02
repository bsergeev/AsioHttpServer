cmake_minimum_required (VERSION 3.0)
project (AsioHttpSever)

# Application --------------------------------------------------------
add_definitions(-DNOMINMAX -DUNICODE -D_UNICODE -DASIO_STANDALONE=1 -DLOG)
if(WIN32 OR WIN64)
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()
if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") AND (NOT ANDROID_ABI))
    set(LINUX true)
endif()

include_directories(${PROJECT_SOURCE_DIR}/3rdParty/asio/asio/include
                    ${PROJECT_SOURCE_DIR}/3rdParty/catch/include)

FILE(GLOB Source_Files "${PROJECT_SOURCE_DIR}/*.cpp")
FILE(GLOB Header_Files "${PROJECT_SOURCE_DIR}/*.h")

add_executable(server ${Source_Files} ${Header_Files})
if (LINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    target_link_libraries(server c++ c++abi pthread)
endif()

# Tests ---------------------------------------------------------
FILE(GLOB Test_Files "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(APPEND Test_Files "${PROJECT_SOURCE_DIR}/requestparser.cpp")
add_executable(tests ${Test_Files})
if (LINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    target_link_libraries(tests c++ c++abi pthread)
endif()

