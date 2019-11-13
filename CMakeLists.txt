cmake_minimum_required(VERSION 3.5.1)

set(EXECUTABLE gardener)

project(gardener VERSION 1.2.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOC_DEBUG -g -O0")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-omit-frame-pointer -fsanitize=address")

find_package(PkgConfig)

pkg_check_modules(iotivity-lite-client-server REQUIRED iotivity-lite-client-server)


add_definitions(-DOC_CLIENT -DOC_SERVER -DOC_CLOUD -DOC_DEBUG -g -O0  -fsanitize=address -DOC_PKI -DOC_DYNAMIC_ALLOCATION -DOC_IPV4 -DOC_TCP)

file(GLOB dev_src
        src/*.c
        src/*.h
        )

include_directories(
        include
)
include_directories(${iotivity-lite-client-server_INCLUDE_DIRS})


add_executable(${EXECUTABLE} ${dev_src} src/serial.h src/serial.c)
target_link_libraries(${EXECUTABLE}
        pthread
        ${iotivity-lite-client-server_LIBRARIES}
        )
