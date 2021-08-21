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

# XXX: For some very strange reason on Windows, building googletest through
# FetchContent causes a failure when using clang-cl. Must investigate further;
# this is temporary.
find_package(GTest REQUIRED)

# We should have a function to avoid duplicating the code for every test that
# we're going to register.
function(register_vmtutorial_core_test TEST_NAME SRCS)
  add_executable(${TEST_NAME} ${SRCS})
  target_link_libraries(${TEST_NAME} PRIVATE core GTest::gtest GTest::gtest_main)
  target_include_directories(${TEST_NAME} PRIVATE ../src/public)

  vmtutorial_configure_target(${TEST_NAME})
  gtest_discover_tests(${TEST_NAME})
endfunction()

register_vmtutorial_core_test(core_disasm_test disasm.cpp)