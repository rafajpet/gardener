cmake_minimum_required(VERSION 3.7)

set(EXECUTABLE gardener)

project(gardener VERSION 1.2.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)

find_package(PkgConfig)

pkg_check_modules(iotivity-constrained-client-server REQUIRED iotivity-constrained-client-server)
pkg_check_modules(iotivity-constrained-cloudaccess REQUIRED iotivity-constrained-cloudaccess)
pkg_check_modules(iotivity-constrained-rd-client REQUIRED iotivity-constrained-rd-client)


add_definitions(-DOC_CLIENT -DOC_SERVER)

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


add_executable(${EXECUTABLE} ${dev_src} src/main.c)
target_link_libraries(${EXECUTABLE}
        pthread
        ${iotivity-constrained-client-server_LIBRARIES}
        ${iotivity-constrained-cloudaccess_LIBRARIES}
        ${iotivity-constrained-rd-client_LIBRARIES}
        )
