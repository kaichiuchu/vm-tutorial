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

#include <core/spec.h>

#include <algorithm>
#include <tuple>

namespace chip8 {
/// This class defines the interface for a CHIP-8 implementation.
///
/// There is a necessity to this class, for there are many ways to implement a
/// virtual machine, and we don't want to duplicate code for each
/// implementation. In addition, we'll want to (at some point) be able to switch
/// implementations at runtime.
///
/// Some common implementations are listed below:
///
/// 1) fetch-decode-execute
///
///    The fetch-decode-execute implementation is the simplest, most portable,
///    but also the slowest possible implementation. This type of implementation
///    is generally the first one to be implemented in a virtual machine as it
///    allows one to immediately begin testing, and will normally contain many
///    features that aid in debugging. These are generally just called
///    "interpreters".
///
///    They do exactly what the name implies; it fetches an instruction, decodes
///    it, and executes the appropriate code to handle this instruction
///    repeatedly.
///
/// 2) computed goto
///
///    This is a gcc extension used to implement efficient dispatch tables,
///    see https://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html. This
///    extension is also supported by clang. Of course, this extension is not
///    supported by all compilers, rendering this technique non-portable. This
///    can produce better code generation, however the use of `goto` may violate
///    structured programming principles.
///
///    To be clear: compilers are generally good at determining what constitutes
///    efficient dispatching, so implementing computed goto can be a hit or
///    miss. It *may* yield a noticeable benefit, it *may not*. The best thing
///    to do is implement and profile.
///
/// 3) cached interpreter
///
///    _removed as of this commit_
///
/// 4) Just-in-time compilation (JIT)
///
///    A JIT implementation is the most complicated, least portable, but will
///    almost certainly by the fastest implementation depending on the code
///    generated.
///
///    Simply put: A JIT implementation organizes the code of function
///    blocks, which are then translated into the host architecture's native
///    code and optimized. If the function is called again, the native optimized
///    code will be executed instead.
///
///    Blocks are generally referenced by program counter. If a block does not
///    exist, a fetch-decode-execute loop will have to take place, but the
///    resulting code generated will be stored as a block. This explains why
///    JITs are a bit slow at first; there isn't any blocks to start with.
///
///    A complication with JITs is the fact the code generated is tied to a
///    specific host architecture, making it non-portable. Another complication
///    is the fact one must be intimately familiar with optimization theory and
///    the host architecture, along with how to generate the most optimal code
///    for it. Finally, a good JIT implementation must handle self-modifying
///    code well. The simplest approach is to invalidate the entire block and
///    recompile it, but as you can probably tell by now, the naive approach
///    won't be the most efficient.
///
///    To maximize performance, JIT implementations sometimes do not generate
///    code involving error or bounds checking. These can have a decent run-time
///    cost, and they're pointless if you _know_ what you're running works on
///    the original hardware.
///
///    In many circles, JITs are often referred to as dynamic recompilers.
///
/// This class defines the architecture of the virtual machine, providing the
/// necessary data structures and members to fully implement the virtual
/// machine.
class ImplementationInterface {
 public:
  using Framebuffer = std::array<uint_fast32_t, framebuffer::kSize>;

  virtual ~ImplementationInterface() noexcept = default;

  /// Determines if the implementation should halt, pending a key press.
  ///
  /// Example code:
  ///   \code
  ///     const auto waiting = IsWaitingForKeyPress();
  ///   \endcode
  ///
  /// \returns true if the implementation should halt pending a key press, or
  /// false otherwise.
  auto IsHaltedUntilKeyPress() const noexcept -> bool {
    return halted_until_key_press_;
  }

  /// Executes the next instruction.
  ///
  /// Example code:
  ///   \code
  ///     chip8::InterpreterImplementation impl;
  ///     impl.Step();
  ///   \endcode
  ///
  /// It is important to note it should not be necessary to call this method
  /// outside of a unit test.
  ///
  /// \returns The result of the step, refer to the \ref chip8::StepResult
  /// definition for more information.
  virtual auto Step() noexcept -> chip8::StepResult = 0;

  /// Resets the implementation to a well-defined startup state.
  ///
  /// Example code:
  ///   \code
  ///     chip8::InterpreterImplementation impl;
  ///     impl.Reset();
  ///   \endcode
  ///
  /// It is important to note it should not be necessary to call this method
  /// outside of a unit test.
  virtual void Reset() noexcept {
    ResetFramebuffer();
    ResetStack();
    ResetKeypad();
    ResetRegisters();
    ResetGeneralPurposeRegisters();
    ResetInternalMemory();
  }

