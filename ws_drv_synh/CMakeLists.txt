cmake_minimum_required(VERSION 3.10)
project(ws_srv_synh)

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_srv_synh)

option(CASE_INSENSITIVE "Case insensitive method names" OFF)
if (WIN32)
    option(STATIC_CRT "Static CRT linkage" ON)
else()
    option(STATIC_CRT "Static CRT linkage" OFF)
endif ()

option(OUT_PARAMS "Support output parameters" OFF)

if (WIN32)
    #add_definitions(-DBOOST_ALL_NO_LIB)
    set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
    set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
    set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
    set(Boost_USE_MULTITHREADED      ON)
    set(Boost_USE_STATIC_RUNTIME    ON)

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

list(APPEND SOURCES
        src/addin.def
        src/stdafx.h
        src/dllmain.cpp
        src/exports.cpp
        src/Component.cpp
        src/Component.h
        src/ws_drv_synh.cpp
        src/ws_drv_synh.h)

add_library(${TARGET} SHARED
        ${SOURCES})

set(ARCIRK_INCLUDE ../arcirk/include)
set(ARCIRK_LIB ../arcirk/cmake-build-debug)

#target_compile_definitions(${TARGET} PRIVATE
#        UNICODE
#        _UNICODE)

if (CASE_INSENSITIVE)
    target_compile_definitions(${TARGET} PRIVATE CASE_INSENSITIVE)
endif ()

if (OUT_PARAMS)
    target_compile_definitions(${TARGET} PRIVATE OUT_PARAMS)
endif ()

target_include_directories(${TARGET} PRIVATE
        include
        ${ARCIRK_INCLUDE})

if (WIN32)
    link_directories(
            ${ARCIRK_LIB}
    )
endif ()
if (WIN32)
    add_definitions(/MT)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    target_compile_definitions(${TARGET} PRIVATE _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)

    #find_package(Boost REQUIRED COMPONENTS regex thread)
    target_link_libraries(${TARGET} PRIVATE
            bcrypt.lib
            arcirk
            )
    include_directories (
            #${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
    )

else()
    #find_package(Boost COMPONENTS regex filesystem system thread REQUIRED)
    include_directories (
            #${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
    )

    target_link_libraries(${TARGET} PRIVATE
#            Boost::filesystem
#            Boost::system
#            Boost::thread
#            OpenSSL::SSL
#            OpenSSL::Crypto
#            ${CURL_LIBRARIES}
            -static-libstdc++)
endif()
if (WIN32 AND NOT MSVC)
    message(FATAL_ERROR "Must be compiled with MSVC on Windows")
endif ()

if (WIN32)
    if (STATIC_CRT)
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    endif ()
    target_compile_definitions(${TARGET} PRIVATE
            _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif ()

if (UNIX)
    if (TARGET_ARCH STREQUAL "x86")
        set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
    endif ()
endif ()
