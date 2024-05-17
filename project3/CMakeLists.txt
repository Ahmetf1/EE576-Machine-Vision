cmake_minimum_required(VERSION 3.12)
project(Project3)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)


include_directories(include)


set(VIEWER_SOURCES
    src/viewer.cpp
    src/ImageProcessor.cpp
)

set(VIEWER_HEADERS
    include/ImageProcessor.h
    include/BagOfWords.h
)

set(BOW_SOURCES
    src/bow.cpp
    src/BagOfWords.cpp
    src/ImageProcessor.cpp
)

set(BOW_HEADERS
    include/BagOfWords.h
    include/ImageProcessor.h
)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(viewer ${VIEWER_SOURCES} ${VIEWER_HEADERS})
target_link_libraries(viewer ${OpenCV_LIBS})

add_executable(bow ${BOW_SOURCES} ${BOW_HEADERS})
target_link_libraries(bow ${OpenCV_LIBS})

