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
CMAKE_SOURCE_DIR = /home/ubuntu/reverse_final_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/reverse_final_project/build

# Include any dependencies generated for this target.
include CMakeFiles/STMcontroler.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/STMcontroler.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/STMcontroler.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/STMcontroler.dir/flags.make

CMakeFiles/STMcontroler.dir/main.c.o: CMakeFiles/STMcontroler.dir/flags.make
CMakeFiles/STMcontroler.dir/main.c.o: /home/ubuntu/reverse_final_project/main.c
CMakeFiles/STMcontroler.dir/main.c.o: CMakeFiles/STMcontroler.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ubuntu/reverse_final_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/STMcontroler.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/STMcontroler.dir/main.c.o -MF CMakeFiles/STMcontroler.dir/main.c.o.d -o CMakeFiles/STMcontroler.dir/main.c.o -c /home/ubuntu/reverse_final_project/main.c

CMakeFiles/STMcontroler.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/STMcontroler.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/ubuntu/reverse_final_project/main.c > CMakeFiles/STMcontroler.dir/main.c.i

CMakeFiles/STMcontroler.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/STMcontroler.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/ubuntu/reverse_final_project/main.c -o CMakeFiles/STMcontroler.dir/main.c.s

# Object files for target STMcontroler
STMcontroler_OBJECTS = \
"CMakeFiles/STMcontroler.dir/main.c.o"

# External object files for target STMcontroler
STMcontroler_EXTERNAL_OBJECTS =

STMcontroler: CMakeFiles/STMcontroler.dir/main.c.o
STMcontroler: CMakeFiles/STMcontroler.dir/build.make
STMcontroler: CMakeFiles/STMcontroler.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/ubuntu/reverse_final_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable STMcontroler"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/STMcontroler.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/STMcontroler.dir/build: STMcontroler
.PHONY : CMakeFiles/STMcontroler.dir/build

CMakeFiles/STMcontroler.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/STMcontroler.dir/cmake_clean.cmake
.PHONY : CMakeFiles/STMcontroler.dir/clean

CMakeFiles/STMcontroler.dir/depend:
	cd /home/ubuntu/reverse_final_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/reverse_final_project /home/ubuntu/reverse_final_project /home/ubuntu/reverse_final_project/build /home/ubuntu/reverse_final_project/build /home/ubuntu/reverse_final_project/build/CMakeFiles/STMcontroler.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/STMcontroler.dir/depend

