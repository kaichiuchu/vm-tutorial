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

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "impl.h"
#include "logger.h"

namespace chip8 {
/// This class represents the entire virtual machine. This is the only class
/// that should ever be instantiated by the frontend, outside of unit tests or
/// benchmarks.
class VMInstance {
 public:
  /// Alias to enhance readability in template arguments.
  using ProgramCounter = uint_fast16_t;

  /// Defines the behavior of the breakpoint after it is triggered.
  enum class BreakpointFlags {
    // The breakpoint should be removed from the breakpoint list after it is
    // triggered.
    kClearAfterTrigger,

    // The breakpoint should remain even after it is triggered.
    kPreserve
  };

  /// Defines a pair containing information necessary to define a breakpoint.
  using BreakpointInfo = std::pair<ProgramCounter, BreakpointFlags>;

  /// Alias to enhance readability of \ref FindBreakpoint().
  using BreakpointsIterator = std::vector<BreakpointInfo>::iterator;

  /// Configures the virtual machine to execute 500 instructions per second
  /// (500Hz) within 60 frames.
  VMInstance() noexcept;

  /// Enables tracing to a file.
  ///
  /// Tracing logs the execution of the program to a file.
  ///
  /// \param file_name The file to write traces to.
  ///
  /// \returns \p true if the file was successfully opened, or \p false
  /// otherwise.
  auto StartTracing(std::string_view file_name) noexcept -> bool;

  /// Stops tracing to the current file, flushing the stream and closing it.
  ///
  /// If tracing isn't active, this method does nothing.
  void StopTracing() noexcept;

  /// Determines if a trace file is currently active.
  ///
  /// \returns \p true if a trace file stream is active, or \p false otherwise.
  auto IsTracing() const noexcept -> bool;

  /// Retrieves the target number of frames per second.
  ///
  /// This value is the last value passed to the \ref SetTiming() method.
  ///
  /// \returns The target number of frames per second.
  auto GetTargetFrameRate() const noexcept -> unsigned int;

  /// Retrieves the maximum frame time in milliseconds as determined by the last
  /// call to \ref SetTiming().
  ///
  /// \returns The maximum frame time in milliseconds as determined by the last
  /// call to \ref SetTiming().
  auto GetMaxFrameTime() const noexcept -> double;

  /// Checks to see if a breakpoint exists.
  ///
  /// \param address The address to search for.
  ///
  /// \returns \p An iterator to the breakpoint if it was found, or \p
  /// std::nullopt otherwise.
  auto FindBreakpoint(uint_fast16_t address) noexcept
      -> std::optional<BreakpointsIterator>;

  /// Resets the virtual machine to a well-defined startup state.
  ///
  /// This method can be called at any time, however it is advisable that
  /// frontends stop execution of the virtual machine before calling this
  /// method.
  void Reset() noexcept;

  /// Adjusts the number of steps per frame, with respect to a desired frame
  /// rate and number of instructions per second.
  ///
  /// This method can be called at any time.
  ///
  /// \param instructions_per_second The total number of instructions to execute
  /// within a specified number of desired frames.
  ///
  /// \param desired_frame_rate The desired frame rate target.
  ///
  /// \returns true if the timing was changed successfully, or false if not due
  /// to bad parameters.
  auto SetTiming(unsigned int instructions_per_second,
                 double desired_frame_rate) noexcept -> bool;

  /// Adjust the number of instructions per second, with respect to the current
  /// frame rate.
  ///
  /// This method can be called at any time.
  ///
  /// \param instructions_per_second The total number of instructions to execute
  /// within the current number of desired frames.
  ///
  /// \returns \p true if the timing was changed successfully, or \p false if
  /// not due to a bad parameter.
  auto SetInstructionsPerSecond(unsigned int instructions_per_second) noexcept
      -> bool;

  /// Adjust the number of frames per second, with respect to the current
  /// instructions per second.
  ///
  /// This method can be called at any time.
  ///
  /// \param frame_rate The number of frames to display within one second.
  ///
  /// \returns \p true if the timing was changed successfully, or \p false if
  /// not due to a bad parameter.
  auto SetFrameRate(double frame_rate) noexcept -> bool;

