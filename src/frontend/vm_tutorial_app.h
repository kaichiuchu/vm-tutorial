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
#include "controllers/settings/audio_settings.h"
#include "controllers/settings/general_settings.h"
#include "controllers/settings/graphics_settings.h"
#include "controllers/settings/keypad_settings.h"
#include "controllers/settings/logger_settings.h"
#include "controllers/settings/machine_settings.h"
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
  /// Connects the signals from the virtual machine thread to event handlers.
  void ConnectVMThreadSignalsToSlots() noexcept;

  /// Connects the signals from the main window controller to event handlers.
  void ConnectMainWindowSignalsToSlots() noexcept;

  /// Creates the audio settings widget, and connects the signals from it to
  /// various slots.
  void CreateAudioSettingsWidget() noexcept;

  /// Creates the general settings widget, and connects the signals from it to
  /// various slots.
  void CreateGeneralSettingsWidget() noexcept;

  /// Creates the graphics settings widget, and connects the signals from it to
  /// various slots.
  void CreateGraphicsSettingsWidget() noexcept;

  /// Creates the keypad settings widget, and connects the signals from it to
  /// various slots.
  void CreateKeypadSettingsWidget() noexcept;

  /// Creates the logger settings widget, and connects the signals from it to
  /// various slots.
  void CreateLoggerSettingsWidget() noexcept;

  /// Creates the machine settings widget, and connects the signals from it to
  /// various slots.
  void CreateMachineSettingsWidget() noexcept;

  /// Adds the widgets of settings to the settings dialog.
  void AddSettingsWidgetsToSettingsContainer() noexcept;

  /// The controller for the debugger window. It is \p nullptr by default
  /// because the debugger is not opened with the main window.
  DebuggerWindowController* debugger_window_ = nullptr;

  /// The controller for the logger window. It is \p nullptr by default because
  /// the logger is not opened with the main window.
  LoggerWindowController* logger_window_ = nullptr;

  /// The controller for the main window.
  MainWindowController* main_window_;

  /// The controller for the audio settings widget. It is \p nullptr by default
  /// because the settings dialog is not opened with the main window.
  AudioSettingsController* audio_settings_ = nullptr;

  /// The controller for the general settings widget. It is \p nullptr by
  /// default because the settings dialog is not opened with the main window.
  GeneralSettingsController* general_settings_ = nullptr;

  /// The controller for the graphics settings widget. It is \p nullptr by
  /// default because the settings dialog is not opened with the main window.
  GraphicsSettingsController* graphics_settings_ = nullptr;

  /// The controller for the keypad settings widget. It is \p nullptr by default
  /// because the settings dialog is not opened with the main window.
  KeypadSettingsController* keypad_settings_ = nullptr;

  /// The controller for the logger settings widget. It is \p nullptr by default
  /// because the settings dialog is not opened with the main window.
  LoggerSettingsController* logger_settings_ = nullptr;

  /// The controller for the machine settings widget. It is \p nullptr by
  /// default because the settings dialog is not opened with the main window.
  MachineSettingsController* machine_settings_ = nullptr;

  /// The controller for the settings dialog. It is \p nullptr by default
  /// because the settings dialog is not opened with the main window.
  SettingsDialogController* settings_dialog_ = nullptr;

  /// The sound manager instance handles the management of sound devices and
  /// tone generation. It is updated implicitly by signals from the audio
  /// settings widget.
  SoundManager* sound_manager_;

  /// The virtual machine thread encapsulates the virtual machine instance.
  VMThread* vm_thread_;

  /// The current ROM data being executed.
  ///
  /// XXX: I think it is awful to store a 2nd copy...
  std::vector<uint_fast8_t> current_rom_data_;

 private slots:
  /// Called when the user has selected a ROM file to run.
  void StartROM(const QString& rom_file_path) noexcept;

  /// Called when the user wishes to display the debugger.
  void DisplayDebugger() noexcept;

  /// Called when the user wishes to display the logger.
  void DisplayLogger() noexcept;

  /// Called when the user wishes to display the program settings.
  void DisplayProgramSettings() noexcept;
};