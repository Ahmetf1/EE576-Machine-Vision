cmake_minimum_required(VERSION 3.12)
project(Project4)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)


include_directories(include)


set(BOW_SOURCES
    src/bow.cpp
    src/BagOfWords.cpp
    src/DataProvider.cpp
)

set(BOW_HEADERS
    include/BagOfWords.h
    include/DataProvider.h
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(bow ${BOW_SOURCES} ${BOW_HEADERS})
target_link_libraries(bow ${OpenCV_LIBS})

