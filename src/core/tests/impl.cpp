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

// This unit test tests the functionality of implementations, verifying that the
// output for each instruction is correct. An implementation should be able to
// be "plugged in" to the `ImplementationTypes` type alias, and testing should
// be automatic.

#include "gtest/gtest.h"

// Yuck.
#include "../src/private/impl_interpreter.h"

template <class T>
class ImplementationTest : public testing::Test {
 protected:
  T impl_;
};

using ImplementationTypes = ::testing::Types<InterpreterImplementation>;
TYPED_TEST_SUITE(ImplementationTest, ImplementationTypes);

namespace {
// These type aliases are used to clarify the usage of the injection functions
// listed below.
using BitRegisters = std::pair<uint_fast8_t&, uint_fast8_t&>;
using AddRegisters = std::pair<uint_fast8_t&, uint_fast8_t&>;
using SubRegisters = std::pair<uint_fast8_t&, uint_fast8_t&>;
using BitShiftRegisters = std::pair<uint_fast8_t&, uint_fast8_t&>;

enum class opcode_state { kDoNotBranch, kBranch };

/// Injects an instruction into an implementation's internal memory, starting at
/// the main program area.
///
/// \param impl The implementation currently in use by the test.
/// \param hi The high byte of the instruction.
/// \param lo The low byte of the instruction.
void InjectInstruction(chip8::ImplementationInterface& impl,
                       const uint_fast8_t hi, const uint_fast8_t lo) noexcept {
  constexpr auto kInstructionIndexHi = chip8::initial_values::kProgramCounter;
  constexpr auto kInstructionIndexLo = kInstructionIndexHi + 1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl.memory_[kInstructionIndexHi] = hi;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl.memory_[kInstructionIndexLo] = lo;
}

void Inject_RET(chip8::ImplementationInterface& impl) noexcept {
  InjectInstruction(impl, 0x00,
                    chip8::control_flow_and_screen_instructions::kRET);
}

void Inject_SE_Vx_Imm(chip8::ImplementationInterface& impl,
                      const opcode_state state) noexcept {
  auto& V1 = impl.V_[1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V1 = 0xBA;
  const auto value = (state == opcode_state::kBranch) ? V1 : V1 - 1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x31, value);
}

void Inject_SNE_Vx_Imm(chip8::ImplementationInterface& impl,
                       const opcode_state state) noexcept {
  auto& V1 = impl.V_[1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V1 = 0xBA;
  const auto value = (state == opcode_state::kBranch) ? V1 - 1 : V1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x41, value);
}

void Inject_SE_Vx_Vy(chip8::ImplementationInterface& impl,
                     const opcode_state state) noexcept {
  auto& V0 = impl.V_[0];
  auto& V1 = impl.V_[1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V0 = 0xBA;
  V1 = (state == opcode_state::kBranch) ? V0 : V0 - 1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x50, 0x10);
}

auto InjectBitInstruction(chip8::ImplementationInterface& impl,
                          const uint_fast8_t instruction_hi,
                          const uint_fast8_t instruction_lo,
                          const uint_fast8_t first_operand,
                          const uint_fast8_t second_operand) noexcept
    -> BitRegisters {
  InjectInstruction(impl, instruction_hi, instruction_lo);
  impl.V_[0] = first_operand;
  impl.V_[1] = second_operand;

  return {impl.V_[0], impl.V_[1]};
}

auto Inject_ADD_Vx_Vy(chip8::ImplementationInterface& impl,
                      const uint_fast8_t first_addend,
                      const uint_fast8_t second_addend) noexcept
    -> AddRegisters {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x80, 0x14);
  impl.V_[0] = first_addend;
  impl.V_[1] = second_addend;

  return {impl.V_[0], impl.V_[0xF]};
}

auto Inject_SUB(chip8::ImplementationInterface& impl,
                const uint_fast8_t minuend_value,
                const uint_fast8_t subtrahend_value) noexcept -> SubRegisters {
  impl.V_[0] = minuend_value;
  impl.V_[1] = subtrahend_value;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x80, 0x15);

  return {impl.V_[0], impl.V_[0xF]};
}

