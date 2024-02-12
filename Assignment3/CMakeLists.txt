cmake_minimum_required(VERSION 3.24)
project(Assignment3)

set(CMAKE_CXX_STANDARD 17)

add_executable(Assignment3 main.cpp)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()