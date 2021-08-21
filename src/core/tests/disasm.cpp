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

#include <core/disasm.h>

#include "gtest/gtest.h"

namespace {
using InstructionValue = uint_fast16_t;
using DisassemblerResult = std::string_view;

using TestDataInfo = std::pair<InstructionValue, DisassemblerResult>;

class DisassemblerTest : public testing::TestWithParam<TestDataInfo> {};

// We're not going to test for every single possible instruction operand, just a
// value that emits the instruction with _some_ operand.
//
// If 0x3456 ("SE V4, $56") returns the correct result, but 0x3556 ("SE V5,
// $56") returns an invalid result, this is likely an issue with the formatting
// library being used, which is outside the scope of the test.
//
// There is one hair in the soup, unfortunately: Some future implementation may
// choose to have an array to handle the V registers (which may contain an
// invalid entry), but our current implementation does not do this, and for a
// relatively minor performance benefit I don't see the point, and such an
// implementation probably wouldn't be merged anyway.
//
// Work on optimizing things that actually matter.
const std::vector<TestDataInfo> test_data{
    {0x00E0, "CLS"},           {0x00EE, "RET"},
    {0x1123, "JP $123"},       {0x2123, "CALL $123"},
    {0x3456, "SE V4, $56"},    {0x4567, "SNE V5, $67"},
    {0x5678, "SE V6, V7"},     {0x6244, "LD V2, $44"},
    {0x7123, "ADD V1, $23"},   {0x8121, "OR V1, V2"},
    {0x8122, "AND V1, V2"},    {0x8123, "XOR V1, V2"},
    {0x8124, "ADD V1, V2"},    {0x8125, "SUB V1, V2"},
    {0x8126, "SHR V1"},        {0x8127, "SUBN V1, V2"},
    {0x812E, "SHL V1"},        {0x9120, "SNE V1, V2"},
    {0xA123, "LD I, $123"},    {0xBEE6, "JP V0, $EE6"},
    {0xC123, "RND V1, $23"},   {0xD123, "DRW V1, V2, 3"},
    {0xE19E, "SKP V1"},        {0xE1A1, "SKNP V1"},
    {0xF107, "LD V1, DT"},     {0xF10A, "LD V1, K"},
    {0xF115, "LD DT, V1"},     {0xF118, "LD ST, V1"},
    {0xF11E, "ADD I, V1"},     {0xF129, "LD F, V1"},
    {0xF133, "LD B, V1"},      {0xF155, "LD [I], V1"},
    {0xF165, "LD V1, [I]"},    {0x00EF, "ILLEGAL $00EF"},
    {0x812F, "ILLEGAL $812F"}, {0xE1A4, "ILLEGAL $E1A4"},
    {0xF1FF, "ILLEGAL $F1FF"}};

TEST_P(DisassemblerTest, VerifyOutputOfInstructions) {
  const auto [instruction, expected_disassembly_result] = GetParam();

  const auto chip8_instruction = chip8::Instruction(instruction);
  const auto disassembly_result =
      chip8::debug::DisassembleInstruction(chip8_instruction);

  ASSERT_EQ(disassembly_result, expected_disassembly_result);
}

INSTANTIATE_TEST_SUITE_P(VirtualMachineCore, DisassemblerTest,
                         testing::ValuesIn(test_data));
}  // namespace
