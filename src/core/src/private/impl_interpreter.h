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

#include <core/impl.h>

#include <random>

/// This implementation is a fetch-decode-execute loop. Each time the
/// \ref InterpreterImplementation::Step() method is called, the interpreter
/// will fetch an instruction, decode it into its fields, and execute the
/// instruction after being deduced by a switch-case statement. This is a very
/// common straightforward implementation generally used to facilitate testing
/// of guest programs and debugging, and it allows you to get off the ground
/// running very quickly.
///
/// Besides the aforementioned advantages, it's also generally portable.
/// There's almost never architecture dependent code within a standard
/// interpreter, however there may be some compiler dependencies to get some
/// speed bumps.
///
/// The major disadvantage is simply: it's slow. It is going to almost
/// certainly be the slowest possible implementation, and really should only be
/// used on host machines for which a JIT has not been targeted to its
/// architecture, or for debugging purposes. With one exception:
///
/// Slow does not necessarily mean unusable. In a practical setting, no one
/// sound of mind would write a JIT for CHIP-8; an interpreter is plenty fast
/// enough. System emulators which are heavily dependent on accurate timings
/// (e.g. Game Boy, (S)NES) do not implement a JIT due to very likely
/// synchronization issues, and for the simple fact an interpreter is plenty
/// fast, so a complicated design is not necessary.
///
/// We're going to write a JIT because this is a tutorial project, but please be
/// mindful of your guest target's technical specifications before you try to
/// write a JIT, and ask yourself if you really have to.
class InterpreterImplementation : public chip8::ImplementationInterface {
 public:
  /// Executes the next instruction.
  ///
  /// Example code:
  ///   \code
  ///     chip8::InterpreterImplementation impl;
  ///     impl.Step();
  ///   \endcode
  ///
  /// It is not necessary to call this method outside of a unit test; use \ref
  /// VMInstance::Step() instead.
  ///
  /// \returns The result of the step, refer to the \ref chip8::StepResult
  /// definition for more information.
  chip8::StepResult Step() noexcept override;

 private:
  /// We use the default random engine because there's no way in hell a CHIP-8
  /// virtual machine needs anything more sophisticated. This is necessary for
  /// the \p RND instruction.
  std::default_random_engine random_engine_;

  /// Likewise, we use a standard uniform integer distribution because as
  /// mentioned, a CHIP-8 virtual machine does not need anything more
  /// sophisticated. This is necessary for the \p RND instruction.
  std::uniform_int_distribution<> random_number_{
      chip8::data_limits::kMinRandomValue, chip8::data_limits::kMaxRandomValue};

  /// This register refers to the \p V0 register defined within the CHIP-8
  /// documentation. It is here for convenience and easy cross referencing with
  /// documentation.
  uint_fast8_t& V0 = V_[0x0];

  /// This register refers to the \p VF register defined within the CHIP-8
  /// documentation. It is here for convenience and easy cross referencing with
  /// documentation.
  uint_fast8_t& VF = V_[0xF];

  /// Returns the index of a pixel in the framebuffer.
  ///
  /// Example code:
  ///   \code
  ///     const auto index = GetPixelIndex(10, 14);
  ///   \endcode
  ///
  /// This will return the index of (10, 14) within the framebuffer.
  ///
  /// \param x_coord The X coordinate of the pixel.
  /// \param y_coord The Y coordinate of the pixel.
  ///
  /// \returns An index into the framebuffer, referring to the location of a
  /// pixel at (x_coord, y_coord).
  static auto GetPixelIndex(size_t x_coord, size_t y_coord) noexcept -> size_t;

  /// Skips the next instruction if the condition specifed was met.
  ///
  /// Example code:
  ///   \code
  ///     SkipNextInstructionIf(Vx == Vy);
  ///   \endcode
  ///
  /// If the contents of register Vx are equal to the contents of register Vy,
  /// then the program counter will be incremented by 4. Otherwise, it will be
  /// incremented normally (by 2).
  ///
  /// \param condition_met The result of a boolean expression.
  void SkipNextInstructionIf(bool condition_met) noexcept;

  /// Type alias for a pair of \p Vx and \p Vy registers.
  using VxVyRegisters = std::pair<uint_fast8_t&, uint_fast8_t&>;

  /// Retrieves a reference to the \p Vx and \p Vy registers.
  ///
  /// This method exists as a matter of convenience and clarity, and easier
  /// cross referencing with documentation.
  ///
  /// Example code:
  ///   \code
  ///     auto [Vx, Vy] = GetVxVyRegisters(0x1234);
  ///   \endcode
  ///
  /// Vx will point to \p V_[2], and Vy will point to \p V_[3].
  ///
  /// \param instruction The CHIP-8 instruction to retrieve the register
  /// information from.
  ///
  /// \returns A pair referencing the Vx and Vy registers.
  auto GetVxVyRegisters(const chip8::Instruction& instruction) noexcept
      -> VxVyRegisters;

  /// Fetches the next instruction and decodes it.
  ///
  /// The location of the instruction is dependent on the current value of the
  /// program counter. It is *critical* that the program counter be within the
  /// range of 0..4094 (0xFFE), otherwise undefined behavior will occur. We do
  /// not perform run-time checking of the program counter as that is slow, and
  /// several checks are in place to verify that the program counter is within a
  /// valid range before this method is called.
  ///
  /// \returns A \ref chip8::Instruction instance.
  auto FetchAndDecodeInstruction() const noexcept -> chip8::Instruction;

  /// The program counter to use after the current instruction has been
  /// executed.
  ///
  /// In the event of an error, we need to preserve the program counter that
  /// caused the fault.
  size_t next_program_counter_;
};
