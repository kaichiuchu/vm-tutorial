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

#pragma once

#include <array>
#include <cstdint>

namespace chip8 {
namespace instruction_decoders {
/// Decodes the group the instruction belongs to.
///
/// This may in fact be the actual instruction if the instruction value in
/// question does not belong to any group, for which there are numerous; refer
/// to the `ungrouped_instructions` namespace.
///
/// Example code:
///  \code
///    const auto group = chip8::instruction_decoders::GetGroup(0x1234);
///  \endcode
///
/// The result will be `0x1` (1).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The group the instruction belongs to, or the actual instruction.
constexpr auto GetGroup(const uint_fast16_t instruction) noexcept
    -> unsigned int {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return instruction >> 12;
}

/// Decodes the lower 12 bits of the instruction.
///
/// Example code:
///   \code
///     const auto address = chip8::instruction_decoders::GetAddress(0x1234);
///   \endcode
///
/// The result will be `0x234` (564).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The lower 12 bits of the instruction.
constexpr auto GetAddress(const uint_fast16_t instruction) noexcept
    -> unsigned int {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return instruction & 0xFFF;
}

/// Decodes the lower 4 bits of the instruction.
///
/// Example code:
///   \code
///     const auto nibble = chip8::instruction_decoders::GetNibble(0x1234);
///   \endcode
///
/// The result will be `0x4` (4).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The lower 4 bits of the instruction.
constexpr auto GetNibble(const uint_fast16_t instruction) noexcept
    -> unsigned int {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return instruction & 0xF;
}

/// Decodes the lower 4 bits of the high byte of the instruction.
///
/// Example code:
///   \code
///     const auto x = chip8::instruction_decoders::GetX(0x1234);
///   \endcode
///
/// The result will be `0x2` (2).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The lower 4 bits of the high byte of the instruction.
constexpr auto GetX(const uint_fast16_t instruction) noexcept -> size_t {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return (instruction >> 8) & 0x0F;
}

/// Decodes the upper 4 bits of the low byte of the instruction.
///
/// Example code:
///   \code
///     const auto y = chip8::instruction_decoders::GetY(0x1234);
///   \endcode
///
/// The result will be `0x3` (3).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The upper 4 bits of the low byte of the instruction.
constexpr auto GetY(const uint_fast16_t instruction) noexcept -> size_t {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return (instruction & 0xFF) >> 4;
}

/// Decodes the lowest 8 bits of the instruction.
///
/// Example code:
///   \code
///     const auto byte = chip8::instruction_decoders::GetByte(0x1234);
///   \endcode
///
/// The result will be `0x34` (52).
///
/// Please note that it should not be necessary to call this function directly,
/// it is called implicitly when the `Instruction` struct is instantiated.
///
/// \returns The lower 8 bits of the instruction.
constexpr auto GetByte(const uint_fast16_t instruction) noexcept
    -> uint_fast8_t {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
  return instruction & 0xFF;
}
}  // namespace instruction_decoders

/// This structure encapsulates a CHIP-8 instruction value to provide automatic
/// decoding of instruction fields.
struct Instruction {
  explicit constexpr Instruction(const uint_fast16_t value) noexcept
      : group_(instruction_decoders::GetGroup(value)),
        x_(instruction_decoders::GetX(value)),
        y_(instruction_decoders::GetY(value)),
        address_(instruction_decoders::GetAddress(value)),
        byte_(instruction_decoders::GetByte(value)),
        nibble_(instruction_decoders::GetNibble(value)),
        value_(value) {}

  Instruction& operator=(const Instruction&) { return *this; }

  /// The group that this instruction falls under. For cases where an
  /// instruction does not belong to any group, this will contain the
  /// instruction instead.
  const unsigned int group_;

  /// The lower 4 bits of the high byte of the instruction. This is used to
  /// access general registers, as such it should be treated like an array
  /// index, because it is.
  const size_t x_;

  /// The upper 4 bits of the low byte of the instruction. This is used to
  /// access general registers, as such it should be treated like an array
  /// index, because it is.
  const size_t y_;

  /// The lowest 12 bits of the instruction.
  const unsigned int address_;

  /// The lowest 8 bits of the instruction.
  const uint_fast8_t byte_;

  /// The lowest 4 bits of the instruction.
  const unsigned int nibble_;

