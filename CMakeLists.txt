cmake_minimum_required(VERSION 3.1...3.5)

project("JW_muon150_ana")

option (UNIX_USE_CCACHE "Treat warnings as error" ON)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")

find_package(Boost REQUIRED COMPONENTS system timer)
find_package(ROOT REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include
    ${ROOT_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIR}
    )
find_program(ROOTCINT rootcint PATH_SUFFIXES bin REQUIRED)
set(root_cint_out "root_cint_out.cpp")
if (EXISTS "${PROJECT_SOURCE_DIR}/data_strcut_cint.h" AND
    EXISTS "${PROJECT_SOURCE_DIR}/LinkDef.h")
  execute_process(COMMAND 
      "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR} -f ${PROJECT_SOURCE_DIR}/${root_cint_out} -c data_strcut_cint.h LinkDef.h)
  execute_process(COMMAND 
      "${ROOTCINT}" -I ${PROJECT_SOURCE_DIR} -f ${root_cint_out} -c data_strcut_cint.h LinkDef.h)
endif()


file(GLOB sources 
    ${PROJECT_SOURCE_DIR}/*.cpp
    ${PROJECT_SOURCE_DIR}/*.cxx
    )
file(GLOB headers
    ${PROJECT_SOURCE_DIR}/*.hpp
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.hxx
    )

set(exec_name "main")
add_executable(${exec_name} main.cpp ${sources} ${headers})
target_link_libraries(${exec_name} ${ROOT_LIBRARIES})
target_link_libraries(${exec_name} ${ROOT_LIBRARIES} Boost::timer)
