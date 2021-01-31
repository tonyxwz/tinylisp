# Almost all CMake files should start with this You should always specify a
# range with the newest and oldest tested versions of CMake. This will ensure
# you pick up the best policies. cmake_minimum_required(VERSION 3.1...3.19)
cmake_minimum_required(VERSION 3.7...3.19)

# This is your project statement. You should always list languages; Listing the
# version is nice here since it sets lots of useful variables project(
# ModernCMakeExample VERSION 1.0 LANGUAGES CXX)
project(
  lisp
  DESCRIPTION "Learn C by building a lisp (tiny/tony lisp)"
  VERSION 0.1
  LANGUAGES C)

# If you set any CMAKE_ variables, that can go here. (But usually don't do this,
# except maybe for C++ standard) set(CMAKE_C_STANDARD ) set(CMAKE_CXX_STANDARD
# 14) set(CMAKE_CXX_COMPILE_FEATURES cxx_std_11)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Find packages go here.
find_package(Catch2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_path(STB_INCLUDE_DIRS "stb.h")

# You should usually split this into folders, but this is a simple example

# This is a "default" library, and will match the *** variable setting. Other
# common choices are STATIC, SHARED, and MODULE Including header files here
# helps IDEs but is not required. Output libname matches target name, with the
# usual extensions on your system


# Link each target with other targets or add options, etc. TODO

# Adding something we can run - Output name matches target name

add_executable(repl ch4-repl.c)

# Make sure you link your targets with this command. It can also link libraries
# and even flags, so linking a target that does not exist will not give a
# configure-time error.

target_link_libraries(repl PRIVATE GTest::gmock GTest::gtest GTest::gmock_main
                                   GTest::gtest_main)
