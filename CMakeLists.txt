# vm-tutorial - Virtual machine tutorial targeting CHIP-8
#
# Written in 2021 by kaichiuchu <kaichiuchu@protonmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.

# Build system ideas are heavily influenced by
# https://cliutils.gitlab.io/modern-cmake/.

cmake_minimum_required(VERSION 3.7...3.21)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

include(cmake/configure_target.cmake)

project(vm-tutorial VERSION 1.0.0.0
                    DESCRIPTION "Virtual machine tutorial targeting CHIP-8"
                    HOMEPAGE_URL "https://github.com/kaichiuchu/vm-tutorial"
                    LANGUAGES CXX)

# This tutorial project only uses the clang and gcc compilers.
#
# Specific compiler options are used in the event the compiler is either one of
# those.
#
# It is not an error if the compiler is not either one of those two, however it is
# prudent to warn the user that unintended consequences may arise, such as poor
# code generation or compiler errors. In such a case, help will not be offered.
#
# gcc and clang were chosen due to their maturity and ubiquity among a variety
# of different systems, and their support for a number of architectures.
#
# Because the tutorial project uses C++17, recommending the first version of
# each compiler to implement reasonably complete C++17 support is wise.

# While CMake >=3.0 has line continuation of quoted arguments, it does not
# respect indentation and as such it is entirely worthless to use. We *could*
# use it, however to do so would look absolutely nasty.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (CMAKE_CXX_COMPILER_VERSION LESS "8.1")
    string(CONCAT WARNING_MSG "You are compiling this tutorial project using an "
                              "unsupported g++ version. It is recommended that "
                              "you use at least g++ >=8.1 to compile this "
                              "project.")
    message(WARNING ${WARNING_MSG})
  endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if (CMAKE_CXX_COMPILER_VERSION LESS "5.0")
    string(CONCAT WARNING_MSG "You are compiling this tutorial project using "
                              "an unsupported clang++ version. It is "
                              "recommended that you use at least "
                              "clang++ >=5.0 to compile this project.")
    message(WARNING ${WARNING_MSG})
  endif()
else()
  string(CONCAT WARNING_MSG "You are compiling this tutorial project using an "
                            "unsupported compiler. It is recommended that you "
                            "use either g++ >=8.1 or clang++ >=5.0 to compile "
                            "this project.")
  message(WARNING ${WARNING_MSG})
endif()

# Now we'll go ahead and move to the source directory.
add_subdirectory(src)