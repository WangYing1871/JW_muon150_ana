# CMakeList.txt : CMake project for adas_muxbrd_sync include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.8)

project("adas_muxbrd_fit")

find_package(ROOT REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/inc ${PROJECT_SOURCE_DIR}/../../my_pub/lib ${PROJECT_SOURCE_DIR}/../../my_pub/common_lib ${ROOT_INCLUDE_DIR})
include(${ROOT_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/../../my_pub/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/inc/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/lib/*.hh ${PROJECT_SOURCE_DIR}/../../my_pub/common_lib)

# Add source to this project's executable.
add_executable(adas_muxbrd_sync "adas_muxbrd_fit.cc" "adas_muxbrd_fit.hh" ${sources} ${headers})
target_link_libraries(adas_muxbrd_sync ${ROOT_LIBRARIES})
target_link_libraries(adas_muxbrd_sync Spectrum)

# TODO: Add tests and install targets if needed.
