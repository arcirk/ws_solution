cmake_minimum_required(VERSION 3.19)
project(ws_admin_wx)

set(CMAKE_CXX_STANDARD 17)

set(wxWidgets_ROOT_DIR </usr/local/include/wx-3.1>)
set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base  REQUIRED)
include(${wxWidgets_USE_FILE})

set(SOURCE_FILES include/wx_pch.h include/main.h include/wx_app.h include/wx_frame.h src/wx_app.cpp src/wx_frame.cpp src/main.cpp include/wx_panels.h src/wx_panels.cpp src/wx_addserver.cpp include/wx_connect.h src/ws_connect.cpp include/settings.h src/settings.cpp)

add_executable(ws_admin_wx ${SOURCE_FILES})

target_link_libraries(ws_admin_wx ${wxWidgets_LIBRARIES})

## Note that for MinGW users the order of libs is important!
#find_package(wxWidgets COMPONENTS gl core base OPTIONAL_COMPONENTS net)
#if(wxWidgets_FOUND)
#    include(${wxWidgets_USE_FILE})
#    # and for each of your dependent executable/library targets:
#    target_link_libraries(ws_admin_wx ${wxWidgets_LIBRARIES})
#endif()