auto Inject_SUBN(chip8::ImplementationInterface& impl,
                 const uint_fast8_t subtrahend_value,
                 const uint_fast8_t minuend_value) noexcept -> SubRegisters {
  impl.V_[0] = subtrahend_value;
  impl.V_[1] = minuend_value;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x80, 0x17);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return {impl.V_[0], impl.V_[0xF]};
}

auto Inject_SHR(chip8::ImplementationInterface& impl,
                const uint_fast8_t value) noexcept -> BitShiftRegisters {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x80, 0x16);

  impl.V_[0] = value;
  return {impl.V_[0], impl.V_[0xF]};
}

auto Inject_SHL(chip8::ImplementationInterface& impl,
                const uint_fast8_t value) noexcept -> BitShiftRegisters {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x80, 0x1E);

  impl.V_[0] = value;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return {impl.V_[0], impl.V_[0xF]};
}

void Inject_SNE_Vx_Vy(chip8::ImplementationInterface& impl,
                      const opcode_state state) noexcept {
  auto& V0 = impl.V_[0];
  auto& V1 = impl.V_[1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V0 = 0xBA;
  V1 = (state == opcode_state::kBranch) ? V0 - 1 : V0;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0x90, 0x10);
}

void Inject_SKP_Vx(chip8::ImplementationInterface& impl,
                   const opcode_state state) noexcept {
  impl.SetKeyState(chip8::Key::k1, state == opcode_state::kBranch
                                       ? chip8::KeyState::kPressed
                                       : chip8::KeyState::kReleased);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0xE1, 0x9E);
}