  /// Sets the program counter, performing bounds checking.
  ///
  /// If the default implementation is used, the specified new program counter
  /// cannot exceed 4094 (0xFFE). While the internal memory can reference a
  /// range between 0 to 4095 (0xFFF), instructions are two bytes long. When
  /// fetching an instruction, any value over 0xFFE will lead to an
  /// out-of-bounds array access, and thus is undefined behavior.
  ///
  /// \param new_program_counter The new program counter to set.
  ///
  /// \returns true if the program counter was changed, or false if the program
  /// counter was too large.
  auto SetProgramCounter(const size_t new_program_counter) noexcept -> bool {
    if (new_program_counter > chip8::data_limits::kProgramCounter) {
      return false;
    }

    program_counter_ = new_program_counter;
    return true;
  }

  /// Sets the delay timer value.
  ///
  /// The specified value will wraparound from 0..255 if the value is too large.
  ///
  /// \param new_delay_timer_value The new delay timer value to set.
  void SetDelayTimerValue(const uint_fast8_t new_delay_timer_value) noexcept {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    delay_timer_ = new_delay_timer_value & 0xFF;
  }

  /// Updates the state of the keypad.
  ///
  /// This method automatically handles the condition specified by the Fx0A ("LD
  /// Vx, K") instruction.
  ///
  /// Example usage:
  ///   \code
  ///     SetKeyState(chip8::Key::k1, chip8::KeyState::kPressed);
  ///   \endcode
  ///
  /// The key "1" on the keypad will be in a pressed state.
  ///
  /// \param key The key to update its state for.
  /// \param state The new state of the key.
  void SetKeyState(const chip8::Key key, const chip8::KeyState state) noexcept {
    if ((state == chip8::KeyState::kPressed) && IsHaltedUntilKeyPress()) {
      // We disable the constant array index warning because we know \ref key
      // *will* be valid.
      //
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      V_[key_press_dest_] = key;
      halted_until_key_press_ = false;
    }
    // We disable the constant array index warning because we know \ref key
    // *will* be valid.
    //
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    keypad_[key] = state;
  }

  /// CHIP-8 has 16 general purpose 8-bit registers, conventionally referred to
  /// as Vx, where `x` is a hexadecimal (base 16) digit between 0 through F.
  std::array<uint_fast8_t, data_size::kV> V_;

  /// CHIP-8 contains a full-ascending stack used to store return addresses when
  /// a subroutine is called. It can store a total of 16 return addresses.
  ///
  /// Full-ascending is a term ripped straight from ARM, meaning:
  ///
  /// In a push, the stack pointer is incremented. The stack pointer will point
  /// to the location in which the last subroutine address was stored.
  std::array<uint_fast16_t, chip8::data_size::kStack> stack_;

  /// CHIP-8 contains an internal memory space totaling 4,096 bytes (or 4KB).
  /// Historically, the first 512 bytes (0x000-0x1FF) contained the virtual
  /// machine itself. In modern implementations, a font set totaling 80 bytes
  /// (0x50) is stored at the beginning of this memory area. Programs may choose
  /// to use the font set, but it is not mandatory.
  std::array<uint_fast8_t, chip8::data_size::kInternalMemory> memory_;

  /// CHIP-8 contains a 64x32 monochrome framebuffer used for displaying
  /// graphics. In our implementation, we store BGRA32 values to allow for easy
  /// displaying through modern APIs.
  Framebuffer framebuffer_;

  /// CHIP-8 contains a hexadecimal keypad, consisting of 16 keys.
  std::array<chip8::KeyState, chip8::data_size::kKeypad> keypad_;

  /// The program counter is an index into internal memory. It may be between
  /// the ranges of 0 or 4094 (0xFFE). While the internal memory can reference a
  /// range between 0 to 4095 (0xFFF), instructions are two bytes long. When
  /// fetching an instruction, any value over 0xFFE will lead to an
  /// out-of-bounds array access, and thus is undefined behavior.
  size_t program_counter_;

  /// The stack pointer is an index to the stack area. It may be between the
  /// ranges of 0-15.
  ptrdiff_t stack_pointer_;

