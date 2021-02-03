cmake_minimum_required(VERSION 3.11.3)

# set(CMAKE_CXX_STANDARD 17)
project(traffic_simulation)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

find_package(OpenCV 4.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

file(GLOB project_SRCS src/*.cpp)

set(EXECUTABLE_NAME "traffic_simulation")

# Add project executable
add_executable(${EXECUTABLE_NAME}
  ${project_SRCS}
)

target_link_libraries(${EXECUTABLE_NAME}
  ${OpenCV_LIBRARIES}
)
