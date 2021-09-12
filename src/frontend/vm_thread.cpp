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

VMThread::VMThread(QObject* parent_object) noexcept : QThread(parent_object) {
  vm_instance_.update_screen_func_ =
      [this](const chip8::ImplementationInterface::Framebuffer& framebuffer) {
        emit UpdateScreen(framebuffer);
      };

  vm_instance_.play_tone_func_ = [this](const double tone_duration) {
    emit PlayTone(tone_duration);
  };
}

void VMThread::run() noexcept {
  for (;;) {
    QElapsedTimer timer;
    timer.start();

    const auto step_result = vm_instance_.RunForOneFrame();

    if (step_result != chip8::StepResult::kSuccess) {
      quit();

      if (step_result != chip8::StepResult::kHaltUntilKeyPress) {
        emit ExecutionFailure(step_result);
      }
      break;
    }

    const auto elapsed_frame_time = timer.elapsed();
    const auto max_frame_time =
        static_cast<qint64>(vm_instance_.GetMaxFrameTime());

    if (elapsed_frame_time < max_frame_time) {
      QThread::msleep(
          static_cast<unsigned long>(max_frame_time - elapsed_frame_time));
    }
  }
}