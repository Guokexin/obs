cmake_minimum_required(VERSION 2.4)
aux_source_directory(. DIR_LIB_SRCS)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
link_directories(${CMAKE_SOURCE_DIR}/lib/linux/curl)
link_directories(${CMAKE_SOURCE_DIR}/lib/linux/uuid)
link_directories(${CMAKE_SOURCE_DIR}/lib/linux/openssl)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(tokenDemo  demo/tokenDemo.cpp)

target_link_libraries(tokenDemo nlsToken curl ssl crypto uuid)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/demo) 

