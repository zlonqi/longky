# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/clion-2019.3.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2019.3.3/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/longky/longky

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/longky/longky/cmake-build-debug

# Include any dependencies generated for this target.
include webServer/CMakeFiles/webServer.dir/depend.make

# Include the progress variables for this target.
include webServer/CMakeFiles/webServer.dir/progress.make

# Include the compile flags for this target's objects.
include webServer/CMakeFiles/webServer.dir/flags.make

webServer/CMakeFiles/webServer.dir/staticServer.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/staticServer.cpp.o: ../webServer/staticServer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object webServer/CMakeFiles/webServer.dir/staticServer.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/staticServer.cpp.o -c /root/longky/longky/webServer/staticServer.cpp

webServer/CMakeFiles/webServer.dir/staticServer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/staticServer.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/staticServer.cpp > CMakeFiles/webServer.dir/staticServer.cpp.i

webServer/CMakeFiles/webServer.dir/staticServer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/staticServer.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/staticServer.cpp -o CMakeFiles/webServer.dir/staticServer.cpp.s

webServer/CMakeFiles/webServer.dir/requestParser.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/requestParser.cpp.o: ../webServer/requestParser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object webServer/CMakeFiles/webServer.dir/requestParser.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/requestParser.cpp.o -c /root/longky/longky/webServer/requestParser.cpp

webServer/CMakeFiles/webServer.dir/requestParser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/requestParser.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/requestParser.cpp > CMakeFiles/webServer.dir/requestParser.cpp.i

webServer/CMakeFiles/webServer.dir/requestParser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/requestParser.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/requestParser.cpp -o CMakeFiles/webServer.dir/requestParser.cpp.s

webServer/CMakeFiles/webServer.dir/start_up.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/start_up.cpp.o: ../webServer/start_up.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object webServer/CMakeFiles/webServer.dir/start_up.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/start_up.cpp.o -c /root/longky/longky/webServer/start_up.cpp

webServer/CMakeFiles/webServer.dir/start_up.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/start_up.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/start_up.cpp > CMakeFiles/webServer.dir/start_up.cpp.i

webServer/CMakeFiles/webServer.dir/start_up.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/start_up.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/start_up.cpp -o CMakeFiles/webServer.dir/start_up.cpp.s

webServer/CMakeFiles/webServer.dir/ZLibString.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/ZLibString.cpp.o: ../webServer/ZLibString.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object webServer/CMakeFiles/webServer.dir/ZLibString.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/ZLibString.cpp.o -c /root/longky/longky/webServer/ZLibString.cpp

webServer/CMakeFiles/webServer.dir/ZLibString.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/ZLibString.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/ZLibString.cpp > CMakeFiles/webServer.dir/ZLibString.cpp.i

webServer/CMakeFiles/webServer.dir/ZLibString.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/ZLibString.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/ZLibString.cpp -o CMakeFiles/webServer.dir/ZLibString.cpp.s

webServer/CMakeFiles/webServer.dir/configLoad.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/configLoad.cpp.o: ../webServer/configLoad.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object webServer/CMakeFiles/webServer.dir/configLoad.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/configLoad.cpp.o -c /root/longky/longky/webServer/configLoad.cpp

webServer/CMakeFiles/webServer.dir/configLoad.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/configLoad.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/configLoad.cpp > CMakeFiles/webServer.dir/configLoad.cpp.i

webServer/CMakeFiles/webServer.dir/configLoad.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/configLoad.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/configLoad.cpp -o CMakeFiles/webServer.dir/configLoad.cpp.s

webServer/CMakeFiles/webServer.dir/redis.cpp.o: webServer/CMakeFiles/webServer.dir/flags.make
webServer/CMakeFiles/webServer.dir/redis.cpp.o: ../webServer/redis.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object webServer/CMakeFiles/webServer.dir/redis.cpp.o"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webServer.dir/redis.cpp.o -c /root/longky/longky/webServer/redis.cpp

webServer/CMakeFiles/webServer.dir/redis.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webServer.dir/redis.cpp.i"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/longky/longky/webServer/redis.cpp > CMakeFiles/webServer.dir/redis.cpp.i

webServer/CMakeFiles/webServer.dir/redis.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webServer.dir/redis.cpp.s"
	cd /root/longky/longky/cmake-build-debug/webServer && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/longky/longky/webServer/redis.cpp -o CMakeFiles/webServer.dir/redis.cpp.s

# Object files for target webServer
webServer_OBJECTS = \
"CMakeFiles/webServer.dir/staticServer.cpp.o" \
"CMakeFiles/webServer.dir/requestParser.cpp.o" \
"CMakeFiles/webServer.dir/start_up.cpp.o" \
"CMakeFiles/webServer.dir/ZLibString.cpp.o" \
"CMakeFiles/webServer.dir/configLoad.cpp.o" \
"CMakeFiles/webServer.dir/redis.cpp.o"

# External object files for target webServer
webServer_EXTERNAL_OBJECTS =

../bin/webServer: webServer/CMakeFiles/webServer.dir/staticServer.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/requestParser.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/start_up.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/ZLibString.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/configLoad.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/redis.cpp.o
../bin/webServer: webServer/CMakeFiles/webServer.dir/build.make
../bin/webServer: ../lib/libbase.a
../bin/webServer: ../lib/libnet.a
../bin/webServer: /usr/lib/libyaml-cpp.so
../bin/webServer: webServer/CMakeFiles/webServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/longky/longky/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable ../../bin/webServer"
	cd /root/longky/longky/cmake-build-debug/webServer && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/webServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
webServer/CMakeFiles/webServer.dir/build: ../bin/webServer

.PHONY : webServer/CMakeFiles/webServer.dir/build

webServer/CMakeFiles/webServer.dir/clean:
	cd /root/longky/longky/cmake-build-debug/webServer && $(CMAKE_COMMAND) -P CMakeFiles/webServer.dir/cmake_clean.cmake
.PHONY : webServer/CMakeFiles/webServer.dir/clean

webServer/CMakeFiles/webServer.dir/depend:
	cd /root/longky/longky/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/longky/longky /root/longky/longky/webServer /root/longky/longky/cmake-build-debug /root/longky/longky/cmake-build-debug/webServer /root/longky/longky/cmake-build-debug/webServer/CMakeFiles/webServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : webServer/CMakeFiles/webServer.dir/depend

