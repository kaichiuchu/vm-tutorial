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

#include "vm_thread.h"

#include <thread>

VMThread::VMThread(QObject* parent_object) noexcept : QThread(parent_object) {
  vm_instance_.update_screen_func_ =
      [this](const chip8::ImplementationInterface::Framebuffer& framebuffer) {
        emit UpdateScreen(framebuffer);
      };

  vm_instance_.play_tone_func_ = [this](const double tone_duration) {
    emit PlayTone(tone_duration);
  };

  vm_instance_.SetLogMessageFunc(
      [this](const std::string& msg) { emit LogMessageEmitted(msg); });
}

void VMThread::StopExecution() noexcept {
  requestInterruption();
  wait();
}

void VMThread::run() noexcept {
  // We pull in this inline namespace to make parts of the frame limiter code
  // more clear.
  using namespace std::chrono_literals;

  // The deadline time point is used to determine at what future point in time
  // the thread should stop sleeping. We start at the current time point because
  // we're not *starting* the run loop in the past or in the future.
  auto deadline_time_point = std::chrono::steady_clock::now();

  // The FPS (frames per second) time point is used to determine when 1 second
  // has passed to notify listeners of performance information, should anyone
  // care. We start at the current time point because we're not *starting* the
  // run loop in the past or in the future.
  auto fps_time_point = std::chrono::steady_clock::now();

  // No frames have taken place yet, clear the frame counter so we don't report
  // specious results to the user who may care about performance information.
  num_frames_ = 0;

  // This thread will continue running until an interrupt is requested by a call
  // to \ref QThread::requestInterruption(). The only time the run loop will be
  // interrupted is if one of the following conditions are met:
  //
  // 1) an error occurred within the guest program
  // 2) the guest program is waiting for a key press
  // 3) the user has paused execution of the virtual machine
  while (!isInterruptionRequested()) {
    // The length of a frame in milliseconds can be retrieved by a call to the
    // \ref chip8::VMInstance::GetMaxFrameTime() method.
    //
    // We want the thread to stop sleeping after one frame. Update the deadline
    // accordingly.
    deadline_time_point += std::chrono::milliseconds(
        static_cast<unsigned int>(vm_instance_.GetMaxFrameTime()));

    // We need to retrieve the current point in time...
    auto current_time_point = std::chrono::steady_clock::now();

    // ...to determine the difference between the current point in time and the
    // last time we checked if we emitted performance information.
    const auto fps_update_delta =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time_point - fps_time_point);

    // Has one second passed since we emitted performance information?
    if (fps_update_delta >= 1s) {
      emit PerformanceInfo({num_frames_,
                            1000.0 / static_cast<double>(num_frames_),
                            vm_instance_.GetTargetFrameRate()});

      num_frames_ = 0;

      // We just emitted the performance information, we'll need to update the
      // time point to keep track of the passage of time.
      fps_time_point = std::chrono::steady_clock::now();
    }

    // Now run the virtual machine for one frame.
    const auto step_result = vm_instance_.RunForOneFrame();
    num_frames_++;

    if (step_result != chip8::StepResult::kSuccess) {
      // A condition has been met in which we have to stop execution of the
      // virtual machine.
      quit();

      // It is not a failure to stop execution until a key has been pressed. We
      // completely stop execution in this case because it would be pointless
      // to run the thread doing absolutely nothing.
      if (step_result != chip8::StepResult::kHaltUntilKeyPress) {
        emit ExecutionFailure(step_result);
      }
      break;
    }

    // We're done here; sleep until the deadline.
    //
    // XXX: Removing the frame limiter here will cause Qt's memory usage to
    // spike dramatically...
    std::this_thread::sleep_until(deadline_time_point);
  }
}