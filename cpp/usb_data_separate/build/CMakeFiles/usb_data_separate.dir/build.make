# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/wangying/software/CMake-build/bin/cmake

# The command to remove a file.
RM = /home/wangying/software/CMake-build/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wangying/work/mis/analyse_code/cpp/usb_data_separate

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build

# Include any dependencies generated for this target.
include CMakeFiles/usb_data_separate.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/usb_data_separate.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/usb_data_separate.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/usb_data_separate.dir/flags.make

CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o: CMakeFiles/usb_data_separate.dir/flags.make
CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o: /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/usb_data_separate.cc
CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o: CMakeFiles/usb_data_separate.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o"
	/home/wangying/software/gcc-13.2.0-build/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o -MF CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o.d -o CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o -c /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/usb_data_separate.cc

CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.i"
	/home/wangying/software/gcc-13.2.0-build/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/usb_data_separate.cc > CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.i

CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.s"
	/home/wangying/software/gcc-13.2.0-build/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/usb_data_separate.cc -o CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.s

# Object files for target usb_data_separate
usb_data_separate_OBJECTS = \
"CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o"

# External object files for target usb_data_separate
usb_data_separate_EXTERNAL_OBJECTS =

usb_data_separate: CMakeFiles/usb_data_separate.dir/usb_data_separate.cc.o
usb_data_separate: CMakeFiles/usb_data_separate.dir/build.make
usb_data_separate: CMakeFiles/usb_data_separate.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable usb_data_separate"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/usb_data_separate.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/usb_data_separate.dir/build: usb_data_separate
.PHONY : CMakeFiles/usb_data_separate.dir/build

CMakeFiles/usb_data_separate.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/usb_data_separate.dir/cmake_clean.cmake
.PHONY : CMakeFiles/usb_data_separate.dir/clean

CMakeFiles/usb_data_separate.dir/depend:
	cd /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wangying/work/mis/analyse_code/cpp/usb_data_separate /home/wangying/work/mis/analyse_code/cpp/usb_data_separate /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build /home/wangying/work/mis/analyse_code/cpp/usb_data_separate/build/CMakeFiles/usb_data_separate.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/usb_data_separate.dir/depend

