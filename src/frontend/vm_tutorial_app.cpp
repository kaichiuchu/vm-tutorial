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

#include <QFileInfo>
#include <QMessageBox>
#include <filesystem>
#include <fstream>

#include "models/app_settings.h"

VMTutorialApplication::VMTutorialApplication() noexcept
    : main_window_(new MainWindowController),
      vm_thread_(new VMThread(main_window_)) {
  // We've initialized the main window and the virtual machine thread
  // immediately because these are mandatory components. The renderer is
  // initialized as part of the main window.

  // This may fail and an error message may be displayed before the main window
  // is shown, but it does not constitute a fatal program termination.
  InitializeAudio();

  ConnectVMThreadSignalsToSlots();
  ConnectMainWindowSignalsToSlots();

  // All set to go; show the main window.
  main_window_->show();
}

void VMTutorialApplication::InitializeAudio() noexcept {
  // This variable will contain the error from the audio subsystem, if any.
  QString sound_manager_error;

  // Try to initialize the sound manager.
  sound_manager_ = SoundManager::Initialize(this, sound_manager_error);

  if (!sound_manager_) {
    // The sound subsystem failed to initialize. This is an error condition we
    // must report to the user.
    NotifyCriticalAudioFailure(sound_manager_error);
    return;
  }

  connect((*sound_manager_), &SoundManager::AudioDevicesUpdated, this,
          [this]() {
            // The host system has gained or lost an audio output device. The
            // user will want to see this change.
            if (settings_dialog_) {
              settings_dialog_->audio_settings_->UpdateSoundCardList(
                  (*sound_manager_)->GetAudioOutputDevices());
            }
          });
}

void VMTutorialApplication::NotifyCriticalAudioFailure(
    const QString& error_message) noexcept {
  // The error string is split like so to allow for internationalization.

  auto message = QString{tr("The audio subsystem failed to initialize")};
  message += QString{": %1. "}.arg(error_message);
  message +=
      QString{tr("Sound will be disabled. If you wish to try and reinitialize "
                 "the audio subsystem, navigate to the audio settings and "
                 "click 'Initialize'. Alternatively, restart the program.")};

  QMessageBox::warning(nullptr, tr("Audio subsystem failure"), message);
}

void VMTutorialApplication::ConnectVMThreadSignalsToSlots() noexcept {
  connect(vm_thread_, &VMThread::BreakpointHit, this,
          [this](const uint_fast16_t address) {
            // We won't have to stop the virtual machine as the thread manager
            // has already done so for us.
            //
            // The debugger window is guaranteed to be valid, as when the
            // debugger is closed all breakpoints are cleared, so this signal
            // would never be fired anyway.
            debugger_window_->NotifyBreakpointHit(address);
          });

  connect(vm_thread_, &VMThread::ExecutionFailure, main_window_,
          &MainWindowController::ReportExecutionFailure);

  connect(vm_thread_, &VMThread::PerformanceInfo,
          [this](const VMThread::PerformanceCounters& perf_counters) {
            // Unpack the tuple into its separate components for readability's
            // sake.
            const auto [current_fps, average_fps, target_fps] = perf_counters;

            main_window_->UpdateFPSInfo(current_fps, target_fps, average_fps);
          });

  connect(vm_thread_, &VMThread::PlayTone, [this](const double duration) {
    // It is possible that the sound manager has not been instantiated due to
    // a failure in initializing it, so we have to check if it actually exists
    // before playing a tone.
    if (sound_manager_) {
      (*sound_manager_)->PlayTone(duration);
    }
  });

  connect(vm_thread_, &VMThread::RunStateChanged, this,
          [this](const RunState run_state) {
            main_window_->SetRunState(run_state);

            if (debugger_window_) {
              // If and only if the debugger window exists, we'll want to enable
              // the controls for it if the virtual machine is no longer
              // running. Otherwise, we'll disable them.
              debugger_window_->EnableControls(run_state != RunState::kRunning);
            }
          });

  connect(vm_thread_, &VMThread::UpdateScreen, main_window_->GetRenderer(),
          &Renderer::UpdateScreen);
}

