cmake_minimum_required(VERSION 3.12)
project(Project5)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 17)

# Set include dir
include_directories(include)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/ImageFlow.cpp
)


set(HEADERS
    include/ImageFlow.h
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Create an executable target
add_executable(project5 ${SOURCES} ${HEADERS})
target_link_libraries( project5 ${OpenCV_LIBS} )

