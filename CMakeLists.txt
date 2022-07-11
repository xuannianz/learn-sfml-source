# https://zhuanlan.zhihu.com/p/399335152
cmake_minimum_required(VERSION 3.12)
set (CMAKE_CXX_STANDARD 14)

project(learn-sfml-source)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

set(sfml_version 2.0)
set(sfml_dir SFML-${sfml_version})
add_subdirectory(${sfml_dir})

add_executable(sfml_test src/main.cpp)
target_include_directories(sfml_test PUBLIC ${sfml_dir}/include)
message(OPENGL_LIBRARIES=${OPENGL_LIBRARIES})
target_link_libraries(sfml_test PUBLIC sfml-system sfml-window sfml-graphics OpenGL::GL)