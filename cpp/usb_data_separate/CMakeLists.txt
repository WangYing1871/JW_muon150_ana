# CMakeList.txt : CMake project for usb_data_seperate, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project ("usb_data_separate")

include_directories(${PROJECT_SOURCE_DIR}/../../my_pub/common_lib)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/common_lib/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/lib/getopt.hh)


# Add source to this project's executable.
add_executable (usb_data_separate "usb_data_separate.cc" "usb_data_separate.hh" ${sources} ${headers} )
target_link_libraries(usb_data_separate)
# TODO: Add tests and install targets if needed.
