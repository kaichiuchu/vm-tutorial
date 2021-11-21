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

#include <core/vm_instance.h>

#include <QThread>

#include "types.h"

/// This class defines a separate thread for the virtual machine to live in.
/// A separate thread is used to allow the virtual machine to run at varying
/// speeds without risk of blocking the UI thread.
///
/// While it is extremely unlikely that we would ever have a scenario where
/// the virtual machine runs fast enough that Qt's event queue can't keep up,
/// we use a thread anyway to demonstrate strictly separating the two run loops.
class VMThread : public QThread {
  Q_OBJECT

 public:
  /// The average number of frames per second, in milliseconds.
  using AverageFPSInMS = double;

  /// The number of frames per second.
  using CurrentFPS = unsigned int;

  /// The desired frames per second.
  using TargetFPS = unsigned int;

  /// A collection containing the current number of frames per second, the
  /// average number of frames per second in milliseconds, and the desired
  /// number of frames per second.
  using PerformanceCounters = std::tuple<CurrentFPS, AverageFPSInMS, TargetFPS>;

  /// Constructs the virtual machine thread.
  ///
  /// \param parent_widget The parent object of which this class is a child of
  /// it.
  explicit VMThread(QObject* parent_object) noexcept;

  /// From Qt documentation:
  ///
  /// The starting point for the thread. After calling \ref QThread::start(),
  /// the newly created thread calls this function. The default implementation
  /// simply calls \ref QThread::exec().
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

  /// Stops the execution of the thread.
  ///
  /// This method has no effect if the thread is not running.
  void StopExecution() noexcept;

  /// The virtual machine instance.
  chip8::VMInstance vm_instance_;

 private:
  /// Connects callback from the virtual machine instance to slots, which in
  /// turn emit our signals.
  void ConnectCallbacksToSlots() noexcept;

  /// Configures the virtual machine based on the current application settings.
  void SetupFromAppSettings() noexcept;

  /// The number of frames that have been generated. This is used to calculate
  /// the frame time averaged to 1 second.
  unsigned int num_frames_;

 signals:
  /// Emitted when the run state of the virtual machine has changed.
  ///
  /// \param state The new run state of the virtual machine.
  void RunStateChanged(const RunState state);

  /// Emitted when a breakpoint has been hit.
  ///
  /// \param address The address of the breakpoint.
  void BreakpointHit(const uint_fast16_t address);

  /// Emitted when 1 second has passed within the run loop.
  ///
  /// \param perf_info The performance information of the virtual machine.
  void PerformanceInfo(const PerformanceCounters& perf_info);

  /// Emitted when a full frame has been completed.
  ///
  /// \param framebuffer The screen data to render, containing BGRA32 values.
  void UpdateScreen(
      const chip8::ImplementationInterface::Framebuffer& framebuffer);

  /// Emitted when the guest program is requesting to play a tone.
  ///
  /// \param tone_duration The duration of the tone in milliseconds.
  void PlayTone(const double tone_duration);

  /// Emitted when a fatal error has occurred within the virtual machine.
  ///
  /// \param step_result The result of the failure, refer to \ref
  /// chip8::StepResult for more details.
  void ExecutionFailure(const chip8::StepResult step_result);

  /// Emitted when a log message has been emitted by the virtual machine.
  ///
  /// \param msg The message from the virtual machine.
  void LogMessageEmitted(const std::string& msg);
};