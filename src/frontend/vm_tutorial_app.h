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

#include "controllers/debugger_window.h"
#include "controllers/logger_window.h"
#include "controllers/main_window.h"
#include "controllers/settings/settings_dialog.h"
#include "sound_manager.h"
#include "vm_thread.h"

/// This class handles the interaction between controllers and the interaction
/// of the user interface with respect to the virtual machine.
class VMTutorialApplication : public QObject {
  Q_OBJECT

 public:
  /// Constructs the vm-tutorial application.
  VMTutorialApplication() noexcept;

 private:
  /// Connects the signals from the virtual machine thread to slots.
  void ConnectVMThreadSignalsToSlots() noexcept;

  /// Connects the signals from the main window controller to slots.
  void ConnectMainWindowSignalsToSlots() noexcept;

  /// Attempts to initialize the audio subsystem.
  ///
  /// If initialization fails, an error message will be displayed to the user.
  /// It is not a fatal error.
  void InitializeAudio() noexcept;

  /// Notifies the user that the audio subsystem failed to initialize.
  ///
  /// \param error_message The error message from the audio subsystem.
  void NotifyCriticalAudioFailure(const QString& error_message) noexcept;

  /// The controller for the main window.
  MainWindowController* main_window_;

  /// The controller for the debugger window. It is encapsulated in a QPointer
  /// because the debugger window is optional.
  QPointer<DebuggerWindowController> debugger_window_;

  /// The controller for the logger window. It is encapsulated in a QPointer
  /// because the logger window is optional.
  QPointer<LoggerWindowController> logger_window_;

  /// The controller for the settings dialog. It is encapsulated in a QPointer
  /// because the settings dialog is optional.
  QPointer<SettingsDialogController> settings_dialog_;

  /// The sound manager instance. This may or may not be valid due to an
  /// initialization error, so it is imperative that before usage the validity
  /// of this object is valid.
  std::optional<SoundManager*> sound_manager_;

  /// The virtual machine thread manager.
  VMThread* vm_thread_;

  /// The current ROM data being executed. We store a 2nd copy to handle the
  /// reset functionality. The first copy is in the internal memory of the
  /// virtual machine, but we can't be sure if it's self-modifying. That's why
  /// we have another copy here for resetting.
  ///
  /// XXX: I think it is awful to store a 2nd copy...
  std::vector<uint_fast8_t> current_rom_data_;

 private slots:
  /// Called when the user has selected a ROM file to run.
  ///
  /// \param rom_file_path The ROM file the user selected.
  void StartROM(const QString& rom_file_path) noexcept;

  /// Connects the signals from the debugger to slots.
  void ConnectDebuggerSignalsToSlots() noexcept;

  /// Connects the signals from the audio settings widget to slots.
  void ConnectAudioSettingsSignalsToSlots() noexcept;

  /// Connects the signals from the machine settings widget to slots.
  void ConnectMachineSettingsSignalsToSlots() noexcept;
};
