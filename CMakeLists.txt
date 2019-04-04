cmake_minimum_required(VERSION 3.5.1)

set(EXECUTABLE gardener)

project(gardener VERSION 1.2.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOC_DEBUG -g -O0")

find_package(PkgConfig)

pkg_check_modules(iotivity-constrained-client-server REQUIRED iotivity-constrained-client-server)
pkg_check_modules(iotivity-constrained-cloudaccess REQUIRED iotivity-constrained-cloudaccess)
pkg_check_modules(iotivity-constrained-rd-client REQUIRED iotivity-constrained-rd-client)
find_library(PAHO_MQTT3C paho-mqtt3c)

add_definitions(-DOC_CLIENT -DOC_SERVER -DOC_DEBUG -g -O0  -DOC_PKI -DOC_DYNAMIC_ALLOCATION -DOC_IPV4 -DOC_TCP)

file(GLOB dev_src
        src/*.c
        src/*.h
        )

include_directories(
        include
)
include_directories(${iotivity-constrained-client-server_INCLUDE_DIRS})
include_directories(${iotivity-constrained-cloudaccess_INCLUDE_DIRS})
include_directories(${iotivity-constrained-rd-client_INCLUDE_DIRS})


add_executable(${EXECUTABLE} ${dev_src} )
target_link_libraries(${EXECUTABLE}
        pthread
        ${iotivity-constrained-client-server_LIBRARIES}
        ${iotivity-constrained-cloudaccess_LIBRARIES}
        ${iotivity-constrained-rd-client_LIBRARIES}
        ${PAHO_MQTT3C}
        )
