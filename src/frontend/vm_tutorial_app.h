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

#include "controllers/logger_window.h"
#include "controllers/main_window.h"
#include "controllers/settings/audio_settings.h"
#include "controllers/settings/general_settings.h"
#include "controllers/settings/graphics_settings.h"
#include "controllers/settings/keypad_settings.h"
#include "controllers/settings/machine_settings.h"
#include "controllers/settings/settings_dialog.h"
#include "sound_manager.h"
#include "vm_thread.h"

/// This class handles the interaction between controllers and the interaction
/// of the user interface with respect to the virtual machine.
class VMTutorialApplication : public QObject {
  Q_OBJECT

 public:
  VMTutorialApplication() noexcept;

 private:
  /// Connects the signals from the virtual machine thread to event handlers.
  void ConnectVMThreadSignalsToSlots() noexcept;

  /// Connects the signals from the main window controller to event handlers.
  void ConnectMainWindowSignalsToSlots() noexcept;

  /// Creates the audio settings widget.
  void CreateAudioSettingsWidget() noexcept;

  /// Creates the general settings widget.
  void CreateGeneralSettingsWidget() noexcept;

  /// Creates the graphics settings widget.
  void CreateGraphicsSettingsWidget() noexcept;

  /// Creates the keypad settings widget.
  void CreateKeypadSettingsWidget() noexcept;

  /// Creates the machine settings widget.
  void CreateMachineSettingsWidget() noexcept;

  /// Adds the widgets of settings to the settings dialog.
  void AddSettingsWidgetsToSettingsContainer() noexcept;

  LoggerWindowController* logger_window_ = nullptr;
  MainWindowController* main_window_;
  AudioSettingsController* audio_settings_ = nullptr;
  GeneralSettingsController* general_settings_ = nullptr;
  GraphicsSettingsController* graphics_settings_ = nullptr;
  KeypadSettingsController* keypad_settings_ = nullptr;
  MachineSettingsController* machine_settings_ = nullptr;
  SettingsDialogController* settings_dialog_ = nullptr;
  SoundManager* sound_manager_;
  VMThread* vm_thread_;

  std::vector<uint_fast8_t> current_rom_data_;

 private slots:
  void StartROM(const QString& rom_file_path) noexcept;
  void DisplayLogger() noexcept;
  void DisplayProgramSettings() noexcept;
};