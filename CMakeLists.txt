cmake_minimum_required(VERSION 2.8)

project(motion_detection)

set(SOURCE PythonMailPhotoSender.cpp MotionDetector.cpp main.cpp)

add_executable(motion_detection ${SOURCE})

target_link_libraries(motion_detection opencv_core opencv_highgui opencv_imgproc)

