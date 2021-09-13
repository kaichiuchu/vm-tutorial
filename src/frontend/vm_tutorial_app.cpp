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

#include "vm_tutorial_app.h"

#include <QFile>

VMTutorialApplication::VMTutorialApplication() noexcept
    : settings_dialog_(&main_window_), vm_thread_(this) {
  connect(&vm_thread_, &VMThread::UpdateScreen, main_window_.GetRenderer(),
          &Renderer::UpdateScreen);

  connect(&vm_thread_, &VMThread::ExecutionFailure, &main_window_,
          &MainWindowController::ReportExecutionFailure);

  connect(&main_window_, &MainWindowController::StartROM,
          [this](const QString& rom_file_path) {
            // We need to keep track if the virtual machine was running or not
            // so that in case opening the new ROM fails, the original program
            // can continue.
            const auto vm_thread_was_running = vm_thread_.isRunning();
            vm_thread_.quit();

            QFile rom_file(rom_file_path);

            if (!rom_file.open(QIODevice::ReadOnly)) {
              main_window_.ReportROMOpenError(rom_file_path,
                                              rom_file.errorString());

              // If a ROM was running, resume it.
              if (vm_thread_was_running) {
                vm_thread_.start();
              }
              return;
            }

            const auto rom_file_size = rom_file.size();

            std::vector<uint_fast8_t> rom_data;
            rom_data.resize(rom_file_size);

            const auto bytes_read = rom_file.read(
                reinterpret_cast<char*>(rom_data.data()), rom_data.size());

            if (bytes_read != rom_file_size) {
              main_window_.ReportROMBadRead(rom_file_path, bytes_read,
                                            rom_file_size);

              // We don't need the file anymore.
              rom_file.close();

              // If a ROM was running, resume it.
              if (vm_thread_was_running) {
                vm_thread_.start();
              }
              return;
            }

            // We don't need the file anymore.
            rom_file.close();

            if (!vm_thread_.vm_instance_.LoadProgram(rom_data)) {
              main_window_.ReportROMTooLargeError(rom_file_path);

              // If a ROM was running, resume it.
              if (vm_thread_was_running) {
                vm_thread_.start();
              }
              return;
            }

            // No errors reported, start the virtual machine thread.
            vm_thread_.start();
          });

  main_window_.show();
}