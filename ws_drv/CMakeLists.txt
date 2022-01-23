cmake_minimum_required(VERSION 3.21)
project(ws_drv)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED YES)
#set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_POSITION_INDEPENDENT_CODE YES)
set(TARGET ws_drv)

if(WIN32)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

#option(NOGUI "disable building GUI project")

#option(CASE_INSENSITIVE "Case insensitive method names" OFF)
#option(STATIC_CRT "Static CRT linkage" OFF)
#option(OUT_PARAMS "Support output parameters" OFF)

#target_precompile_headers(${TARGET} PRIVATE F:/qt_sr/infactum/ws_client/include/stdafx.h)
#if (WIN32)
#    target_precompile_headers(${TARGET} PUBLIC stdafx.h)
#endif()


#add_definitions (-DBOOST_ALL_NO_LIB)
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    ON)

#if (NOGUI)

#    set (Boost_USE_STATIC_LIBS ON)
#    #set (Boost_USE_MULTITHREADED OFF)
#    set (Boost_USE_STATIC_RUNTIME ON)

    if (MSVC)

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
#
    endif()
#
#    if (CMAKE_COMPILER_IS_GNUCXX)
#        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
#    endif()

#endif()

list(APPEND SOURCES
        src/addin.def
        src/stdafx.h
        src/dllmain.cpp
        src/exports.cpp
        src/Component.cpp
        src/Component.h
        src/ws_drv.cpp
        src/ws_drv.h
        ../ws_client/include/net.h
        ../ws_client/include/beast.h
        ../ws_client/include/ws_session.h
        ../ws_client/include/ws_client.h
        ../ws_client/src/iws_client.cpp
        ../ws_client/src/ws_session.cpp
        ../ws_client/src/ws_client.cpp
        ../arcirk/include/arcirk.h
        ../arcirk/src/arcirk.cpp
        ../arcirk/src/arcirk_ip.cpp
        ../arcirk/src/arcirk_json.cpp
        ../arcirk/src/arcirk_message.cpp
        ../arcirk/src/arcirk_settings.cpp
        ../arcirk/src/arcirk_static.cpp
        include/server_response.h
        src/server_response.cpp
        )

#if (ANDROID)
#    list(APPEND SOURCES
#            src/jnienv.cpp
#            src/jnienv.h)
#endif ()

add_library(${TARGET} SHARED
        ${SOURCES})

#target_compile_definitions(${TARGET} PRIVATE
#        UNICODE
#        _UNICODE)

#if (CASE_INSENSITIVE)
#    target_compile_definitions(${TARGET} PRIVATE CASE_INSENSITIVE)
#endif ()
#
#if (OUT_PARAMS)
#    target_compile_definitions(${TARGET} PRIVATE OUT_PARAMS)
#endif ()

target_include_directories(${TARGET} PRIVATE
        include
        ../arcirk/include
        ../ws_client/include
        ../webdav_plugin)

#if (WIN32 AND NOT MSVC)
#    message(FATAL_ERROR "Must be compiled with MSVC on Windows")
#endif ()

if (WIN32)
#    if (STATIC_CRT)
#        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#        string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
#    endif ()
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

#if (ANDROID)
#    if (CMAKE_BUILD_TYPE STREQUAL Release)
#        add_custom_command(TARGET ${TARGET} POST_BUILD
#                COMMAND ${CMAKE_STRIP} ${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET}${CMAKE_SHARED_LIBRARY_SUFFIX})
#    endif ()
#endif ()

#if (UNIX)
    find_package(Boost REQUIRED COMPONENTS regex filesystem system)
#else()
#    find_package(Boost REQUIRED COMPONENTS regex)
#endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set (NMOS_CPP_SYSTEM_LIBS bcrypt.lib)
else()
    set (NMOS_CPP_SYSTEM_LIBS )
endif()

#find_package(WDC CONFIG REQUIRED)

include_directories(
        ${Boost_INCLUDE_DIRS}
)
#link_directories(C:/lib/webdav/webdav-client-cpp/_install/default/lib)
#link_directories(C:/lib/webdav/webdav-client-cpp/cmake-build-release)
target_link_libraries(ws_drv
        ${Boost_LIBRARIES}
        ${NMOS_CPP_SYSTEM_LIBS})
        #WDC::libwdc)
#        arcirk
#        ws_client)
#        #ws_client)