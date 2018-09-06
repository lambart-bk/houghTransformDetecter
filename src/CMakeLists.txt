
find_package(OpenCV  REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} )


add_executable(houghtransformdetecter houghtransformdetecter.cpp)
target_link_libraries(houghtransformdetecter ${OpenCV_LIBRARIES} )


