cmake_minimum_required(VERSION 3.21)
project(ws_drv)

set(CMAKE_CXX_STANDARD 17)
set(TARGET ws_drv)

option(CASE_INSENSITIVE "Case insensitive method names" OFF)
if (WIN32)
    option(STATIC_CRT "Static CRT linkage" ON)
else()
    option(STATIC_CRT "Static CRT linkage" OFF)
endif ()

option(OUT_PARAMS "Support output parameters" OFF)

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
endif()

list(APPEND SOURCES
        src/addin.def
        src/stdafx.h
        src/dllmain.cpp
        src/exports.cpp
        src/Component.cpp
        src/Component.h
        src/ws_drv.cpp
        src/ws_drv.h)



if (WIN32)
    add_library(${TARGET} SHARED
            ${SOURCES})
else()
    file(GLOB ARCIRK_SOURCES ../arcirk/src/*.cpp)
    file(GLOB WDC_SOURCES ../wdc/src/*.cpp ../wdc/src/*.hpp)
    file(GLOB PUGI_SOURCES ../pugixml/*.cpp)
    add_library(${TARGET} SHARED
            ${SOURCES}
            ${ARCIRK_SOURCES}
            ${WDC_SOURCES}
            ${PUGI_SOURCES})
endif ()


set(ARCIRK_INCLUDE ../arcirk/include)
set(ARCIRK_LIB ../arcirk/cmake-build-debug)
set(WDC_LIB ../wdc/cmake-build-debug)
set(WDC_INCLUDE ../wdc/include)
set(PUGI_INCLUDE ../pugixml)

#target_compile_definitions(${TARGET} PRIVATE UNICODE _UNICODE)

target_include_directories(${TARGET} PRIVATE
        include
        ${ARCIRK_INCLUDE}
        ${PUGI_INCLUDE}
        ${WDC_INCLUDE})

set (LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH})
set (CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

set(MySuffix2 "")
#if (UNIX)
#    set(MySuffix2 "")
#else(UNIX)
#    if (NOT MSVC)
#        message(FATAL_ERROR "Must be compiled with MSVC on Windows")
#    endif(NOT MSVC)
#    set(MyPrefix "lib")
#    set(MySuffix1 "Win")
#endif(UNIX)

set_target_properties( ${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME ${MyPrefix}${PROJECT_NAME}${MySuffix1}${MySuffix2}
        POSITION_INDEPENDENT_CODE ON
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD 17
        )

if (UNIX)
#    if (TARGET_PLATFORM_32)
#        set(CMAKE_C_FLAGS "-m32 ${CMAKE_C_FLAGS}")
#        set(CMAKE_CXX_FLAGS "-m32 ${CMAKE_CXX_FLAGS}")
#    else(TARGET_PLATFORM_32)
#        set(CMAKE_C_FLAGS "-m64 ${CMAKE_C_FLAGS}")
#        set(CMAKE_CXX_FLAGS "-m64 ${CMAKE_CXX_FLAGS}")
#    endif ()
#    target_link_libraries(${PROJECT_NAME} -static-libstdc++)
else(UNIX)
    add_definitions(/MT)
    set(CMAKE_SUPPRESS_REGENERATION 1)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    target_compile_definitions(${TARGET} PRIVATE _WINDOWS
            _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
    target_compile_options(${TARGET} PRIVATE /utf-8)
endif(UNIX)

if (WIN32)
    link_directories(
            ${ARCIRK_LIB}
            ${WDC_LIB})
endif ()


if (WIN32)
    find_package(Boost REQUIRED COMPONENTS regex)
    target_link_libraries(${TARGET} PRIVATE
            bcrypt.lib libwdc arcirk)

    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${WDC_INCLUDE}
    )

else()
    find_package(OpenSSL REQUIRED)
    find_package(CURL REQUIRED)
    find_package(Boost COMPONENTS regex filesystem system thread REQUIRED)
    include_directories (
            ${Boost_INCLUDE_DIRS}
            ${ARCIRK_INCLUDE}
            ${PUGI_INCLUDE}
            ${WDC_INCLUDE})

    target_link_libraries(${TARGET} PRIVATE
            Boost::filesystem
            Boost::system
            Boost::thread
            OpenSSL::SSL
            OpenSSL::Crypto
            ${CURL_LIBRARIES}
            -static-libstdc++)
endif()

