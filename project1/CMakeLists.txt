cmake_minimum_required(VERSION 3.12)
project(Project1)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Set include dir
include_directories(include)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/ImageViewer.cpp
)


set(HEADERS
    include/ImageViewer.h
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Create an executable target
add_executable(project1 ${SOURCES} ${HEADERS})
target_link_libraries( project1 ${OpenCV_LIBS} )

