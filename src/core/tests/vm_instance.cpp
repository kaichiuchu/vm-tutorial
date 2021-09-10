// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by kaichiuchu <kaichiuchu@protonmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#include <core/vm_instance.h>

#include "gtest/gtest.h"

namespace {
TEST(VMInstance, LoadsProgramSuccessfully) {
  chip8::VMInstance chip8_vm;

  // Now let's create "program data" with some amount of data.
  constexpr std::array<uint_fast8_t, 10> program_data{
      0x6B, 0x61, 0x69, 0x63, 0x68, 0x69, 0x75, 0x63, 0x68, 0x75};

  // The program code should be loaded, as it is well within the maximum size of
  // a program.
  ASSERT_TRUE(chip8_vm.LoadProgram(program_data));

  // Now verify that the program code made it into the implementation.
  ASSERT_TRUE(std::equal(
      chip8_vm.impl_->memory_.cbegin() + chip8::memory_region::kProgramArea,
      chip8_vm.impl_->memory_.cbegin() + chip8::memory_region::kProgramArea +
          program_data.size(),
      program_data.cbegin()));
}

TEST(VMInstance, RejectsLargeProgram) {
  chip8::VMInstance chip8_vm;

  // Let's pretend we have a large number of elements.
  std::vector<uint_fast8_t> program_data;
  constexpr auto kFakeElementAmount = 4000;
  program_data.resize(kFakeElementAmount);

  // The program code should be loaded, as it is well within the maximum size of
  // a program.
  ASSERT_FALSE(chip8_vm.LoadProgram(program_data));
}
}  // namespace