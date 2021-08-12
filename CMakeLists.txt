cmake_minimum_required(VERSION 3.7)
project (rayTracing)

#----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_CXX_STANDARD 17)
#----------------------------------------------------------------------------
# prevent for in-source building
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

#----------------------------------------------------------------------------
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)
set(PROJECT_LINK_DIRECTORY ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "-Wall")
add_compile_options(${CMAKE_CXX_FLAGS})

#----------------------------------------------------------------------------
find_package(glfw3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")
find_package(tinyobjloader CONFIG REQUIRED)

#----------------------------------------------------------------------------
include_directories(${PROJECT_SOURCE_DIR}/Sources/include)
file(GLOB SOURCES "Sources/src/*.cpp")

#----------------------------------------------------------------------------
add_executable(${PROJECT_NAME} Sources/main.cpp ${SOURCES} Sources/src/stb_image_IMPL.cpp)

#----------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME}  PRIVATE imgui::imgui)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE tinyobjloader::tinyobjloader)