void VMTutorialApplication::ConnectMainWindowSignalsToSlots() noexcept {
  connect(main_window_, &MainWindowController::CHIP8KeyPress,
          [this](const chip8::Key key) {
            // The virtual machine may be waiting for a key press before
            // continuing execution. We need to find out now because when
            // setting CHIP-8 key state, this value will be cleared.
            const auto halted_until_key_press =
                vm_thread_->vm_instance_.impl_->IsHaltedUntilKeyPress();

            vm_thread_->vm_instance_.impl_->SetKeyState(
                key, chip8::KeyState::kPressed);

            if (halted_until_key_press) {
              // The virtual machine was waiting for a key press, which means
              // that the virtual machine thread was not running. Start it now.
              vm_thread_->start();
            }
          });

  connect(main_window_, &MainWindowController::CHIP8KeyRelease,
          [this](const chip8::Key key) {
            vm_thread_->vm_instance_.impl_->SetKeyState(
                key, chip8::KeyState::kReleased);
          });

  connect(main_window_, &MainWindowController::DisplayDebugger, this, [this]() {
    if (!debugger_window_) {
      debugger_window_ = new DebuggerWindowController(vm_thread_->vm_instance_);
      debugger_window_->setAttribute(Qt::WA_DeleteOnClose);
      debugger_window_->EnableControls(!vm_thread_->isRunning());

      ConnectDebuggerSignalsToSlots();
    }
    // If the debugger window already exists, it will be brought to the
    // foreground.
    debugger_window_->show();
  });

  connect(main_window_, &MainWindowController::DisplayLogger, this, [this]() {
    if (!logger_window_) {
      logger_window_ = new LoggerWindowController();
      logger_window_->setAttribute(Qt::WA_DeleteOnClose);

      connect(vm_thread_, &VMThread::LogMessageEmitted, this,
              [this](const std::string& msg) {
                logger_window_->AddCoreMessage(QString::fromStdString(msg));
              });
    }
    // If the logger window already exists, it will be brought to the
    // foreground.
    logger_window_->show();
  });

  connect(main_window_, &MainWindowController::DisplayProgramSettings, this,
          [this]() {
            if (!settings_dialog_) {
              settings_dialog_ = new SettingsDialogController(main_window_);
              settings_dialog_->setAttribute(Qt::WA_DeleteOnClose);

              if (sound_manager_) {
                settings_dialog_->audio_settings_->UpdateSoundCardList(
                    (*sound_manager_)->GetAudioOutputDevices());
              }

              ConnectAudioSettingsSignalsToSlots();
              ConnectGraphicsSettingsSignalsToSlots();
              ConnectMachineSettingsSignalsToSlots();
            }
            // If the settings dialog already exists, it will be brought to the
            // foreground.
            settings_dialog_->show();
          });

  connect(main_window_, &MainWindowController::PauseEmulation, [this]() {
    // It's safe to call this method here, as this signal is only triggerable if
    // and only if the virtual machine is already running.
    vm_thread_->StopExecution();
  });

  connect(main_window_, &MainWindowController::ResumeEmulation, [this]() {
    // It's safe to call this method here, as this signal is only triggerable if
    // and only if the virtual machine is not running.
    vm_thread_->start();
  });

  connect(main_window_, &MainWindowController::ResetEmulation, [this]() {
    // We stop execution so that we're not loading new data as the old ROM is
    // running, which may cause spurious error messages to be displayed.
    vm_thread_->StopExecution();

    // The current program was loaded before, so it is guaranteed to be valid.
    // Upon loading a program, the virtual machine is reset.
    vm_thread_->vm_instance_.LoadProgram(current_rom_data_);

    vm_thread_->start();
  });

  connect(main_window_, &MainWindowController::StartROM, this,
          &VMTutorialApplication::StartROM);
}

