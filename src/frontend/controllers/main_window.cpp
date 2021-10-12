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

#include "main_window.h"

#include <QFileDialog>
#include <QMessageBox>

#include "models/app_settings.h"

MainWindowController::MainWindowController() noexcept {
  view_.setupUi(this);
  CreateStatusBarWidgets();

  connect(view_.actionStart_ROM, &QAction::triggered, this, [this]() {
    const auto file_name = QFileDialog::getOpenFileName(
        this, tr("Open CHIP-8 ROM file"), "",
        tr("CHIP-8 ROM files (*.c8, *.ch8);;All files (*)"));

    if (!file_name.isEmpty()) {
      emit StartROM(file_name);
    }
  });

  connect(view_.actionResume, &QAction::triggered,
          [this]() { emit ResumeEmulation(); });

  connect(view_.actionPause, &QAction::triggered,
          [this]() { emit PauseEmulation(); });

  connect(view_.actionReset, &QAction::triggered,
          [this]() { emit ResetEmulation(); });

  connect(view_.actionDisplayLogger, &QAction::triggered,
          [this]() { emit DisplayLogger(); });

  connect(view_.actionSettings, &QAction::triggered,
          [this]() { emit DisplayProgramSettings(); });
}

void MainWindowController::SetRunState(const RunState run_state) noexcept {
  switch (run_state) {
    case RunState::kRunning:
      view_.actionResume->setEnabled(false);
      view_.actionPause->setEnabled(true);
      view_.actionReset->setEnabled(true);

      break;

    case RunState::kPaused:
      view_.actionResume->setEnabled(true);
      view_.actionPause->setEnabled(false);
      view_.actionReset->setEnabled(true);

    default:
      break;
  }
}

void MainWindowController::ReportROMOpenError(
    const QString& rom_file, const QString& error_string) noexcept {
  auto error_message = QString(tr("Unable to open ROM file"));
  error_message += QString(" %1: %2").arg(rom_file).arg(error_string);

  QMessageBox::critical(this, tr("Error opening ROM"), error_message);
}

void MainWindowController::ReportROMTooLargeError(
    const QString& rom_file) noexcept {
  auto error_message = QString(tr("Unable to open ROM file"));
  error_message += QString(" %1: ").arg(rom_file);
  error_message +=
      QString(tr("The ROM file is too large, probably not a CHIP-8 ROM."));

  QMessageBox::critical(this, tr("Error opening ROM"), error_message);
}

void MainWindowController::ReportROMBadRead(
    const QString& rom_file, const quint64 bytes_read,
    const quint64 bytes_expected) noexcept {
  auto error_message = QString(tr("Failed to fully read ROM file"));
  error_message += QString(" %1: ").arg(rom_file);
  error_message += QString("%1 ").arg(bytes_read);

  const auto bytes_read_word_pluralized =
      QString("byte%1").arg(bytes_read != 1 ? "s" : "");
  error_message += QString(tr(qPrintable(bytes_read_word_pluralized)));
  error_message += QString(tr(" read, expected"));
  error_message += QString(" %1 ").arg(bytes_expected);

  const auto bytes_expected_word_pluralized =
      QString("byte%1").arg(bytes_expected != 1 ? "s" : "");

  error_message += QString(tr(qPrintable(bytes_expected_word_pluralized)));

  QMessageBox::critical(this, tr("Error reading ROM"), error_message);
}

void MainWindowController::ReportExecutionFailure(
    const chip8::StepResult step_result) noexcept {
  QString step_result_message;

  switch (step_result) {
    case chip8::StepResult::kInvalidMemoryLocation:
      step_result_message = tr("Invalid memory location");
      break;

    case chip8::StepResult::kInvalidInstruction:
      step_result_message = tr("Invalid instruction");
      break;

    case chip8::StepResult::kInvalidKey:
      step_result_message = tr("Invalid key specified");
      break;

    case chip8::StepResult::kInvalidSpriteLocation:
      step_result_message = tr("Invalid sprite location");
      break;

    case chip8::StepResult::kStackUnderflow:
      step_result_message = tr("Stack underflow");
      break;

    case chip8::StepResult::kStackOverflow:
      step_result_message = tr("Stack overflow");
      break;

    default:
      step_result_message = tr("This should never have happened!");
      break;
  }

  auto error_message = QString(tr(
      "The virtual machine encountered a problem running the guest program:"));

  error_message += QString(" %1.\n\n").arg(step_result_message);
  error_message += tr("Open debugger?");

  const auto user_response =
      QMessageBox::critical(this, tr("Execution failure"), error_message,
                            QMessageBox::Yes | QMessageBox::No);

  if (user_response == QMessageBox::Yes) {
    emit DisplayDebugger();
  }
}

void MainWindowController::SetWindowTitleGuestProgramInfo(
    const QString& program_file_name) noexcept {
  setWindowTitle(QString{"vm-tutorial - running %1"}.arg(program_file_name));
}

void MainWindowController::UpdateFPSInfo(const unsigned int current_fps,
                                         const unsigned int target_fps,
                                         const double average_fps) noexcept {
  const auto average_fps_text = QString::number(average_fps, 'f', 2);

  fps_info_->setText(QString{"FPS: %1/%2 (avg. %3ms)"}
                         .arg(current_fps)
                         .arg(target_fps)
                         .arg(average_fps_text));
}

Renderer* MainWindowController::GetRenderer() const noexcept {
  return view_.openGLWidget;
}

void MainWindowController::CreateStatusBarWidgets() noexcept {
  frame_time_info_ = new QLabel(view_.statusBar);
  fps_info_ = new QLabel(view_.statusBar);

  view_.statusBar->addPermanentWidget(fps_info_);
  view_.statusBar->addPermanentWidget(frame_time_info_);
}

void MainWindowController::keyPressEvent(QKeyEvent* key_event) noexcept {
  const auto chip8_key = AppSettingsModel().GetVMKeyBinding(key_event->key());

  if (!chip8_key) {
    QMainWindow::keyPressEvent(key_event);
    return;
  }
  emit CHIP8KeyPress(chip8_key.value());
}

void MainWindowController::keyReleaseEvent(QKeyEvent* key_event) noexcept {
  const auto chip8_key = AppSettingsModel().GetVMKeyBinding(key_event->key());

  if (!chip8_key) {
    QMainWindow::keyReleaseEvent(key_event);
    return;
  }
  emit CHIP8KeyRelease(chip8_key.value());
}