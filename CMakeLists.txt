cmake_minimum_required(VERSION 2.8)

project(motion_detection)

set(SOURCE PythonMailPhotoSender.cpp MotionDetector.cpp main.cpp)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(motion_detection ${SOURCE})

target_link_libraries(motion_detection ${OpenCV_LIBS})
