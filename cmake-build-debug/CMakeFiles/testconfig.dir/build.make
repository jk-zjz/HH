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

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hh/HH

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hh/HH/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/testconfig.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/testconfig.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/testconfig.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testconfig.dir/flags.make

CMakeFiles/testconfig.dir/tests/test_config.cpp.o: CMakeFiles/testconfig.dir/flags.make
CMakeFiles/testconfig.dir/tests/test_config.cpp.o: /home/hh/HH/tests/test_config.cpp
CMakeFiles/testconfig.dir/tests/test_config.cpp.o: CMakeFiles/testconfig.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testconfig.dir/tests/test_config.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testconfig.dir/tests/test_config.cpp.o -MF CMakeFiles/testconfig.dir/tests/test_config.cpp.o.d -o CMakeFiles/testconfig.dir/tests/test_config.cpp.o -c /home/hh/HH/tests/test_config.cpp

CMakeFiles/testconfig.dir/tests/test_config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testconfig.dir/tests/test_config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/tests/test_config.cpp > CMakeFiles/testconfig.dir/tests/test_config.cpp.i

CMakeFiles/testconfig.dir/tests/test_config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testconfig.dir/tests/test_config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/tests/test_config.cpp -o CMakeFiles/testconfig.dir/tests/test_config.cpp.s

CMakeFiles/testconfig.dir/hh/config.cc.o: CMakeFiles/testconfig.dir/flags.make
CMakeFiles/testconfig.dir/hh/config.cc.o: /home/hh/HH/hh/config.cc
CMakeFiles/testconfig.dir/hh/config.cc.o: CMakeFiles/testconfig.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/testconfig.dir/hh/config.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testconfig.dir/hh/config.cc.o -MF CMakeFiles/testconfig.dir/hh/config.cc.o.d -o CMakeFiles/testconfig.dir/hh/config.cc.o -c /home/hh/HH/hh/config.cc

CMakeFiles/testconfig.dir/hh/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testconfig.dir/hh/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/config.cc > CMakeFiles/testconfig.dir/hh/config.cc.i

CMakeFiles/testconfig.dir/hh/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testconfig.dir/hh/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/config.cc -o CMakeFiles/testconfig.dir/hh/config.cc.s

CMakeFiles/testconfig.dir/hh/log.cc.o: CMakeFiles/testconfig.dir/flags.make
CMakeFiles/testconfig.dir/hh/log.cc.o: /home/hh/HH/hh/log.cc
CMakeFiles/testconfig.dir/hh/log.cc.o: CMakeFiles/testconfig.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/testconfig.dir/hh/log.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testconfig.dir/hh/log.cc.o -MF CMakeFiles/testconfig.dir/hh/log.cc.o.d -o CMakeFiles/testconfig.dir/hh/log.cc.o -c /home/hh/HH/hh/log.cc

CMakeFiles/testconfig.dir/hh/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testconfig.dir/hh/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/log.cc > CMakeFiles/testconfig.dir/hh/log.cc.i

CMakeFiles/testconfig.dir/hh/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testconfig.dir/hh/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/log.cc -o CMakeFiles/testconfig.dir/hh/log.cc.s

CMakeFiles/testconfig.dir/hh/util.cc.o: CMakeFiles/testconfig.dir/flags.make
CMakeFiles/testconfig.dir/hh/util.cc.o: /home/hh/HH/hh/util.cc
CMakeFiles/testconfig.dir/hh/util.cc.o: CMakeFiles/testconfig.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/testconfig.dir/hh/util.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testconfig.dir/hh/util.cc.o -MF CMakeFiles/testconfig.dir/hh/util.cc.o.d -o CMakeFiles/testconfig.dir/hh/util.cc.o -c /home/hh/HH/hh/util.cc

CMakeFiles/testconfig.dir/hh/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testconfig.dir/hh/util.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/util.cc > CMakeFiles/testconfig.dir/hh/util.cc.i

CMakeFiles/testconfig.dir/hh/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testconfig.dir/hh/util.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/util.cc -o CMakeFiles/testconfig.dir/hh/util.cc.s

# Object files for target testconfig
testconfig_OBJECTS = \
"CMakeFiles/testconfig.dir/tests/test_config.cpp.o" \
"CMakeFiles/testconfig.dir/hh/config.cc.o" \
"CMakeFiles/testconfig.dir/hh/log.cc.o" \
"CMakeFiles/testconfig.dir/hh/util.cc.o"

# External object files for target testconfig
testconfig_EXTERNAL_OBJECTS =

bin/testconfig: CMakeFiles/testconfig.dir/tests/test_config.cpp.o
bin/testconfig: CMakeFiles/testconfig.dir/hh/config.cc.o
bin/testconfig: CMakeFiles/testconfig.dir/hh/log.cc.o
bin/testconfig: CMakeFiles/testconfig.dir/hh/util.cc.o
bin/testconfig: CMakeFiles/testconfig.dir/build.make
bin/testconfig: CMakeFiles/testconfig.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable bin/testconfig"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testconfig.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testconfig.dir/build: bin/testconfig
.PHONY : CMakeFiles/testconfig.dir/build

CMakeFiles/testconfig.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testconfig.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testconfig.dir/clean

CMakeFiles/testconfig.dir/depend:
	cd /home/hh/HH/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hh/HH /home/hh/HH /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug/CMakeFiles/testconfig.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/testconfig.dir/depend

