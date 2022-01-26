cmake_minimum_required(VERSION 3.21)
project(bwebdav VERSION 1.0.1 DESCRIPTION "webdav client")

set(CMAKE_CXX_STANDARD 17)
set(TARGET bwebdav)
set(ignoreMe "${FOO}${BAZ}${BAR}")

if (WIN32)
    list(APPEND CMAKE_PREFIX_PATH C:/lib/webdav/webdav-client-cpp/_install/default/lib/cmake/WDC)
else()
    list(APPEND CMAKE_PREFIX_PATH /home/arcady/lib/webdav/webdav-client-cpp/_install/default/lib/cmake/WDC)
endif ()


add_library(bwebdav SHARED bwebdav.cpp bwebdav_api.h)

set_target_properties(bwebdav PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER bwebdav.h)

target_include_directories(${TARGET} PRIVATE
        ../../../lib/webdav/webdav-client-cpp/_install/default/include)

find_package(WDC CONFIG REQUIRED)
target_link_libraries(bwebdav WDC::libwdc)