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

include(FetchContent)

# In our structure, we separate private and public files like so:
#
# * public files are any header files that are exposed to the user of the
#   library that compose the interface to the library.
#
# * private files are any files that remain internal to the library, regardless
#   of whether they are source or header files.
#
# This level of separation allows us to think in terms of "interface" vs
# "implementation".
set(PRIVATE_SRCS private/disasm.cpp private/impl_interpreter.cpp)
set(PRIVATE_HDRS private/impl_interpreter.h)

set(PUBLIC_HDRS public/core/disasm.h
                public/core/impl.h
                public/core/spec.h)

# I am not sure if this is the result of my own ignorance, or if CMake truly
# does not offer this functionality:
#
# `find_package()` does not have any way to notify us if an error was
# encountered, especially if a `FindXXX.cmake` script was not found. This is a
# problem, because it would be nice to be able to download, compile, and use
# any dependencies that aren't present via the FetchContent method if at all
# possible.
#
# Well, it is possible. We're just not going to use `find_package()` for
# certain dependencies and download them regardless of their presence
# (or lack thereof) on the system. libfmt is one of them.
#
# If you can point me in the right direction to avoid this, I'd be grateful.
# I'm aware it's not ideal, but so far it sure as hell beats the alternatives.
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        8.0.1
)
FetchContent_MakeAvailable(fmt)

# We're only going to support compiling the core as a static library for now;
# there's no reason to make it shared as only one other component of this
# project is going to use it: the frontend.
add_library(core STATIC ${PRIVATE_SRCS} ${PRIVATE_HDRS} ${PUBLIC_HDRS})

# Make sure we can use libfmt in our core!
target_link_libraries(core fmt::fmt)

# We want to be able to access the public files within the private ones as
# there's probably information we need to use from them.
target_include_directories(core PRIVATE public)

# This function configures the target to various specifications; refer to its
# body for more details.
vmtutorial_configure_target(core)