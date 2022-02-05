cmake_minimum_required(VERSION 3.19)
project(ws_client_console)

set(CMAKE_CXX_STANDARD 20)
set(TARGET ws_client_console)

set(WS_CLIENT_INCLUDE ../ws_client/include)
set(WS_CLIENT_LIB ../ws_client/cmake-build-debug)
set(CHARED_INCLUDE ../chared)
set(ARCIRK_INCLUDE ../arcirk/include)
set(ARCIRK_LIB ../arcirk/cmake-build-debug)

#option(CASE_INSENSITIVE "Case insensitive method names" OFF)
#if (WIN32)
#    option(STATIC_CRT "Static CRT linkage" ON)
#else()
#    option(STATIC_CRT "Static CRT linkage" OFF)
#endif ()
#
#option(OUT_PARAMS "Support output parameters" OFF)

add_executable(${TARGET}
        src/main.cpp
        ../chared/server_response.cpp
        ../chared/server_response.h
        )


if (WIN32)
    add_definitions(/MT)
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
endif()


if (UNIX)
    SET(CMAKE_CXX_FLAGS -pthread)
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wunreachable-code -Wpedantic)
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET} PRIVATE -Wweak-vtables -Wexit-time-destructors -Wglobal-constructors -Wmissing-noreturn )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${TARGET} PRIVATE /W4 /w44265 /w44061 /w44062)
endif()


if (WIN32)
    link_directories(
            ${ARCIRK_LIB}
            ${WS_CLIENT_LIB})
endif ()


#link_directories(../ws_client/cmake-build-debug)

if (WIN32)
    find_package(Boost REQUIRED COMPONENTS regex)
    target_link_libraries(${TARGET} PRIVATE
            bcrypt.lib arcirk ws_client)

    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${WS_CLIENT_INCLUDE}
            ${CHARED_INCLUDE}
    )

else()
#    find_package(OpenSSL REQUIRED)
#    find_package(CURL REQUIRED)
    find_package(Boost COMPONENTS regex filesystem system thread REQUIRED)
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${WS_CLIENT_INCLUDE}
            ${CHARED_INCLUDE}
    )

    target_link_libraries(${TARGET} PRIVATE
            Boost::filesystem
            Boost::system
            Boost::thread)
endif()
