cmake_minimum_required (VERSION 2.8)
project (AsioHttpSever)

# Application --------------------------------------------------------
add_definitions(-DNOMINMAX -DUNICODE -D_UNICODE -DASIO_STANDALONE=1)
if(WIN32 OR WIN64)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()
if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") AND (NOT ANDROID_ABI))
    set(LINUX true)
endif()

include_directories(${PROJECT_SOURCE_DIR}/3rdParty/asio/asio/include
                    ${PROJECT_SOURCE_DIR}/3rdParty/GSL
                    ${PROJECT_SOURCE_DIR}/3rdParty/catch/include)

FILE(GLOB Source_Files "${PROJECT_SOURCE_DIR}/src/*.cpp")
FILE(GLOB Header_Files "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(server ${Source_Files} ${Header_Files})
set_target_properties(server PROPERTIES COMPILE_DEFINITIONS "DO_LOG")
if(WIN32 OR WIN64)
    add_definitions(-D_WIN32_WINNT=0x0601)
    target_compile_options(server PRIVATE "/W4")
elseif(LINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    target_link_libraries(server c++ c++abi pthread)
endif()

# Tests ---------------------------------------------------------
FILE(GLOB Test_Files "${PROJECT_SOURCE_DIR}/test/*.cpp")
list(APPEND Test_Files "${PROJECT_SOURCE_DIR}/src/connection.cpp"
                       "${PROJECT_SOURCE_DIR}/src/evaluator.cpp")
add_executable(tests ${Test_Files})
set_target_properties(tests PROPERTIES COMPILE_DEFINITIONS "TESTING")
if (LINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    target_link_libraries(tests c++ c++abi pthread)
endif()
