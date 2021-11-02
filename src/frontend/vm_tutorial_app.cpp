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

#include "vm_tutorial_app.h"

#include <QFile>
#include <QFileInfo>

#include "models/app_settings.h"

VMTutorialApplication::VMTutorialApplication() noexcept
    : main_window_(new MainWindowController),
      sound_manager_(new SoundManager(this)),
      vm_thread_(new VMThread(main_window_)) {
  ConnectVMThreadSignalsToSlots();
  ConnectMainWindowSignalsToSlots();
  main_window_->show();
}

void VMTutorialApplication::ConnectMainWindowSignalsToSlots() noexcept {
  connect(main_window_, &MainWindowController::StartROM, this,
          &VMTutorialApplication::StartROM);

  connect(main_window_, &MainWindowController::DisplayLogger, this,
          &VMTutorialApplication::DisplayLogger);

  connect(main_window_, &MainWindowController::DisplayProgramSettings, this,
          &VMTutorialApplication::DisplayProgramSettings);

  connect(main_window_, &MainWindowController::CHIP8KeyPress,
          [this](const chip8::Key key) {
            const auto halted_until_key_press =
                vm_thread_->vm_instance_.impl_->IsHaltedUntilKeyPress();

            vm_thread_->vm_instance_.impl_->SetKeyState(
                key, chip8::KeyState::kPressed);

            if (halted_until_key_press) {
              vm_thread_->start();
            }
          });

  connect(main_window_, &MainWindowController::CHIP8KeyRelease,
          [this](const chip8::Key key) {
            vm_thread_->vm_instance_.impl_->SetKeyState(
                key, chip8::KeyState::kReleased);
          });

  connect(main_window_, &MainWindowController::ResumeEmulation, [this]() {
    vm_thread_->start();
    main_window_->SetRunState(MainWindowController::RunState::kRunning);
  });

  connect(main_window_, &MainWindowController::PauseEmulation, [this]() {
    vm_thread_->StopExecution();
    main_window_->SetRunState(MainWindowController::RunState::kPaused);
  });

  connect(main_window_, &MainWindowController::ResetEmulation, [this]() {
    vm_thread_->StopExecution();

    vm_thread_->vm_instance_.LoadProgram(current_rom_data_);
    vm_thread_->start();
  });
}

void VMTutorialApplication::StartROM(const QString& rom_file_path) noexcept {
  const auto vm_thread_was_running = vm_thread_->isRunning();

  vm_thread_->StopExecution();

  QFile rom_file(rom_file_path);

  if (!rom_file.open(QIODevice::ReadOnly)) {
    main_window_->ReportROMOpenError(rom_file_path, rom_file.errorString());

    // If a ROM was running, resume it.
    if (vm_thread_was_running) {
      vm_thread_->start();
    }
    return;
  }

  const auto rom_file_size = rom_file.size();

  current_rom_data_.resize(rom_file_size);

  const auto bytes_read =
      rom_file.read(reinterpret_cast<char*>(current_rom_data_.data()),
                    current_rom_data_.size());

  if (bytes_read != rom_file_size) {
    main_window_->ReportROMBadRead(rom_file_path, bytes_read, rom_file_size);

    // We don't need the file anymore.
    rom_file.close();

    // If a ROM was running, resume it.
    if (vm_thread_was_running) {
      vm_thread_->start();
    }
    return;
  }

  // We don't need the file anymore.
  rom_file.close();

  if (!vm_thread_->vm_instance_.LoadProgram(current_rom_data_)) {
    main_window_->ReportROMTooLargeError(rom_file_path);

    // If a ROM was running, resume it.
    if (vm_thread_was_running) {
      vm_thread_->start();
    }
    return;
  }

  // No errors reported, start the virtual machine thread.
  main_window_->SetRunState(MainWindowController::RunState::kRunning);

  main_window_->SetWindowTitleGuestProgramInfo(
      QFileInfo(rom_file_path).fileName());

  vm_thread_->start();
}

void VMTutorialApplication::DisplayLogger() noexcept {
  if (!logger_window_) {
    logger_window_ = new LoggerWindowController;

    connect(vm_thread_, &VMThread::LogMessageEmitted, this,
            [this](const std::string& msg) {
              logger_window_->AddCoreMessage(QString::fromStdString(msg));
            });
  }
  logger_window_->show();
}

