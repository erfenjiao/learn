# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug

# Include any dependencies generated for this target.
include muduo/base/tests/CMakeFiles/atomic_unittest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include muduo/base/tests/CMakeFiles/atomic_unittest.dir/compiler_depend.make

# Include the progress variables for this target.
include muduo/base/tests/CMakeFiles/atomic_unittest.dir/progress.make

# Include the compile flags for this target's objects.
include muduo/base/tests/CMakeFiles/atomic_unittest.dir/flags.make

muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o: muduo/base/tests/CMakeFiles/atomic_unittest.dir/flags.make
muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o: /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo/muduo/base/tests/Atomic_unittest.cc
muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o: muduo/base/tests/CMakeFiles/atomic_unittest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o"
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o -MF CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o.d -o CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o -c /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo/muduo/base/tests/Atomic_unittest.cc

muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.i"
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo/muduo/base/tests/Atomic_unittest.cc > CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.i

muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.s"
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests && g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo/muduo/base/tests/Atomic_unittest.cc -o CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.s

# Object files for target atomic_unittest
atomic_unittest_OBJECTS = \
"CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o"

# External object files for target atomic_unittest
atomic_unittest_EXTERNAL_OBJECTS =

bin/atomic_unittest: muduo/base/tests/CMakeFiles/atomic_unittest.dir/Atomic_unittest.cc.o
bin/atomic_unittest: muduo/base/tests/CMakeFiles/atomic_unittest.dir/build.make
bin/atomic_unittest: muduo/base/tests/CMakeFiles/atomic_unittest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../bin/atomic_unittest"
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/atomic_unittest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
muduo/base/tests/CMakeFiles/atomic_unittest.dir/build: bin/atomic_unittest
.PHONY : muduo/base/tests/CMakeFiles/atomic_unittest.dir/build

muduo/base/tests/CMakeFiles/atomic_unittest.dir/clean:
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests && $(CMAKE_COMMAND) -P CMakeFiles/atomic_unittest.dir/cmake_clean.cmake
.PHONY : muduo/base/tests/CMakeFiles/atomic_unittest.dir/clean

muduo/base/tests/CMakeFiles/atomic_unittest.dir/depend:
	cd /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo /home/erfenjiao/learn/大并发服务器架构/代码/12/jmuduo/muduo/base/tests /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests /home/erfenjiao/learn/大并发服务器架构/代码/12/build/debug/muduo/base/tests/CMakeFiles/atomic_unittest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : muduo/base/tests/CMakeFiles/atomic_unittest.dir/depend

