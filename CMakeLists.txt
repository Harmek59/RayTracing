cmake_minimum_required(VERSION 3.7)
project(rayTracing)

#----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#----------------------------------------------------------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
find_package(tinyobjloader CONFIG REQUIRED)

#----------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES Sources/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/Sources/include ${PROJECT_SOURCE_DIR}/Sources/core ${PROJECT_SOURCE_DIR}/Sources/ogl ${PROJECT_SOURCE_DIR}/Sources/app)
add_executable(${PROJECT_NAME} Sources/main.cpp ${SOURCES})

#----------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE glad::glad)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE tinyobjloader::tinyobjloader)