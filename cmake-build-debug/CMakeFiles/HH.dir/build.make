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
CMAKE_COMMAND = /home/hh/cmake-3.29.2/bin/cmake

# The command to remove a file.
RM = /home/hh/cmake-3.29.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.ZVwVQZO1im

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.ZVwVQZO1im/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/HH.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/HH.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/HH.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/HH.dir/flags.make

CMakeFiles/HH.dir/main.cpp.o: CMakeFiles/HH.dir/flags.make
CMakeFiles/HH.dir/main.cpp.o: /tmp/tmp.ZVwVQZO1im/main.cpp
CMakeFiles/HH.dir/main.cpp.o: CMakeFiles/HH.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/tmp/tmp.ZVwVQZO1im/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/HH.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/HH.dir/main.cpp.o -MF CMakeFiles/HH.dir/main.cpp.o.d -o CMakeFiles/HH.dir/main.cpp.o -c /tmp/tmp.ZVwVQZO1im/main.cpp

CMakeFiles/HH.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/HH.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.ZVwVQZO1im/main.cpp > CMakeFiles/HH.dir/main.cpp.i

CMakeFiles/HH.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/HH.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.ZVwVQZO1im/main.cpp -o CMakeFiles/HH.dir/main.cpp.s

# Object files for target HH
HH_OBJECTS = \
"CMakeFiles/HH.dir/main.cpp.o"

# External object files for target HH
HH_EXTERNAL_OBJECTS =

HH: CMakeFiles/HH.dir/main.cpp.o
HH: CMakeFiles/HH.dir/build.make
HH: CMakeFiles/HH.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/tmp/tmp.ZVwVQZO1im/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable HH"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/HH.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/HH.dir/build: HH
.PHONY : CMakeFiles/HH.dir/build

CMakeFiles/HH.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/HH.dir/cmake_clean.cmake
.PHONY : CMakeFiles/HH.dir/clean

CMakeFiles/HH.dir/depend:
	cd /tmp/tmp.ZVwVQZO1im/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.ZVwVQZO1im /tmp/tmp.ZVwVQZO1im /tmp/tmp.ZVwVQZO1im/cmake-build-debug /tmp/tmp.ZVwVQZO1im/cmake-build-debug /tmp/tmp.ZVwVQZO1im/cmake-build-debug/CMakeFiles/HH.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/HH.dir/depend
