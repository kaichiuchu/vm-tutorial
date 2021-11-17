// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by Michael Rodriguez aka kaichiuchu <mike@kaichiuchu.dev>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "impl_interpreter.h"

auto InterpreterImplementation::GetPixelIndex(const size_t x_coord,
                                              const size_t y_coord) noexcept
    -> size_t {
  return y_coord * chip8::framebuffer::kWidth + x_coord;
}

auto InterpreterImplementation::FetchAndDecodeInstruction() const noexcept
    -> chip8::Instruction {
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-pro-bounds-constant-array-index,readability-magic-numbers)
  const auto kInstructionHiByte = memory_[program_counter_];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-pro-bounds-constant-array-index,readability-magic-numbers)
  const auto kInstructionLoByte = memory_[program_counter_ + 1];

  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,cppcoreguidelines-pro-bounds-constant-array-index,readability-magic-numbers)
  return chip8::Instruction((kInstructionHiByte << 8) | kInstructionLoByte);
}

void InterpreterImplementation::SkipNextInstructionIf(
    const bool condition_met) noexcept {
  if (condition_met) {
    next_program_counter_ =
        program_counter_ + (chip8::data_size::kInstructionLength * 2);
  }
}

auto InterpreterImplementation::GetVxVyRegisters(
    const chip8::Instruction& instruction) noexcept -> VxVyRegisters {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  return {V_[instruction.x_], V_[instruction.y_]};
}