void VMTutorialApplication::DisplayProgramSettings() noexcept {
  if (!settings_dialog_) {
    settings_dialog_ = new SettingsDialogController(main_window_);
    CreateAudioSettingsWidget();
    CreateGeneralSettingsWidget();
    CreateGraphicsSettingsWidget();
    CreateKeypadSettingsWidget();
    CreateLoggerSettingsWidget();
    CreateMachineSettingsWidget();
    AddSettingsWidgetsToSettingsContainer();
  }
  settings_dialog_->show();
}

void VMTutorialApplication::CreateAudioSettingsWidget() noexcept {
  audio_settings_ = new AudioSettingsController(settings_dialog_);

  audio_settings_->UpdateSoundCardList(sound_manager_->GetAudioOutputDevices());

  connect(audio_settings_, &AudioSettingsController::ToneTypeChanged,
          [this](const ToneType type) { sound_manager_->tone_type_ = type; });

  connect(audio_settings_, &AudioSettingsController::VolumeChanged,
          sound_manager_, &SoundManager::SetVolume);

  connect(audio_settings_, &AudioSettingsController::FrequencyChanged,
          sound_manager_, [this](const unsigned int freq) {
            sound_manager_->tone_freq_ = freq;
          });

  connect(audio_settings_, &AudioSettingsController::AudioDeviceChanged,
          sound_manager_, &SoundManager::SetAudioOutputDevice);

  connect(sound_manager_, &SoundManager::AudioDevicesUpdated, this, [this]() {
    audio_settings_->UpdateSoundCardList(
        sound_manager_->GetAudioOutputDevices());
  });
}

void VMTutorialApplication::CreateGeneralSettingsWidget() noexcept {
  general_settings_ = new GeneralSettingsController(settings_dialog_);
}

void VMTutorialApplication::CreateGraphicsSettingsWidget() noexcept {
  graphics_settings_ = new GraphicsSettingsController(settings_dialog_);

  connect(graphics_settings_,
          &GraphicsSettingsController::EnableBilinearFiltering,
          main_window_->GetRenderer(), &Renderer::EnableBilinearFiltering);
}

void VMTutorialApplication::CreateKeypadSettingsWidget() noexcept {
  keypad_settings_ = new KeypadSettingsController(settings_dialog_);
}

void VMTutorialApplication::CreateLoggerSettingsWidget() noexcept {
  logger_settings_ = new LoggerSettingsController(settings_dialog_);
}

void VMTutorialApplication::CreateMachineSettingsWidget() noexcept {
  machine_settings_ = new MachineSettingsController(settings_dialog_);

  connect(machine_settings_,
          &MachineSettingsController::MachineInstructionsPerSecondChanged,
          [this](const int instructions_per_second) {
            const auto frame_rate = AppSettingsModel().GetMachineFrameRate();

            vm_thread_->vm_instance_.SetTiming(instructions_per_second,
                                               frame_rate);
          });

  connect(
      machine_settings_, &MachineSettingsController::MachineFrameRateChanged,
      [this](const double frame_rate) {
        const auto instructions_per_second =
            AppSettingsModel().GetMachineInstructionsPerSecond();

        vm_thread_->vm_instance_.SetTiming(instructions_per_second, frame_rate);
      });
}

void VMTutorialApplication::ConnectVMThreadSignalsToSlots() noexcept {
  connect(vm_thread_, &VMThread::PerformanceInfo,
          [this](const VMThread::PerformanceCounters& perf_counters) {
            const auto [current_fps, average_fps, target_fps] = perf_counters;

            main_window_->UpdateFPSInfo(current_fps, target_fps, average_fps);
          });

  connect(vm_thread_, &VMThread::UpdateScreen, main_window_->GetRenderer(),
          &Renderer::UpdateScreen);

  connect(vm_thread_, &VMThread::PlayTone, sound_manager_,
          &SoundManager::PlayTone);

  connect(vm_thread_, &VMThread::ExecutionFailure, main_window_,
          &MainWindowController::ReportExecutionFailure);
}

void VMTutorialApplication::AddSettingsWidgetsToSettingsContainer() noexcept {
  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kGeneralSettings,
      general_settings_);

  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kMachineSettings,
      machine_settings_);

  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kGraphicsSettings,
      graphics_settings_);

  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kKeypadSettings,
      keypad_settings_);

  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kLoggerSettings,
      logger_settings_);

  settings_dialog_->AddWidgetToSettingsContainer(
      SettingsDialogController::SettingsCategory::kAudioSettings,
      audio_settings_);
}