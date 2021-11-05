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

#include <core/vm_instance.h>

#include "impl_interpreter.h"
#include "logger.h"

chip8::VMInstance::VMInstance() noexcept
    : impl_(std::make_unique<InterpreterImplementation>()),
      update_screen_func_(nullptr),
      play_tone_func_(nullptr) {
  SetTiming(chip8::timing::kDefaultInstructionsPerSecond,
            chip8::timing::kDefaultFrameRate);
  Reset();
}

void chip8::VMInstance::SetLogMessageFunc(
    const LogMessageFunc& func) const noexcept {
  Logger::Get().log_message_func_ = func;
}

void chip8::VMInstance::SetLogLevel(const LogLevel level) noexcept {
  Logger::Get().level_ = level;
}

auto chip8::VMInstance::GetTargetFrameRate() const noexcept -> unsigned int {
  return target_frame_rate_;
}

auto chip8::VMInstance::GetMaxFrameTime() const noexcept -> double {
  return max_frame_time_;
}

auto chip8::VMInstance::CalculateDurationOfTone() const noexcept -> double {
  constexpr auto kTimerDecrementRate = 60.0;
  constexpr auto kMilliseconds = 1000;

  return (impl_->sound_timer_ / kTimerDecrementRate) * kMilliseconds;
}

void chip8::VMInstance::CheckTimers() noexcept {
  // The maximum number of times this method can be called before we have to
  // decrement the timers.
  constexpr auto kMaxTimerSteps = 7;

  if ((number_of_steps_executed_ % (kMaxTimerSteps + 1)) == kMaxTimerSteps) {
    DecrementTimers();
  }
}

void chip8::VMInstance::DecrementTimers() noexcept {
  auto logger = Logger::Get();

  if (impl_->sound_timer_ > 0) {
    // We need a boolean here to make sure we're not calling the play tone
    // function just because the sound timer is >0. We have to check to see if
    // this condition was met at least once, otherwise we'll just call it over
    // and over, giving us a bad result. But we'll still need to decrement the
    // sound timer for proper operation of future conditions.
    if (!is_playing_tone_ && play_tone_func_) {
      const auto tone_duration = CalculateDurationOfTone();

      logger.Emit(LogLevel::kDebug, "Emitting a {}ms long tone", tone_duration);

      play_tone_func_(tone_duration);
      is_playing_tone_ = true;
    }
    impl_->sound_timer_--;
  } else if (impl_->sound_timer_ == 0) {
    is_playing_tone_ = false;
  }

  if (impl_->delay_timer_ > 0) {
    impl_->delay_timer_--;
  }
}

void chip8::VMInstance::Reset() noexcept {
  impl_->Reset();
  number_of_steps_executed_ = 0;
  is_playing_tone_ = false;

  Logger::Get().Emit(LogLevel::kInfo, "Virtual machine has been reset");
}

auto chip8::VMInstance::SetTiming(const unsigned int instructions_per_second,
                                  const double desired_frame_rate) noexcept
    -> bool {
  if ((instructions_per_second == 0) || (desired_frame_rate <= 0.0)) {
    return false;
  }

  const auto steps_per_frame = instructions_per_second / desired_frame_rate;

  if (steps_per_frame < 1.0) {
    // One way or another, the parameters passed to us are simply bad.
    return false;
  }

  target_frame_rate_ = static_cast<unsigned int>(desired_frame_rate);
  number_of_steps_per_frame_ = static_cast<unsigned int>(steps_per_frame);

  constexpr auto kSecInMs = 1000;
  max_frame_time_ = kSecInMs / desired_frame_rate;

  Logger::Get().Emit(LogLevel::kInfo,
                     "Timing changed to {}Hz (instructions) within {} frames",
                     instructions_per_second, desired_frame_rate);
  return true;
}

auto chip8::VMInstance::RunForOneFrame() noexcept -> chip8::StepResult {
  for (auto executed_steps = 0; executed_steps < number_of_steps_per_frame_;
       ++executed_steps) {
    const auto step_result = Step();

    if (step_result != chip8::StepResult::kSuccess) {
      return step_result;
    }
  }
  return chip8::StepResult::kSuccess;
}

auto chip8::VMInstance::Step() noexcept -> chip8::StepResult {
  CheckTimers();

  const auto result = impl_->Step();
  number_of_steps_executed_++;

  if (update_screen_func_ &&
      (number_of_steps_executed_ % number_of_steps_per_frame_) ==
          (number_of_steps_per_frame_ - 1)) {
    update_screen_func_(impl_->framebuffer_);
  }
  return result;
}
