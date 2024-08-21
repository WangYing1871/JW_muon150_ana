cmake_minimum_required (VERSION 3.8)

project ("tracker_seek")

find_package(ROOT REQUIRED)
include_directories(
	${PROJECT_SOURCE_DIR}/inc 
	${PROJECT_SOURCE_DIR}/../../my_pub/lib 
	${PROJECT_SOURCE_DIR}/../../my_pub/common_lib 
	${ROOT_INCLUDE_DIR}
)
include(${ROOT_USE_FILE})

file(GLOB sources 
	${PROJECT_SOURCE_DIR}/src/*.cc 
	${PROJECT_SOURCE_DIR}/../../my_pub/src/*.cc
)
file(GLOB headers 
	${PROJECT_SOURCE_DIR}/inc/*.hh 
	${PROJECT_SOURCE_DIR}/../../my_pub/lib/*.hh 
	${PROJECT_SOURCE_DIR}/../../my_pub/common_lib/*.hh
)


# Add source to this project's executable.

add_executable (tracker_seek "tracker_seek.cc" "tracker_seek.hh" ${sources} ${headers})
target_link_libraries(tracker_seek ${ROOT_LIBRARIES})