chip8::StepResult InterpreterImplementation::Step() noexcept {
  if (IsHaltedUntilKeyPress()) {
    return chip8::StepResult::kHaltUntilKeyPress;
  }

  const auto instruction = FetchAndDecodeInstruction();
  auto [Vx, Vy] = GetVxVyRegisters(instruction);

  next_program_counter_ =
      program_counter_ + chip8::data_size::kInstructionLength;

  auto step_result = chip8::StepResult::kSuccess;

  switch (instruction.group_) {
    case chip8::instruction_groups::kControlFlowAndScreen:
      switch (instruction.byte_) {
        case chip8::control_flow_and_screen_instructions::kCLS:
          ResetFramebuffer();
          break;

        case chip8::control_flow_and_screen_instructions::kRET:
          if (stack_pointer_ < 0) {
            step_result = chip8::StepResult::kStackUnderflow;
            break;
          }

          // We just did bounds checking, so it's safe to directly access the
          // array.
          //
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
          next_program_counter_ = stack_[stack_pointer_--];
          break;

        default:
          step_result = chip8::StepResult::kInvalidInstruction;
          break;
      }
      break;

    case chip8::ungrouped_instructions::kJP_Address:
      next_program_counter_ = instruction.address_;
      break;

    case chip8::ungrouped_instructions::kCALL_Address:
      if (stack_pointer_ < static_cast<ptrdiff_t>(stack_.size() - 1)) {
        stack_[++stack_pointer_] =
            program_counter_ + chip8::data_size::kInstructionLength;

        next_program_counter_ = instruction.address_;
        break;
      }

      step_result = chip8::StepResult::kStackOverflow;
      break;

    case chip8::ungrouped_instructions::kSE_Vx_Imm:
      SkipNextInstructionIf(Vx == instruction.byte_);
      break;

    case chip8::ungrouped_instructions::kSNE_Vx_Imm:
      SkipNextInstructionIf(Vx != instruction.byte_);
      break;

    case chip8::ungrouped_instructions::kSE_Vx_Vy:
      SkipNextInstructionIf(Vx == Vy);
      break;

    case chip8::ungrouped_instructions::kLD_Vx_Imm:
      Vx = instruction.byte_;
      break;

    case chip8::ungrouped_instructions::kADD:
      Vx += instruction.byte_;

      // Because we use `uint_fast8_t` for registers, there's no guarantee that
      // there's only 8 bits; this type only guarantees that we have at least 8
      // bits. CHIP-8 doesn't care about that; 8-bit results only period. On
      // systems where `uint_fast8_t` is actually 8 bits (most of them), the
      // following statement is optimized out.
      //
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      Vx &= 0xFF;
      break;

    case chip8::instruction_groups::kMath:
      switch (instruction.nibble_) {
        case chip8::math_instructions::kLD:
          Vx = Vy;
          break;

        case chip8::math_instructions::kOR:
          Vx |= Vy;
          break;

        case chip8::math_instructions::kAND:
          Vx &= Vy;
          break;

        case chip8::math_instructions::kXOR:
          Vx ^= Vy;
          break;

        case chip8::math_instructions::kADD: {
          const auto sum = Vx + Vy;

          // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-implicit-bool-conversion)
          VF = sum > 0xFF;

          // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          Vx = sum & 0xFF;
          break;
        }

        case chip8::math_instructions::kSUB:
          VF = Vx > Vy;  // NOLINT(readability-implicit-bool-conversion)
          Vx -= Vy;

          break;

        case chip8::math_instructions::kSHR_Vx:
          VF = Vx & 1;
          Vx >>= 1;

          break;

        case chip8::math_instructions::kSUBN:
          VF = Vy > Vx;  // NOLINT(readability-implicit-bool-conversion)
          Vx = Vy - Vx;

          break;

        case chip8::math_instructions::kSHL_Vx:
          // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers,readability-implicit-bool-conversion)
          VF = (Vx & 0x80) != 0;
          Vx <<= 1;

          // Because we use `uint_fast8_t` for registers, there's no guarantee
          // that there's only 8 bits; this type only guarantees that we have at
          // least 8 bits. CHIP-8 doesn't care about that; 8-bit results only
          // period. On systems where `uint_fast8_t` is actually 8 bits (most of
          // them), the following statement is optimized out.
          //
          // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          Vx &= 0xFF;
          break;

        default:
          step_result = chip8::StepResult::kInvalidInstruction;
          break;
      }
      break;

    case chip8::ungrouped_instructions::kSNE_Vx_Vy:
      SkipNextInstructionIf(Vx != Vy);
      break;

    case chip8::ungrouped_instructions::kLD_I_Addr:
      I_ = instruction.address_;
      break;

    case chip8::ungrouped_instructions::kJP_V0_Addr:
      next_program_counter_ = V0 + instruction.address_;
      break;

    case chip8::ungrouped_instructions::kRND:
      Vx = random_number_(random_engine_) & instruction.byte_;
      break;

    case chip8::ungrouped_instructions::kDRW:
      VF = 0;

      for (unsigned int y = 0; y < instruction.nibble_; ++y) {
        const auto sprite_location = I_ + y;

        if (sprite_location >= memory_.size()) {
          step_result = chip8::StepResult::kInvalidSpriteLocation;
          break;
        }

        const auto sprite_line = memory_[sprite_location];
        const unsigned int y_pos = (Vy + y) % chip8::framebuffer::kHeight;

        for (auto x = 0; x < 8; x++) {
          const unsigned int x_pos = (Vx + x) % chip8::framebuffer::kWidth;
          const auto pixel_location = GetPixelIndex(x_pos, y_pos);

          if (sprite_line & (0x80 >> x)) {
            if (framebuffer_[pixel_location] == chip8::pixel::kWhite) {
              framebuffer_[pixel_location] = chip8::pixel::kBlack;
              VF = 1;
            } else {
              framebuffer_[pixel_location] = chip8::pixel::kWhite;
            }
          }
        }
      }
      break;

    case chip8::instruction_groups::kKeyboardControlFlow:
      switch (instruction.byte_) {
        case chip8::keyboard_control_flow_instructions::kSKP:
          if (Vx >= keypad_.size()) {
            step_result = chip8::StepResult::kInvalidKey;
            break;
          }

          // We just did bounds checking, so it's safe to directly access the
          // array.
          //
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
          SkipNextInstructionIf(keypad_[Vx] == chip8::KeyState::kPressed);
          break;

        case chip8::keyboard_control_flow_instructions::kSKNP:
          if (Vx >= keypad_.size()) {
            step_result = chip8::StepResult::kInvalidKey;
            break;
          }

          // We just did bounds checking, so it's safe to directly access the
          // array.
          //
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
          SkipNextInstructionIf(keypad_[Vx] == chip8::KeyState::kReleased);
          break;

        default:
          step_result = chip8::StepResult::kInvalidInstruction;
          break;
      }
      break;

    case chip8::instruction_groups::kTimerAndMemoryControl:
      switch (instruction.byte_) {
        case chip8::timer_and_memory_control_instructions::kLD_Vx_DT:
          Vx = delay_timer_;
          break;

        case chip8::timer_and_memory_control_instructions::kLD_Vx_K:
          HaltUntilKeyPress(instruction.x_);
          step_result = chip8::StepResult::kHaltUntilKeyPress;

          break;

        case chip8::timer_and_memory_control_instructions::kLD_DT_Vx:
          delay_timer_ = Vx;
          break;

        case chip8::timer_and_memory_control_instructions::kLD_ST_Vx:
          sound_timer_ = Vx;
          break;

        case chip8::timer_and_memory_control_instructions::kADD_I_Vx:
          I_ += Vx;
          break;

        case chip8::timer_and_memory_control_instructions::kLD_F_Vx:
          I_ = Vx * chip8::data_size::kFontLength;
          break;

        case chip8::timer_and_memory_control_instructions::kLD_B_Vx: {
          const auto ones_digit_store_address = I_ + 2;

          if (ones_digit_store_address >= memory_.size()) {
            step_result = chip8::StepResult::kInvalidMemoryLocation;
            break;
          }

          const auto hundreds_digit_store_address = I_ + 0;
          const auto tens_digit_store_address = I_ + 1;

          const auto [hundreds_digit, tens_digit, ones_digit] =
              GetPlaceValues(Vx);

          // We just did bounds checking, so it's safe to directly access the
          // array.

          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          memory_[hundreds_digit_store_address] = hundreds_digit;

          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          memory_[tens_digit_store_address] = tens_digit;

          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
          memory_[ones_digit_store_address] = ones_digit;

          break;
        }

        case chip8::timer_and_memory_control_instructions::kLD_I_Vx:
          // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
          std::copy(V_.cbegin(), V_.cbegin() + instruction.x_ + 1,
                    memory_.begin() + I_);
          break;

        case chip8::timer_and_memory_control_instructions::kLD_Vx_I:
          std::copy(memory_.cbegin() + I_,
                    // NOLINTNEXTLINE(cppcoreguidelines-narrowing-conversions)
                    memory_.cbegin() + I_ + instruction.x_ + 1, V_.begin());
          break;

        default:
          step_result = chip8::StepResult::kInvalidInstruction;
          break;
      }
      break;

    default:
      step_result = chip8::StepResult::kInvalidInstruction;
      break;
  }

  // If an error occurred, we want the program counter that caused the fault.
  if ((step_result == chip8::StepResult::kSuccess) ||
      (step_result == chip8::StepResult::kHaltUntilKeyPress)) {
    program_counter_ = next_program_counter_;
  }
  return step_result;
}
