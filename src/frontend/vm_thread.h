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

#include <core/vm_instance.h>

#include <QThread>

/// This class defines a separate thread for the virtual machine to live in.
/// This is the definition of unnecessary as a CHIP-8 virtual machine is so
/// insignificant to computation time, but we do this anyway to demonstrate how
/// a real virtual machine might operate.
class VMThread : public QThread {
  Q_OBJECT

 public:
  using AverageFPSInMS = double;
  using CurrentFPS = unsigned int;
  using TargetFPS = unsigned int;

  using PerformanceCounters = std::tuple<CurrentFPS, AverageFPSInMS, TargetFPS>;

  explicit VMThread(QObject* parent_object) noexcept;

  /// From Qt documentation:
  ///
  /// The starting point for the thread. After calling start(), the newly
  /// created thread calls this function. The default implementation simply
  /// calls exec().
  ///
  /// You can reimplement this function to facilitate advanced thread
  /// management. Returning from this method will end the execution of the
  /// thread.
  ///
  /// See also \ref QThread::start() and \ref QThread::wait().
  ///
  /// We override this method to handle execution of the virtual machine and
  /// handle various events that may occur.
  void run() noexcept override;

  /// The virtual machine instance.
  chip8::VMInstance vm_instance_;

 private:
  /// The number of frames that have been generated. This is used to calculate
  /// the frame time averaged to 1 second.
  unsigned int num_frames_;

 signals:
  /// This signal is emitted when 1 second has passed within the run loop.
  ///
  /// \param perf_info The performance information of the virtual machine.
  void PerformanceInfo(const PerformanceCounters& perf_info);

  /// This signal is emitted when it is time to update the screen.
  ///
  /// \param framebuffer The screen data to render, containing BGRA32 values.
  void UpdateScreen(
      const chip8::ImplementationInterface::Framebuffer& framebuffer);

  /// This signal is emitted when it is time to play a tone.
  ///
  /// \param tone_duration The duration of the tone in milliseconds.
  void PlayTone(double tone_duration);

  /// This signal is emitted when a fatal error has occurred within the virtual
  /// machine.
  ///
  /// \param step_result The result of the failure, refer to \ref
  /// chip8::StepResult for more details.
  void ExecutionFailure(chip8::StepResult step_result);
};