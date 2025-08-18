cmake_minimum_required(VERSION 3.13)
project(http-server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(SOURCE_FILES 
    main.cpp
    tcp_server.cpp
    router.cpp
)

add_executable(http-server ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(http-server PRIVATE ws2_32)
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(http-server PRIVATE Threads::Threads)
endif()

if(MSVC)
    target_compile_options(http-server PRIVATE /W4)
else()
    target_compile_options(http-server PRIVATE -Wall -Wextra -O2)
endif()