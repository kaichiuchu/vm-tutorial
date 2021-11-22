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

#include <core/disasm.h>
#include <fmt/core.h>

auto chip8::debug::DisassembleInstruction(
    const chip8::Instruction& instruction) noexcept -> std::string {
  switch (instruction.group_) {
    case chip8::instruction_groups::kControlFlowAndScreen:
      switch (instruction.byte_) {
        case chip8::control_flow_and_screen_instructions::kCLS:
          return "CLS";

        case chip8::control_flow_and_screen_instructions::kRET:
          return "RET";

        default:
          return fmt::format("ILLEGAL ${:04X}", instruction.value_);
      }

    case chip8::ungrouped_instructions::kJP_Address:
      return fmt::format("JP ${:04X}", instruction.address_);

    case chip8::ungrouped_instructions::kCALL_Address:
      return fmt::format("CALL ${:04X}", instruction.address_);

    case chip8::ungrouped_instructions::kSE_Vx_Imm:
      return fmt::format("SE V{:X}, ${:02X}", instruction.x_,
                         instruction.byte_);

    case chip8::ungrouped_instructions::kSNE_Vx_Imm:
      return fmt::format("SNE V{:X}, ${:02X}", instruction.x_,
                         instruction.byte_);

    case chip8::ungrouped_instructions::kSE_Vx_Vy:
      return fmt::format("SE V{:X}, V{:X}", instruction.x_, instruction.y_);

    case chip8::ungrouped_instructions::kLD_Vx_Imm:
      return fmt::format("LD V{:X}, ${:02X}", instruction.x_,
                         instruction.byte_);

    case chip8::ungrouped_instructions::kADD:
      return fmt::format("ADD V{:X}, ${:02X}", instruction.x_,
                         instruction.byte_);

    case chip8::instruction_groups::kMath:
      switch (instruction.nibble_) {
        case chip8::math_instructions::kLD:
          return fmt::format("LD V{:X}, V{:X}", instruction.x_, instruction.y_);

        case chip8::math_instructions::kOR:
          return fmt::format("OR V{:X}, V{:X}", instruction.x_, instruction.y_);

        case chip8::math_instructions::kAND:
          return fmt::format("AND V{:X}, V{:X}", instruction.x_,
                             instruction.y_);

        case chip8::math_instructions::kXOR:
          return fmt::format("XOR V{:X}, V{:X}", instruction.x_,
                             instruction.y_);

        case chip8::math_instructions::kADD:
          return fmt::format("ADD V{:X}, V{:X}", instruction.x_,
                             instruction.y_);

        case chip8::math_instructions::kSUB:
          return fmt::format("SUB V{:X}, V{:X}", instruction.x_,
                             instruction.y_);

        case chip8::math_instructions::kSHR_Vx:
          return fmt::format("SHR V{:X}", instruction.x_);

        case chip8::math_instructions::kSUBN:
          return fmt::format("SUBN V{:X}, V{:X}", instruction.x_,
                             instruction.y_);

        case chip8::math_instructions::kSHL_Vx:
          return fmt::format("SHL V{:X}", instruction.x_);

        default:
          return fmt::format("ILLEGAL ${:X}", instruction.value_);
      }

    case chip8::ungrouped_instructions::kSNE_Vx_Vy:
      return fmt::format("SNE V{:X}, V{:X}", instruction.x_, instruction.y_);

    case chip8::ungrouped_instructions::kLD_I_Addr:
      return fmt::format("LD I, ${:04X}", instruction.address_);

    case chip8::ungrouped_instructions::kJP_V0_Addr:
      return fmt::format("JP V0, ${:04X}", instruction.address_);

    case chip8::ungrouped_instructions::kRND:
      return fmt::format("RND V{:X}, ${:02X}", instruction.x_,
                         instruction.byte_);

    case chip8::ungrouped_instructions::kDRW:
      return fmt::format("DRW V{:X}, V{:X}, {:#}", instruction.x_,
                         instruction.y_, instruction.nibble_);

    case chip8::instruction_groups::kKeyboardControlFlow:
      switch (instruction.byte_) {
        case chip8::keyboard_control_flow_instructions::kSKP:
          return fmt::format("SKP V{:X}", instruction.x_);

        case chip8::keyboard_control_flow_instructions::kSKNP:
          return fmt::format("SKNP V{:X}", instruction.x_);

        default:
          return fmt::format("ILLEGAL ${:X}", instruction.value_);
      }

    case chip8::instruction_groups::kTimerAndMemoryControl:
      switch (instruction.byte_) {
        case chip8::timer_and_memory_control_instructions::kLD_Vx_DT:
          return fmt::format("LD V{:X}, DT", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_Vx_K:
          return fmt::format("LD V{:X}, K", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_DT_Vx:
          return fmt::format("LD DT, V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_ST_Vx:
          return fmt::format("LD ST, V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kADD_I_Vx:
          return fmt::format("ADD I, V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_F_Vx:
          return fmt::format("LD F, V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_B_Vx:
          return fmt::format("LD B, V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_I_Vx:
          return fmt::format("LD [I], V{:X}", instruction.x_);

        case chip8::timer_and_memory_control_instructions::kLD_Vx_I:
          return fmt::format("LD V{:X}, [I]", instruction.x_);

        default:
          return fmt::format("ILLEGAL ${:X}", instruction.value_);
      }

    default:
      return fmt::format("ILLEGAL ${:X}", instruction.value_);
  }
}