void VMTutorialApplication::StartROM(const QString& rom_file_path) noexcept {
  // We need to know if the virtual machine was already running, so in the event
  // of a failure to load a new ROM the one that was already running can be
  // resumed.
  const auto vm_thread_was_running = vm_thread_->isRunning();

  // Stop execution of the virtual machine, if and only if it is already
  // running. If it is not running, this method will perform no operation. We
  // stop execution so that we're not loading new data as the old ROM is
  // running, which may cause spurious error messages to be displayed.
  vm_thread_->StopExecution();

  // We're using C++ I/O facilities, which don't handle QStrings. So, we'll
  // need to convert the QString into a form that C++ I/O facilities understand,
  // in this case an std::string. Fortunately, Qt natively provides a method
  // for this.
  const auto rom_file = rom_file_path.toStdString();

  // Now, try to open the ROM file in binary I/O mode. Using the initialization
  // constructor means the file will attempt to be opened immediately.
  std::ifstream rom_file_handle{rom_file, std::ios::binary};

  if (!rom_file_handle) {
    // There was an error in opening the file. We need to tell the user of this
    // error condition, and why it happened.
    //
    // We don't support or use exceptions in the tutorial, so we can't throw
    // something like an `std::system_error` exception. The simplest and most
    // correct action we can perform under these circumstances is to retrieve
    // the string that describes the error code specified by the thread-local
    // `errno` variable.
    main_window_->ReportROMOpenError(rom_file_path, strerror(errno));

    if (vm_thread_was_running) {
      // If and only if a ROM was running before this, resume it.
      vm_thread_->start();
    }
    // There's nothing more we can do here.
    return;
  }

  // We've opened the file successfully. Now, let's copy the contents of it into
  // our ROM data storage.
  current_rom_data_ = std::vector<uint_fast8_t>(
      std::istreambuf_iterator<char>(rom_file_handle), {});

  // We don't need the file opened anymore, we've read the file to the best of
  // our ability.
  rom_file_handle.close();

  // We'll need to reference the file size multiple times, so let's store it
  // once here.
  const auto rom_file_size = std::filesystem::file_size(rom_file);

  // We'll need to reference the size of the ROM data multiple times, so let's
  // store it once here.
  const auto rom_data_size = current_rom_data_.size();

  if (rom_data_size != rom_file_size) {
    // The file was unable to be read in its entirety. We need to tell the user
    // of this error condition, as we won't be able to use this ROM.
    main_window_->ReportROMBadRead(rom_file_path, rom_data_size, rom_file_size);

    // If and only if a ROM was running before this, resume it.
    if (vm_thread_was_running) {
      vm_thread_->start();
    }
    // There's nothing more we can do here.
    return;
  }

  if (!vm_thread_->vm_instance_.LoadProgram(current_rom_data_)) {
    // The file the user selected is too large to fit into the virtual machine's
    // internal memory. This is an error condition we must report to the user,
    // as we cannot reasonably use it. The file the user selected probably is
    // not a CHIP-8 ROM.
    main_window_->ReportROMTooLargeError(rom_file_path);

    // If and only if a ROM was running before this, resume it.
    if (vm_thread_was_running) {
      vm_thread_->start();
    }
    // There's nothing more we can do here.
    return;
  }

  // For informational purposes, change the title of the window to display the
  // ROM file currently in use.
  main_window_->SetWindowTitleGuestProgramInfo(
      QFileInfo(rom_file_path).fileName());

  // The contents of the ROM file have been copied into the virtual machine's
  // internal memory, and no errors have occurred. Start the virtual machine.
  vm_thread_->start();
}

void VMTutorialApplication::ConnectDebuggerSignalsToSlots() noexcept {
  connect(debugger_window_, &DebuggerWindowController::ToggleRunState, this,
          [this]() {
            if (vm_thread_->isRunning()) {
              vm_thread_->StopExecution();
            } else {
              vm_thread_->start();
            }
          });
}

void VMTutorialApplication::ConnectAudioSettingsSignalsToSlots() noexcept {
  if (!sound_manager_) {
    // The audio subsystem may not be active due to a failure during program
    // start. We won't be able to connect the audio settings signals to slots if
    // this is the case.
    return;
  }

  connect(settings_dialog_->audio_settings_,
          &AudioSettingsController::ToneTypeChanged, [this](const int type) {
            // This change will be reflected in the sound output immediately.
            (*sound_manager_)->tone_type_ = static_cast<ToneType>(type);
          });

  connect(settings_dialog_->audio_settings_,
          &AudioSettingsController::VolumeChanged, (*sound_manager_),
          &SoundManager::SetVolume);

  connect(settings_dialog_->audio_settings_,
          &AudioSettingsController::FrequencyChanged, (*sound_manager_),
          [this](const unsigned int freq) {
            // This change will be reflected in the sound output immediately.
            (*sound_manager_)->tone_freq_ = freq;
          });

  connect(settings_dialog_->audio_settings_,
          &AudioSettingsController::AudioDeviceChanged, (*sound_manager_),
          &SoundManager::SetAudioOutputDevice);
}

void VMTutorialApplication::ConnectGraphicsSettingsSignalsToSlots() noexcept {
  connect(settings_dialog_->graphics_settings_,
          &GraphicsSettingsController::BilinearFilteringStateChanged,
          main_window_->GetRenderer(), &Renderer::EnableBilinearFiltering);
}

void VMTutorialApplication::ConnectMachineSettingsSignalsToSlots() noexcept {
  connect(settings_dialog_->machine_settings_,
          &MachineSettingsController::MachineInstructionsPerSecondChanged,
          [this](const int instructions_per_second) {
            // This change will be reflected during the execution of the virtual
            // machine immediately.
            vm_thread_->vm_instance_.SetInstructionsPerSecond(
                instructions_per_second);
          });

  connect(settings_dialog_->machine_settings_,
          &MachineSettingsController::MachineFrameRateChanged,
          [this](const double frame_rate) {
            // This change will be reflected during the execution of the virtual
            // machine immediately.
            vm_thread_->vm_instance_.SetFrameRate(frame_rate);
          });
}
