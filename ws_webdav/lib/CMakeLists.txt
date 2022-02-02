cmake_minimum_required(VERSION 3.20)
project(ws_webdav VERSION 1.0.1 DESCRIPTION "webdav client")

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_webdav)

add_library(${TARGET} src/bwebdav.cpp include/bwebdav.h)

#set_target_properties(${TARGET} PROPERTIES
#        VERSION ${PROJECT_VERSION}
#        SOVERSION 1
#        PUBLIC_HEADER include/bwebdav.h)

find_package(OpenSSL)
find_package(Boost REQUIRED COMPONENTS regex filesystem)
target_include_directories(${TARGET} PRIVATE
        ../../arcirk/include)

include_directories(
        ${Boost_INCLUDE_DIRS}
)
link_directories(../../arcirk/cmake-build-debug)
target_link_libraries(${TARGET} PRIVATE
        ${Boost_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        OpenSSL::SSL OpenSSL::Crypto
        arcirk)