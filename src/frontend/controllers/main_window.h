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

#include <QKeyEvent>
#include <QMainWindow>

#include "ui_main_window.h"

/// This class handles the logic of user actions that take place in the main
/// window. Additionally, it contains methods to report to the user various
/// events that take place.
class MainWindowController : public QMainWindow {
  Q_OBJECT

 public:
  /// Defines the various run states of the virtual machine.
  enum class RunState { kRunning, kPaused };

  MainWindowController() noexcept;

  /// Activates or deactivates specific buttons, based on the specified run
  /// state of the virtual machine.
  ///
  /// If the state of the virtual machine is running, then the Pause and Reset
  /// buttons become triggerable, and the Resume button becomes disabled.
  ///
  /// If the state of the virtual machine is paused, then the Reset and Resume
  /// buttons become triggerable, and the Pause button becomes disabled.
  ///
  /// \param run_state The current run state of the virtual machine.
  void SetRunState(const RunState run_state) noexcept;

  /// Reports to the user that the ROM file they selected failed to open due to
  /// an I/O error.
  ///
  /// \param rom_file The ROM file that the user selected.
  /// \param error_string The error that occurred while opening the file.
  void ReportROMOpenError(const QString& rom_file,
                          const QString& error_string) noexcept;

  /// Reports to the user that the ROM file they selected failed to open because
  /// it would exceed the maximum size of CHIP-8 internal memory.
  ///
  /// \param rom_file The ROM file that the user selected.
  void ReportROMTooLargeError(const QString& rom_file) noexcept;

  /// Reports to the user that the ROM file they selected failed to be read in
  /// its entirety.
  ///
  /// \param rom_file The ROM file that the user selected.
  /// \param bytes_read The number of bytes that were able to be read from the
  /// file.
  /// \param bytes_expected The number of bytes that were expected to be read.
  void ReportROMBadRead(const QString& rom_file, const quint64 bytes_read,
                        const quint64 bytes_expected) noexcept;

  /// Reports to the user that the virtual machine encountered a fatal error.
  ///
  /// This method will ask the user if they wish to open the debugger to inspect
  /// the failure. If they do, the \ref OpenDebugger() signal will be emitted.
  ///
  /// \param step_result The result of the execution, refer to \ref
  /// chip8::StepResult for more details.
  void ReportExecutionFailure(const chip8::StepResult step_result) noexcept;

  /// Retrieves the renderer instance.
  ///
  /// \returns The OpenGL renderer instance.
  Renderer* GetRenderer() const noexcept;

 private:
  /// From Qt documentation (with minor changes):
  ///
  /// This event handler can be reimplemented in a subclass to
  /// receive key press events for the widget.
  ///
  /// A widget must call \ref QWidget::setFocusPolicy() to accept focus
  /// initially and have focus in order to receive a key press event.
  ///
  /// If you reimplement this handler, it is very important that you call the
  /// base class implementation if you do not act upon the key.
  ///
  /// The default implementation closes popup widgets if the user presses the
  /// key sequence for \ref QKeySequence::Cancel (typically the Escape key).
  /// Otherwise the event is ignored, so that the widget's parent can interpret
  /// it.
  ///
  /// Note that \ref QKeyEvent starts with isAccepted() == true, so you do not
  /// need to call \ref QKeyEvent::accept() - just do not call the base class
  /// implementation if you act upon the key.
  ///
  /// \param key_event The information about the key press event.
  void keyPressEvent(QKeyEvent* key_event) noexcept override;

  /// From Qt documentation (with minor changes):
  ///
  /// This event handler can be reimplemented in a subclass to receive key
  /// release events for the widget.
  ///
  /// A widget must accept focus initially and have focus in order to receive a
  /// key release event.
  ///
  /// If you reimplement this handler, it is very important that you call the
  /// base class implementation if you do not act upon the key.
  ///
  /// The default implementation ignores the event, so that the widget's parent
  /// can interpret it.
  ///
  /// Note that \ref QKeyEvent starts with isAccepted() == true, so you do not
  /// need to call \ref QKeyEvent::accept() - just do not call the base class
  /// implementation if you act upon the key.
  ///
  /// We reimplement this event to signal that a relevant virtual machine key
  /// has been released.
  /// \param key_event The information about the key release event.
  void keyReleaseEvent(QKeyEvent* key_event) noexcept override;

  /// The widget view as generated by UIC.
  Ui::MainWindow view_;

 signals:
  /// The user has pressed a key that corresponds to a CHIP-8 key.
  ///
  /// \param key The CHIP-8 key to signal as pressed.
  void CHIP8KeyPress(const chip8::Key key);

  /// The user has released a key that corresponds to a CHIP-8 key.
  ///
  /// \param key The CHIP-8 key to signal as released.
  void CHIP8KeyRelease(const chip8::Key key);

  /// The user wishes to resume the execution of the virtual machine.
  void ResumeEmulation();

  /// The user wishes to pause the execution of the virtual machine.
  void PauseEmulation();

  /// The user wishes to reset the virtual machine to the startup state, with
  /// the current program.
  void ResetEmulation();

  /// The user wishes to open the debugger.
  void DisplayDebugger();

  /// The user wishes to open the logger.
  void DisplayLogger();

  /// The user wishes to open the program settings.
  void DisplayProgramSettings();

  /// The user has selected a ROM file to execute.
  void StartROM(const QString& rom_file_path);
};