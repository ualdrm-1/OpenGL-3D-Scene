# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ualdrm/Studies/KR

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ualdrm/Studies/KR/build

# Include any dependencies generated for this target.
include CMakeFiles/kr.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/kr.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/kr.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/kr.dir/flags.make

CMakeFiles/kr.dir/main.cpp.o: CMakeFiles/kr.dir/flags.make
CMakeFiles/kr.dir/main.cpp.o: /home/ualdrm/Studies/KR/main.cpp
CMakeFiles/kr.dir/main.cpp.o: CMakeFiles/kr.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ualdrm/Studies/KR/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/kr.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/kr.dir/main.cpp.o -MF CMakeFiles/kr.dir/main.cpp.o.d -o CMakeFiles/kr.dir/main.cpp.o -c /home/ualdrm/Studies/KR/main.cpp

CMakeFiles/kr.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/kr.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ualdrm/Studies/KR/main.cpp > CMakeFiles/kr.dir/main.cpp.i

CMakeFiles/kr.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/kr.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ualdrm/Studies/KR/main.cpp -o CMakeFiles/kr.dir/main.cpp.s

# Object files for target kr
kr_OBJECTS = \
"CMakeFiles/kr.dir/main.cpp.o"

# External object files for target kr
kr_EXTERNAL_OBJECTS =

kr: CMakeFiles/kr.dir/main.cpp.o
kr: CMakeFiles/kr.dir/build.make
kr: /usr/lib/x86_64-linux-gnu/libGLEW.so
kr: /usr/lib/x86_64-linux-gnu/libglfw.so.3.3
kr: /usr/lib/x86_64-linux-gnu/libGLX.so
kr: /usr/lib/x86_64-linux-gnu/libOpenGL.so
kr: CMakeFiles/kr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/ualdrm/Studies/KR/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable kr"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/kr.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/kr.dir/build: kr
.PHONY : CMakeFiles/kr.dir/build

CMakeFiles/kr.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/kr.dir/cmake_clean.cmake
.PHONY : CMakeFiles/kr.dir/clean

CMakeFiles/kr.dir/depend:
	cd /home/ualdrm/Studies/KR/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ualdrm/Studies/KR /home/ualdrm/Studies/KR /home/ualdrm/Studies/KR/build /home/ualdrm/Studies/KR/build /home/ualdrm/Studies/KR/build/CMakeFiles/kr.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/kr.dir/depend

