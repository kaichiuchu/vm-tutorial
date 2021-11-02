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

#pragma once

#include <string>

#include "spec.h"

namespace chip8 {
namespace debug {
/// Converts a CHIP-8 bytecode instruction into human-readable CHIP-8 assembly
/// language.
///
/// If \p instruction is unknown, this function will return \p ILLEGAL and a
/// base 16 representation of the instruction.
///
/// Example usage:
///  \code
///    const auto disassembly = chip8::debug::DisassembleInstruction(0x00E0);
///  \endcode
///
///  The result will be \p CLS.
///
/// \param instruction The instruction to process.
///
/// \returns Human-readable CHIP-8 assembly language.
auto DisassembleInstruction(const Instruction& instruction) noexcept
    -> std::string;
}  // namespace debug
}  // namespace chip8
