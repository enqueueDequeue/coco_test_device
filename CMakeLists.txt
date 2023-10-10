cmake_minimum_required(VERSION 2.6)

project(deviceapp)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -pedantic-errors -Wall -std=c11 -D_GNU_SOURCE -Os")

file(GLOB SRCS *.c *.h)

add_executable(deviceapp ${SRCS})

target_link_libraries(deviceapp cocodevicesdk)
target_link_libraries(deviceapp dl)
target_link_libraries(deviceapp pthread)
target_link_libraries(deviceapp m)

