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
include CMakeFiles/config_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/config_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/config_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/config_test.dir/flags.make

CMakeFiles/config_test.dir/tests/config_test.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/tests/config_test.cc.o: /home/hh/HH/tests/config_test.cc
CMakeFiles/config_test.dir/tests/config_test.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/config_test.dir/tests/config_test.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/config_test.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/tests/config_test.cc.o -MF CMakeFiles/config_test.dir/tests/config_test.cc.o.d -o CMakeFiles/config_test.dir/tests/config_test.cc.o -c /home/hh/HH/tests/config_test.cc

CMakeFiles/config_test.dir/tests/config_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/tests/config_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/config_test.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/tests/config_test.cc > CMakeFiles/config_test.dir/tests/config_test.cc.i

CMakeFiles/config_test.dir/tests/config_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/tests/config_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"tests/config_test.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/tests/config_test.cc -o CMakeFiles/config_test.dir/tests/config_test.cc.s

CMakeFiles/config_test.dir/hh/config.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/config.cc.o: /home/hh/HH/hh/config.cc
CMakeFiles/config_test.dir/hh/config.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/config_test.dir/hh/config.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/config.cc.o -MF CMakeFiles/config_test.dir/hh/config.cc.o.d -o CMakeFiles/config_test.dir/hh/config.cc.o -c /home/hh/HH/hh/config.cc

CMakeFiles/config_test.dir/hh/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/config.cc > CMakeFiles/config_test.dir/hh/config.cc.i

CMakeFiles/config_test.dir/hh/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/config.cc -o CMakeFiles/config_test.dir/hh/config.cc.s

CMakeFiles/config_test.dir/hh/fiber.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/fiber.cc.o: /home/hh/HH/hh/fiber.cc
CMakeFiles/config_test.dir/hh/fiber.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/config_test.dir/hh/fiber.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/fiber.cc.o -MF CMakeFiles/config_test.dir/hh/fiber.cc.o.d -o CMakeFiles/config_test.dir/hh/fiber.cc.o -c /home/hh/HH/hh/fiber.cc

CMakeFiles/config_test.dir/hh/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/fiber.cc > CMakeFiles/config_test.dir/hh/fiber.cc.i

CMakeFiles/config_test.dir/hh/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/fiber.cc -o CMakeFiles/config_test.dir/hh/fiber.cc.s

CMakeFiles/config_test.dir/hh/log.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/log.cc.o: /home/hh/HH/hh/log.cc
CMakeFiles/config_test.dir/hh/log.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/config_test.dir/hh/log.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/log.cc.o -MF CMakeFiles/config_test.dir/hh/log.cc.o.d -o CMakeFiles/config_test.dir/hh/log.cc.o -c /home/hh/HH/hh/log.cc

CMakeFiles/config_test.dir/hh/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/log.cc > CMakeFiles/config_test.dir/hh/log.cc.i

CMakeFiles/config_test.dir/hh/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/log.cc -o CMakeFiles/config_test.dir/hh/log.cc.s

CMakeFiles/config_test.dir/hh/scheduler.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/scheduler.cc.o: /home/hh/HH/hh/scheduler.cc
CMakeFiles/config_test.dir/hh/scheduler.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/config_test.dir/hh/scheduler.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/scheduler.cc.o -MF CMakeFiles/config_test.dir/hh/scheduler.cc.o.d -o CMakeFiles/config_test.dir/hh/scheduler.cc.o -c /home/hh/HH/hh/scheduler.cc

CMakeFiles/config_test.dir/hh/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/scheduler.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/scheduler.cc > CMakeFiles/config_test.dir/hh/scheduler.cc.i

CMakeFiles/config_test.dir/hh/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/scheduler.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/scheduler.cc -o CMakeFiles/config_test.dir/hh/scheduler.cc.s

CMakeFiles/config_test.dir/hh/thread.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/thread.cc.o: /home/hh/HH/hh/thread.cc
CMakeFiles/config_test.dir/hh/thread.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/config_test.dir/hh/thread.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/thread.cc.o -MF CMakeFiles/config_test.dir/hh/thread.cc.o.d -o CMakeFiles/config_test.dir/hh/thread.cc.o -c /home/hh/HH/hh/thread.cc

CMakeFiles/config_test.dir/hh/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/thread.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/thread.cc > CMakeFiles/config_test.dir/hh/thread.cc.i

CMakeFiles/config_test.dir/hh/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/thread.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/thread.cc -o CMakeFiles/config_test.dir/hh/thread.cc.s

CMakeFiles/config_test.dir/hh/util.cc.o: CMakeFiles/config_test.dir/flags.make
CMakeFiles/config_test.dir/hh/util.cc.o: /home/hh/HH/hh/util.cc
CMakeFiles/config_test.dir/hh/util.cc.o: CMakeFiles/config_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/config_test.dir/hh/util.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/config_test.dir/hh/util.cc.o -MF CMakeFiles/config_test.dir/hh/util.cc.o.d -o CMakeFiles/config_test.dir/hh/util.cc.o -c /home/hh/HH/hh/util.cc

CMakeFiles/config_test.dir/hh/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/config_test.dir/hh/util.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/util.cc > CMakeFiles/config_test.dir/hh/util.cc.i

CMakeFiles/config_test.dir/hh/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/config_test.dir/hh/util.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"hh/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/util.cc -o CMakeFiles/config_test.dir/hh/util.cc.s

# Object files for target config_test
config_test_OBJECTS = \
"CMakeFiles/config_test.dir/tests/config_test.cc.o" \
"CMakeFiles/config_test.dir/hh/config.cc.o" \
"CMakeFiles/config_test.dir/hh/fiber.cc.o" \
"CMakeFiles/config_test.dir/hh/log.cc.o" \
"CMakeFiles/config_test.dir/hh/scheduler.cc.o" \
"CMakeFiles/config_test.dir/hh/thread.cc.o" \
"CMakeFiles/config_test.dir/hh/util.cc.o"

# External object files for target config_test
config_test_EXTERNAL_OBJECTS =

bin/config_test: CMakeFiles/config_test.dir/tests/config_test.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/config.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/fiber.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/log.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/scheduler.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/thread.cc.o
bin/config_test: CMakeFiles/config_test.dir/hh/util.cc.o
bin/config_test: CMakeFiles/config_test.dir/build.make
bin/config_test: /usr/local/lib64/libyaml-cpp.a
bin/config_test: /usr/lib64/libpthread.a
bin/config_test: CMakeFiles/config_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable bin/config_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/config_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/config_test.dir/build: bin/config_test
.PHONY : CMakeFiles/config_test.dir/build

CMakeFiles/config_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/config_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/config_test.dir/clean

CMakeFiles/config_test.dir/depend:
	cd /home/hh/HH/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hh/HH /home/hh/HH /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug/CMakeFiles/config_test.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/config_test.dir/depend

