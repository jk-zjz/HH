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
include CMakeFiles/httpclient_parser.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/httpclient_parser.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/httpclient_parser.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/httpclient_parser.dir/flags.make

/home/hh/HH/hh/http/httpclient_parser.cpp: /home/hh/HH/hh/http/httpclient_parser.rl
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --blue --bold --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating /home/hh/HH/hh/http/httpclient_parser.cpp"
	cd /home/hh/HH && ragel -G2 -o/home/hh/HH/hh/http hh/http/httpclient_parser.rl

CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o: CMakeFiles/httpclient_parser.dir/flags.make
CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o: /home/hh/HH/hh/http/httpclient_parser.cpp
CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o: CMakeFiles/httpclient_parser.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o -MF CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o.d -o CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o -c /home/hh/HH/hh/http/httpclient_parser.cpp

CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hh/HH/hh/http/httpclient_parser.cpp > CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.i

CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hh/HH/hh/http/httpclient_parser.cpp -o CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.s

# Object files for target httpclient_parser
httpclient_parser_OBJECTS = \
"CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o"

# External object files for target httpclient_parser
httpclient_parser_EXTERNAL_OBJECTS =

libhttpclient_parser.a: CMakeFiles/httpclient_parser.dir/hh/http/httpclient_parser.cpp.o
libhttpclient_parser.a: CMakeFiles/httpclient_parser.dir/build.make
libhttpclient_parser.a: CMakeFiles/httpclient_parser.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/hh/HH/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libhttpclient_parser.a"
	$(CMAKE_COMMAND) -P CMakeFiles/httpclient_parser.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/httpclient_parser.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/httpclient_parser.dir/build: libhttpclient_parser.a
.PHONY : CMakeFiles/httpclient_parser.dir/build

CMakeFiles/httpclient_parser.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/httpclient_parser.dir/cmake_clean.cmake
.PHONY : CMakeFiles/httpclient_parser.dir/clean

CMakeFiles/httpclient_parser.dir/depend: /home/hh/HH/hh/http/httpclient_parser.cpp
	cd /home/hh/HH/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hh/HH /home/hh/HH /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug /home/hh/HH/cmake-build-debug/CMakeFiles/httpclient_parser.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/httpclient_parser.dir/depend

