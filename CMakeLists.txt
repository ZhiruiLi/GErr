cmake_minimum_required(VERSION 3.13)
project(gErr)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(thirdparty/fmt)
include_directories("thirdparty/fmt/include")
include_directories("include")

add_executable(simpleerr examples/simpleerr/main.cpp)
target_link_libraries(simpleerr fmt::fmt)
add_executable(defineerr examples/defineerr/main.cpp)
target_link_libraries(defineerr fmt::fmt)
add_executable(simpletry examples/simpletry/main.cpp)
target_link_libraries(simpletry fmt::fmt)
