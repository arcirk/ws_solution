cmake_minimum_required(VERSION 3.21)
project(ws_server)

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_server)

if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif()

if (WIN32)

    set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
    set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
    set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    ON)
    #        set (Boost_USE_MULTITHREADED ON)
    #
    foreach (flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)

        if (${flag_var} MATCHES "/MD")
            string (REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        endif (${flag_var} MATCHES "/MD")

    endforeach(flag_var)

    set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT")
    set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:MSVCRTD")

endif()

if(WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_definitions(/MT)
endif()

SET(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/x64/)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(ARCIRK_LIB ../arcirk/cmake-build-debug)
set(ARCIRK_INCLUDE ../arcirk/include)
set(SHARED_INCLUDE ../chared)
set(SQLWRAPPER_INCLUDE ../sqlwrapper/include)
set(SQLWRAPPER_LIB ../sqlwrapper/x64-static)

link_directories(
        ${ARCIRK_LIB}
        ${SQLWRAPPER_LIB}
)

add_executable(${TARGET}
        main.cpp
        include/websocket_session.hpp
        include/shared_state.hpp
        include/net.hpp
        include/listener.hpp
        include/http_session.hpp
        include/beast.hpp
        http_session.cpp
        listener.cpp
        shared_state.cpp
        websocket_session.cpp
        include/subscriber.h
        subscriber.cpp
        include/public_channels.h
        sqlite/sqlite3.c
        sqlite/sqlite3.h
        include/base.h
        src/base.cpp)

#find_package(Boost REQUIRED COMPONENTS regex filesystem system CONFIG)
#find_package (SQLite3 REQUIRED CONFIG)

target_include_directories(${TARGET} PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${ARCIRK_INCLUDE}
        ${SQLWRAPPER_INCLUDE}
        )
#set(Boost_INCLUDE_DIR ${_VCPKG_INSTALLED_DIR}/x64-windows-static/include)
if (WIN32)
    #set(Boost_INCLUDE_DIR "C:/lib/vcpkg/installed/x64-windows-static/include")
    
    find_package(Boost REQUIRED COMPONENTS regex)
    target_link_libraries(${TARGET} PRIVATE
            bcrypt.lib arcirk sqlwrapper)

    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${SQLWRAPPER_INCLUDE}
    )

else()

    find_package(Boost COMPONENTS thread regex filesystem system REQUIRED)
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${SQLWRAPPER_INCLUDE}
    )

    target_link_libraries(${TARGET} PRIVATE
            Boost::filesystem
            Boost::system
            Boost::thread
            arcirk
            sqlwrapper)

endif()

add_custom_command(TARGET ${TARGET} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ws_server/app/ ${CMAKE_BINARY_DIR})

if (WIN32)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm
            ${CMAKE_BINARY_DIR}/sqlwrapper.dll)
    add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/sqlwrapper/x64-static/sqlwrapper.dll ${CMAKE_BINARY_DIR}/sqlwrapper.dll)
endif()