  /// The delay timer (aptly named) is used by programs to delay the execution
  /// of certain code paths. This is entirely up to the program how they wish to
  /// use it, but this timer is decremented at a fixed rate of 60Hz, regardless
  /// of how fast the virtual machine is running. It has a range of 0-255
  /// (0xFF).
  uint_fast8_t delay_timer_;

  /// The sound timer (once again, aptly named) is used by programs to generate
  /// sound. While the timer is >0, a tone will play. I don't have a COSMAC VIP
  /// so I don't know exactly what sound it would make, but it's safe to say
  /// modern implementations choose an arbitrary tone and frequency.
  ///
  /// Like the delay timer, it is decremented at a fixed rate of 60Hz,
  /// regardless of how fast the virtual machine is running. It has a range of
  /// 0-255 (0xFF).
  uint_fast8_t sound_timer_;

  /// The I register is an offset into internal memory, used by certain
  /// instructions.
  unsigned int I_;

 protected:
  ImplementationInterface() noexcept { Reset(); }

  /// Signals that the implementation should halt until a key is pressed.
  ///
  /// \param x The V register destination to store the key value pressed.
  void HaltUntilKeyPress(const size_t x) noexcept {
    halted_until_key_press_ = true;
    key_press_dest_ = x;
  }

  /// Clears the framebuffer.
  ///
  /// The framebuffer will contain all black pixels after this method call.
  void ResetFramebuffer() noexcept {
    std::fill(framebuffer_.begin(), framebuffer_.end(), chip8::pixel::kBlack);
  }

  /// Sets all of the elements in the internal memory to zero, and copies the
  /// default font set into internal memory.
  ///
  /// If a guest program was loaded, the program code will be cleared by this
  /// call. It will be necessary to reload the guest program, should one choose.
  void ResetInternalMemory() noexcept {
    std::fill(memory_.begin(), memory_.end(),
              chip8::initial_values::kInternalMemory);

    std::copy(chip8::initial_values::kFontSet.cbegin(),
              chip8::initial_values::kFontSet.cend(), memory_.begin());
  }

  /// Sets all of the elements in the stack to zero.
  void ResetStack() noexcept {
    std::fill(stack_.begin(), stack_.end(), chip8::initial_values::kStack);
  }

  /// Sets all of the key states within the keypad to released.
  void ResetKeypad() noexcept {
    std::fill(keypad_.begin(), keypad_.end(), chip8::initial_values::kKeypad);
  }

  /// Initializes all of the internal registers with their appropriate values.
  /// For more information, see the \ref chip8::initial_values namespace.
  void ResetRegisters() noexcept {
    program_counter_ = chip8::initial_values::kProgramCounter;
    stack_pointer_ = chip8::initial_values::kStackPointer;
    delay_timer_ = chip8::initial_values::kDelayTimer;
    sound_timer_ = chip8::initial_values::kSoundTimer;
    I_ = chip8::initial_values::kI;
    halted_until_key_press_ = chip8::initial_values::kKeyPressHaltState;
  }

  void ResetGeneralPurposeRegisters() noexcept {
    std::fill(V_.begin(), V_.end(), chip8::initial_values::kV);
  }

  using HundredsPlace = unsigned int;
  using TensPlace = unsigned int;
  using OnesPlace = unsigned int;

  using PlaceValues = std::tuple<HundredsPlace, TensPlace, OnesPlace>;

  /// Determines the digits located at the hundreds, tens, and ones places.
  ///
  /// \param Vx The value of the Vx register.
  ///
  /// \returns A tuple containing the hundreds, tens, and ones digits in that
  /// order.
  static auto GetPlaceValues(const uint_fast8_t Vx) noexcept -> PlaceValues {
    return {((Vx / 100) % 10),  // Hundreds digit
            ((Vx / 10) % 10),   // Tens digit
            Vx % 10};           // Ones digit
  }

 private:
  /// One instruction requires the virtual machine to stop execution until a key
  /// is pressed (0xFx0A, "LD Vx, K"). If this is set to true, implementations
  /// should do nothing when their \ref Step() method is called. The state of
  /// this variable can be examined through the `IsHaltedUntilKeyPress()` method
  /// call, and changed through a call to the `HaltUntilKeyPress()` method.
  bool halted_until_key_press_;

  /// The index to store a pressed key value, assuming the implementation is
  /// waiting for a key press.
  size_t key_press_dest_;
};
}  // namespace chip8
