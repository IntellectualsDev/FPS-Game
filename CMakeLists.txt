cmake_minimum_required(VERSION 3.20)
project(FPS-Game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)



set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()
#include_directories("C:/Users/Joseph/Desktop/Intellectuals/FPS-Game/DevEnv/flatbuffers/include")
include_directories("/Users/anshulgowda/Documents/CODE/Odyssey/FPS-Game/DevEnv/flatbuffers/include")
include_directories(/usr/local/include)
#include_directories("C:/Users/Joseph/Desktop/Intellectuals/FPS-Game/DevEnv/Enet Headers/include")
#link_directories(C:/Users/Joseph/Desktop/Intellectuals/FPS-Game/Dev Libs/"Dev_Libs") #contains enet.lib, I don't need that
link_directories(/usr/local/lib)
link_directories(/usr/local/bin)

#find_library(ENET_LIBRARY NAMES enet.a PATHS "Dev_Libs")
#if(NOT ENET_LIBRARY)
#    MESSAGE(FATAL_ERROR "ENet library not found")
#endif()
#
#message(STATUS "ENet library found: ${ENET_LIBRARY}")



add_executable(FPS-Game main.cpp Player.cpp Bullet.cpp game_state_generated.h game_state.fbs Transmitter.cpp Transmitter.h PacketBuffer.cpp PacketBuffer.h DummyClient.cpp DummyClient.h Gateway.cpp Gateway.h BufferHandler.h)

target_link_libraries(${PROJECT_NAME} raylib)
#target_link_libraries(${PROJECT_NAME} ${ENET_LIBRARY} ws2_32 winmm)
target_link_libraries(${PROJECT_NAME} enet)

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")
endif()

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()