  /// The original instruction value used to create this structure.
  const uint_fast16_t value_;
};

enum Key { k0, k1, k2, k3, k4, k5, k6, k7, k8, k9, kA, kB, kC, kD, kE, kF };

enum KeyState : bool { kPressed = true, kReleased = false };

/// Defines BGRA32 values for pixel colors.
namespace pixel {
constexpr auto kWhite = 0xFFFFFF;
constexpr auto kBlack = 0x000000;
}  // namespace pixel

namespace instruction_groups {
constexpr auto kControlFlowAndScreen = 0x0;
constexpr auto kMath = 0x8;
constexpr auto kKeyboardControlFlow = 0xE;
constexpr auto kTimerAndMemoryControl = 0xF;
}  // namespace instruction_groups

namespace ungrouped_instructions {
constexpr auto kJP_Address = 0x1;
constexpr auto kCALL_Address = 0x2;
constexpr auto kSE_Vx_Imm = 0x3;
constexpr auto kSNE_Vx_Imm = 0x4;
constexpr auto kSE_Vx_Vy = 0x5;
constexpr auto kLD_Vx_Imm = 0x6;
constexpr auto kADD = 0x7;
constexpr auto kSNE_Vx_Vy = 0x9;
constexpr auto kLD_I_Addr = 0xA;
constexpr auto kJP_V0_Addr = 0xB;
constexpr auto kRND = 0xC;
constexpr auto kDRW = 0xD;
}  // namespace ungrouped_instructions

/// There are no additional operands for these instructions.
namespace control_flow_and_screen_instructions {
constexpr uint_fast8_t kCLS = 0xE0;
constexpr uint_fast8_t kRET = 0xEE;
}  // namespace control_flow_and_screen_instructions

/// The operands for these instructions are assumed to be `Vx, Vy`. If an
/// operand is specified, the one NOT specified is not used in the instruction.
namespace math_instructions {
constexpr auto kLD = 0x0;
constexpr auto kOR = 0x1;
constexpr auto kAND = 0x2;
constexpr auto kXOR = 0x3;
constexpr auto kADD = 0x4;
constexpr auto kSUB = 0x5;
constexpr auto kSHR_Vx = 0x6;
constexpr auto kSUBN = 0x7;
constexpr auto kSHL_Vx = 0xE;
}  // namespace math_instructions

/// The operands for these instructions are fixed to `Vx`.
namespace keyboard_control_flow_instructions {
constexpr uint_fast8_t kSKP = 0x9E;
constexpr uint_fast8_t kSKNP = 0xA1;
}  // namespace keyboard_control_flow_instructions

namespace timer_and_memory_control_instructions {
constexpr uint_fast8_t kLD_Vx_DT = 0x07;
constexpr uint_fast8_t kLD_Vx_K = 0x0A;
constexpr uint_fast8_t kLD_DT_Vx = 0x15;
constexpr uint_fast8_t kLD_ST_Vx = 0x18;
constexpr uint_fast8_t kADD_I_Vx = 0x1E;
constexpr uint_fast8_t kLD_F_Vx = 0x29;
constexpr uint_fast8_t kLD_B_Vx = 0x33;
constexpr uint_fast8_t kLD_I_Vx = 0x55;
constexpr uint_fast8_t kLD_Vx_I = 0x65;
}  // namespace timer_and_memory_control_instructions

/// Defines the sizes of various CHIP-8 data types.
namespace data_size {
constexpr auto kV = 16;
constexpr auto kStack = 16;
constexpr auto kInternalMemory = 4096;
constexpr auto kInstructionLength = 2;
constexpr auto kKeypad = 16;
constexpr auto kFontLength = 5;
}  // namespace data_size

/// Defines the limits of various CHIP-8 types.
namespace data_limits {
constexpr auto kProgramCounter =
    data_size::kInternalMemory - data_size::kInstructionLength;

constexpr auto kMinRandomValue = 0;
constexpr auto kMaxRandomValue = 255;
}  // namespace data_limits

namespace memory_region {
constexpr auto kProgramArea = 0x200;
}

namespace timing {
constexpr auto kDefaultInstructionsPerSecond = 500;
constexpr auto kDefaultFrameRate = 60.0;
}  // namespace timing

/// Defines the dimensions of the framebuffer.
namespace framebuffer {
constexpr auto kWidth = 64;
constexpr auto kHeight = 32;
constexpr auto kSize = kWidth * kHeight;
}  // namespace framebuffer

namespace initial_values {
constexpr auto kProgramCounter = 0x200;
constexpr auto kStackPointer = -1;
constexpr auto kStack = 0;
constexpr auto kI = 0;
constexpr auto kDelayTimer = 0;
constexpr auto kSoundTimer = 0;
constexpr auto kKeypad = KeyState::kReleased;
constexpr auto kKeyPressHaltState = false;
constexpr auto kInternalMemory = 0x00;
constexpr auto kV = 0x00;

/// This font set can be used by guest programs to display predefined
/// hexadecimal sprites, ranging from 0 to F. It should be copied to the
/// beginning of an implementation's internal memory following a reset.
constexpr std::array kFontSet = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};
}  // namespace initial_values

/// Defines the results of execution steps.
enum class StepResult {
  kSuccess,
  kHaltUntilKeyPress,
  kInvalidMemoryLocation,
  kInvalidInstruction,
  kInvalidKey,
  kInvalidSpriteLocation,
  kStackUnderflow,
  kStackOverflow
};
}  // namespace chip8