void Inject_SKNP_Vx(chip8::ImplementationInterface& impl,
                    const opcode_state state) noexcept {
  impl.SetKeyState(chip8::Key::k1, state == opcode_state::kBranch
                                       ? chip8::KeyState::kReleased
                                       : chip8::KeyState::kPressed);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl, 0xE1, 0xA1);
}
}  // namespace

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_CLS) {
  InjectInstruction(impl_, 0x00,
                    chip8::control_flow_and_screen_instructions::kCLS);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Make sure the program counter has advanced for one instruction.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);

  // Make sure that the entire framebuffer is filled with black pixels.
  ASSERT_TRUE(std::all_of(
      impl_.framebuffer_.cbegin(), impl_.framebuffer_.cend(),
      [](const uint_fast32_t pixel) { return pixel == chip8::pixel::kBlack; }));
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_RET) {
  // Put a fake address within the stack at the first element.
  //
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.stack_[0] = 0xBEE6;

  // Pretend that the stack pointer is 0.
  impl_.stack_pointer_ = 0;

  // The stack pointer is 0, and the subroutine address at stack element 0 is
  // 0xBEE6, so calling RET should set the program counter to 0xBEE6.
  Inject_RET(impl_);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Make sure the program counter is at the fake subroutine address from the
  // stack.
  ASSERT_EQ(impl_.program_counter_, 0xBEE6);

  // Make sure the stack pointer underflowed.
  ASSERT_EQ(impl_.stack_pointer_, -1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_RET_Detect_StackUnderflow) {
  // Upon reset the stack pointer is -1, so calling a RET should be sufficient
  // enough to trigger a fault condition.
  Inject_RET(impl_);

  // Make sure that the step failed, and that a stack overflow occurred.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kStackUnderflow);

  // Make sure that the program counter is trapped at the beginning of the
  // program area.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_JP) {
  // We can pretty much just try and jump to any arbitrary location, in this
  // case we'll just say 0x123.

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  InjectInstruction(impl_, 0x11, 0x23);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Make sure the program counter is at 0x0123.
  ASSERT_EQ(impl_.program_counter_, 0x0123);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_CALL) {
  // We can pretty much just try and call a subroutine at an arbitrary location.

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers
  InjectInstruction(impl_, 0x21, 0x23);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Make sure that the stack pointer pointing to the right element: It should
  // be 0 because at reset, the stack pointer is -1, the CALL opcode immediately
  // increments the stack pointer, which should then be 0.
  ASSERT_EQ(impl_.stack_pointer_, 0);

  // Make sure the return address in the stack is accurate. Upon a return from a
  // subroutine, we don't want to return to the address that contains the CALL
  // instruction, which would trap the implementation in a loop. Therefore, we
  // want to execute from the address _after_ the CALL instruction.
  //
  // We checked the validity of the stack pointer already, so it's within
  // bounds.
  //
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  ASSERT_EQ(impl_.stack_[impl_.stack_pointer_],
            chip8::initial_values::kProgramCounter + 2);

  // Make sure that the program counter is 0x0123.
  ASSERT_EQ(impl_.program_counter_, 0x0123);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_CALL_Detect_StackOverflow) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0x21, 0x23);

  // Intentionally overflow the stack pointer with some value.
  //
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.stack_pointer_ = 0xDEFEC8ED;

  // Make sure that the return value of the step is a stack overflow.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kStackOverflow);

  // Make sure the stack pointer wasn't changed.
  ASSERT_EQ(impl_.stack_pointer_, 0xDEFEC8ED);

  // Make sure the program counter points to the location that caused the fault.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SE_Vx_Imm_BranchTaken) {
  Inject_SE_Vx_Imm(impl_, opcode_state::kBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch should've been taken, so the program counter should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SE_Vx_Imm_BranchNotTaken) {
  Inject_SE_Vx_Imm(impl_, opcode_state::kDoNotBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch shouldn't have been taken, so the program counter should be
  // 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SNE_Vx_Imm_BranchTaken) {
  Inject_SNE_Vx_Imm(impl_, opcode_state::kBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch should've been taken, so the program counter should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SNE_Vx_Imm_BranchNotTaken) {
  Inject_SNE_Vx_Imm(impl_, opcode_state::kDoNotBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch shouldn't have been taken, so the program counter should be
  // 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SE_Vx_Vy_BranchTaken) {
  Inject_SE_Vx_Vy(impl_, opcode_state::kBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch should've been taken because Vx==Vy, so the program counter
  // should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SE_Vx_Vy_BranchNotTaken) {
  Inject_SE_Vx_Vy(impl_, opcode_state::kDoNotBranch);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch shouldn't have been taken, so the program counter should be
  // 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_Vx_Imm) {
  const auto& V1 = impl_.V_[1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0x61, 0xFF);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V1 should contain $FF.
  ASSERT_EQ(V1, 0xFF);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_ADD_Vx_Imm) {
  auto& V1 = impl_.V_[1];
  V1 = 1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0x71, 0xFE);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V1 should contain $FF.
  ASSERT_EQ(V1, 0xFF);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_Vx_Vy) {
  const auto& V0 = impl_.V_[0];  // Destination register
  auto& V1 = impl_.V_[1];        // Source register

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V1 = 0xBA;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0x80, 0x10);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain 0xBA.
  ASSERT_EQ(V0, V1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_OR_Vx_Vy) {
  const auto [V0, V1] = InjectBitInstruction(impl_, 0x80, 0x11, 0xB0, 0xB4);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $B4.
  ASSERT_EQ(V0, V1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_AND_Vx_Vy) {
  const auto [V0, V1] = InjectBitInstruction(impl_, 0x80, 0x12, 0xB5, 0xB1);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $B1.
  ASSERT_EQ(V0, V1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_XOR_Vx_Vy) {
  const auto [V0, V1] = InjectBitInstruction(impl_, 0x80, 0x13, 0xA0, 0xA0);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $00.
  ASSERT_EQ(V0, 0x00);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_ADD_Vx_Vy) {
  const auto [V0, VF] = Inject_ADD_Vx_Vy(impl_, 0xFE, 0x01);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $FF.
  ASSERT_EQ(V0, 0xFF);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_ADD_Vx_Vy_Detect_Overflow) {
  const auto [V0, VF] = Inject_ADD_Vx_Vy(impl_, 0xFE, 0xFA);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $F8.
  ASSERT_EQ(V0, 0xF8);

  // Since an overflow took place, the carry flag should've been set.
  ASSERT_EQ(VF, 1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SUB_Vx_Vy) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  const auto [V0, VF] = Inject_SUB(impl_, 0xFF, 0x01);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $FE.
  ASSERT_EQ(V0, 0xFE);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SUB_Vx_Vy_DetectOverflow) {
  const auto [V0, VF] = Inject_SUB(impl_, 0x03, 0x01);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $02.
  ASSERT_EQ(V0, 0x02);

  // The carry flag should've been set.
  ASSERT_EQ(VF, 1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SHR_Vx_LSBClear) {
  const auto [V0, VF] = Inject_SHR(impl_, 0xDC);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $6E.
  ASSERT_EQ(V0, 0x6E);

  // Since the LSB was clear, the carry flag should also be clear.
  ASSERT_EQ(VF, 0);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SHR_Vx_LSBSet) {
  const auto [V0, VF] = Inject_SHR(impl_, 0xDD);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain $6E.
  ASSERT_EQ(V0, 0x6E);

  // Since the LSB was set, the carry flag should've also been set.
  ASSERT_EQ(VF, 1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SUBN_Vx_Vy_NoBorrow) {
  const auto [V0, VF] = Inject_SUBN(impl_, 0x1, 0x03);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain 0x2.
  ASSERT_EQ(V0, 0x2);

  // A borrow didn't take place, so the carry flag should've been set.
  ASSERT_EQ(VF, 1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SUBN_Vx_Vy_WithBorrow) {
  const auto [V0, VF] = Inject_SUBN(impl_, 0x4, 0x3);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain 0xFF.
  ASSERT_EQ(V0, 0xFF);

  // A borrow took place, so the carry flag should not have been set.
  ASSERT_EQ(VF, 0);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SHL_Vx_WithCarry) {
  const auto [V0, VF] = Inject_SHL(impl_, 0x90);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain 0x20.
  ASSERT_EQ(V0, 0x20);

  // Since the MSB is 1, the carry flag should also be 1.
  ASSERT_EQ(VF, 1);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SHL_Vx_WithoutCarry) {
  const auto [V0, VF] = Inject_SHL(impl_, 0x5A);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // V0 should contain 0xB4.
  ASSERT_EQ(V0, 0xB4);

  // Since the MSB is 0, the carry flag should also be 0.
  ASSERT_EQ(VF, 0);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SNE_Vx_Vy_BranchTaken) {
  Inject_SNE_Vx_Vy(impl_, opcode_state::kBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Since the branch should've been taken, the program counter should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SNE_Vx_Vy_BranchNotTaken) {
  Inject_SNE_Vx_Vy(impl_, opcode_state::kDoNotBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Since the branch should've been taken, the program counter should be 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_I_Addr) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xA2, 0x45);

  // Make sure the instruction succeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // I should be 0x245.
  ASSERT_EQ(impl_.I_, 0x245);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_JP_V0_Addr) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 0x20;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xB1, 0x23);

  // Make sure the instruction succeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Since (0x20 + 0x123) = 0x143, the program counter should be as such.
  ASSERT_EQ(impl_.program_counter_, 0x143);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_RND_Vx_Imm) {
  auto& V0 = impl_.V_[0];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  V0 = 0xF2;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xC0, 0x12);

  // Make sure the instruction succeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The random result should never be 0xF2, because it is ANDed with 0x12.
  ASSERT_NE(V0, 0xF2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKP_Vx_BranchTaken) {
  Inject_SKP_Vx(impl_, opcode_state::kBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch condition should've been triggered, so the program counter
  // should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKP_Vx_BranchNotTaken) {
  Inject_SKP_Vx(impl_, opcode_state::kDoNotBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch condition should not have been triggered, so the program counter
  // should be 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKP_Vx_DetectInvalidKey) {
  Inject_SKP_Vx(impl_, opcode_state::kBranch);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[1] = 0xFE;

  // The instruction should fail.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kInvalidKey);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKNP_Vx_BranchTaken) {
  Inject_SKNP_Vx(impl_, opcode_state::kBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch condition should've been triggered, so the program counter
  // should be 0x204.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKNP_Vx_BranchNotTaken) {
  Inject_SKNP_Vx(impl_, opcode_state::kDoNotBranch);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The branch condition should not have been triggered, so the program counter
  // should be 0x202.
  ASSERT_EQ(impl_.program_counter_, chip8::initial_values::kProgramCounter + 2);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_SKNP_Vx_DetectInvalidKey) {
  Inject_SKNP_Vx(impl_, opcode_state::kBranch);

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[1] = 0xFE;

  // The instruction should fail.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kInvalidKey);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_Vx_DT) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.delay_timer_ = 0xFE;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x07);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Verify that V0 is 0xFE.
  ASSERT_EQ(impl_.V_[0], impl_.delay_timer_);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_Vx_K) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x0A);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kWaitForKeyPress);

  // Now press a key to stop waiting for a key press.
  impl_.SetKeyState(chip8::Key::k1, chip8::KeyState::kPressed);

  // Make sure that the key "pressed" made it to the V0 register.
  ASSERT_EQ(impl_.V_[0], chip8::Key::k1);

  // The virtual machine should no longer be waiting for a key press.
  ASSERT_FALSE(impl_.IsWaitingForKeyPress());
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_DT_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 0xDE;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x15);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The delay timer should be 0xDE.
  ASSERT_EQ(impl_.delay_timer_, 0xDE);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_ST_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 0xDE;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x18);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The sound timer should be 0xDE.
  ASSERT_EQ(impl_.sound_timer_, 0xDE);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_ADD_I_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 0x05;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.I_ = 0xBF;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x1E);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // The result should be 0xC4.
  ASSERT_EQ(impl_.I_, 0xC4);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_F_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 0x3;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x29);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // I should be 0xF.
  ASSERT_EQ(impl_.I_, 0xF);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_B_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.V_[0] = 123;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.I_ = 0x300;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x33);

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  ASSERT_EQ(impl_.memory_[impl_.I_ + 0], 1);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  ASSERT_EQ(impl_.memory_[impl_.I_ + 1], 2);

  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  ASSERT_EQ(impl_.memory_[impl_.I_ + 2], 3);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_B_Vx_DetectInvalidMemoryAddress) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.I_ = impl_.memory_.size() + 1;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xF0, 0x33);

  // Make sure the instruction did not succeed.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kInvalidMemoryLocation);
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_I_Vx) {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  impl_.I_ = 0x300;

  // Let's copy 16 bytes of the font set into the registers.
  std::copy(impl_.memory_.cbegin(), impl_.memory_.cbegin() + impl_.V_.size(),
            impl_.V_.begin());

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xFF, 0x55);

  // Make sure that the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Now make sure the values in memory are correct.
  std::for_each(impl_.memory_.cbegin() + impl_.I_,
                impl_.memory_.cbegin() + impl_.I_ + impl_.V_.size(),
                [this, register_index = 0](const uint_fast8_t value) mutable {
                  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                  ASSERT_EQ(value, impl_.V_[register_index++]);
                });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
TYPED_TEST(ImplementationTest, Opcode_LD_Vx_I) {
  impl_.I_ = 0x000;

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  InjectInstruction(impl_, 0xFF, 0x65);

  // Where I == 0x000 and the X portion of the instruction equals 15, this will
  // copy a portion of the font set into all of the registers.

  // Make sure the instruction succeeded.
  ASSERT_EQ(impl_.Step(), chip8::StepResult::kSuccess);

  // Now make sure the values in the registers are correct.
  std::for_each(impl_.V_.cbegin(), impl_.V_.cend(),
                [this](const uint_fast8_t value) {
                  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                  ASSERT_EQ(value, impl_.memory_[impl_.I_++]);
                });
}