  /// Attempts to load program code into the current implementation's internal
  /// memory.
  ///
  /// If this method is successful, the virtual machine will be reset. It is
  /// advisable that frontends stop execution of the virtual machine before
  /// calling this method. If this method is not successful, the state of the
  /// virtual machine will be unchanged.
  ///
  /// \param program_data A container containing program data. This container
  /// MUST hold elements of the `uint_fast8_t` type.
  ///
  /// \returns true if the program data was successfully loaded, or false if the
  /// program data is larger than the CHIP-8 program area.
  template <typename Container,
            typename = std::enable_if_t<
                std::is_same_v<typename Container::value_type, uint_fast8_t>>>
  auto LoadProgram(const Container& program_data) noexcept -> bool {
    auto logger = Logger::Get();

    constexpr auto kMaxProgramSize =
        chip8::data_size::kInternalMemory - chip8::memory_region::kProgramArea;

    if (program_data.size() > kMaxProgramSize) {
      logger.Emit(Logger::LogLevel::kError,
                  "Could not load the requested program as it is too "
                  "large to fit ({} > {})",
                  program_data.size(), kMaxProgramSize);
      return false;
    }

    Reset();
    std::copy(program_data.cbegin(), program_data.cend(),
              impl_->memory_.begin() + chip8::memory_region::kProgramArea);

    logger.Emit(Logger::LogLevel::kDebug,
                "Loaded a program of size {} into internal memory",
                program_data.size());
    return true;
  }

  /// Executes the number of steps necessary to count as a full frame, based on
  /// the current timing configuration.
  ///
  /// \returns The result of the execution, refer to \ref chip8::StepResult for
  /// more details.
  auto RunForOneFrame() noexcept -> chip8::StepResult;

  /// Executes one full step of the virtual machine, and returns the result.
  ///
  /// \returns A step result which could be indicative of an error or normal
  /// operation; refer to \ref chip8::StepResult for details.
  auto Step() noexcept -> chip8::StepResult;

  /// Adds a breakpoint corresponding to the address of the first non CALL
  /// instruction in the current scope.
  auto PrepareForStepOver() noexcept -> void;

  /// Adds a breakpoint corresponding to the return address of a subroutine.
  ///
  /// \returns A step result which could be indicative of an error or normal
  /// operation; refer to \ref chip8::StepResult for details.
  auto PrepareForStepOut() noexcept -> chip8::StepResult;

  /// Direct access to the underlying implementation. Accessing the underlying
  /// implementation is only important for debugging purposes, and under no
  /// circumstances should one call the \ref ImplementationInterface::Step() or
  /// \ref ImplementationInterface::Reset() methods directly; always use the
  /// equivalent methods provided within this class.
  std::unique_ptr<ImplementationInterface> impl_;

  /// This is the function that will be called when it is time to update the
  /// screen. This may be safely set to `nullptr` if for some reason you don't
  /// care about graphics.
  std::function<void(const ImplementationInterface::Framebuffer&)>
      update_screen_func_;

  /// This is the function that will be called when it is time to play a tone.
  /// This may be safely set to `nullptr` if for some reason you don't care
  /// about sound.
  std::function<void(const double)> play_tone_func_;

  // Defines a list of breakpoints.
  std::vector<BreakpointInfo> breakpoints_;

 private:
  /// Calculates the duration a tone should be.
  ///
  /// The duration calculated is based on the current sound timer value.
  ///
  /// \returns The duration of a tone in milliseconds.
  auto CalculateDurationOfTone() const noexcept -> double;

  /// Checks to see if the timers have to be decremented.
  void CheckTimers() noexcept;

  /// Decrements the timers. This method should be called every 60Hz (also known
  /// as 8 machine steps).
  void DecrementTimers() noexcept;

  /// The number of steps that constitute one frame. The value of this variable
  /// is determined by the call to the \ref SetTiming() method.
  unsigned int number_of_steps_per_frame_;

  /// The current number of instructions to execute per second as set by the
  /// last call to \ref SetTiming().
  unsigned int instructions_per_sec_;

  /// The target frame rate as passed by the last call to the \ref SetTiming()
  /// method.
  unsigned int target_frame_rate_;

  /// The total number of steps executed since the last call to the \ref Reset()
  /// method.
  uintmax_t number_of_steps_executed_;

  /// If this is set to true, we're currently playing a tone and don't need to
  /// call the play tone callback. We need to do this, so we can decrement the
  /// sound timer normally.
  bool is_playing_tone_;

  /// The maximum frame time as determined by the last call to the \ref
  /// SetTiming() method.
  double max_frame_time_;

  /// The current frame rate as set by the last call to \ref SetTiming().
  double frame_rate_;

  struct {
    std::ofstream file_handle_;
    std::string file_name_;
  } trace_info_;
};
}  // namespace chip8
