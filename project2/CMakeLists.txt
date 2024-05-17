cmake_minimum_required(VERSION 3.12)
project(Project2)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 17)

# Set include dir
include_directories(include)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/ImageTransformer.cpp
)


set(HEADERS
    include/ImageTransformer.h
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Create an executable target
add_executable(project2 ${SOURCES} ${HEADERS})
target_link_libraries( project2 ${OpenCV_LIBS} )

