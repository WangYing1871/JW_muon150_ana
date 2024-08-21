# CMakeList.txt : CMake project for adas_data_unpack include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("adas_data_dec")

find_package(ROOT REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/inc ${PROJECT_SOURCE_DIR}/../../my_pub/lib ${PROJECT_SOURCE_DIR}/../../my_pub/common_lib ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/../../my_pub/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/lib/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/common_lib)


# Add source to this project's executable.
add_executable (adas_data_dec "adas_data_dec.cc" "adas_data_dec.hh"  ${sources} ${headers})
target_link_libraries(adas_data_dec  ${ROOT_LIBRARIES})
# TODO: Add tests and install targets if needed.
