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

#include "debugger_window.h"

#include <models/app_settings.h>

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

DebuggerWindowController::DebuggerWindowController(
    chip8::VMInstance& vm_instance) noexcept
    : vm_instance_(vm_instance),
      disasm_model_(new DebuggerDisasmModel(this, vm_instance)),
      registers_model_(new DebuggerRegistersModel(this, vm_instance)),
      stack_model_(new DebuggerStackModel(this, vm_instance)) {
  view_.setupUi(this);

  view_.disasmView->setModel(disasm_model_);
  view_.registerView->setModel(registers_model_);
  view_.stackView->setModel(stack_model_);

  view_.memoryView->SetData(vm_instance_.impl_->memory_);

  ConnectSignalsToSlots();
  SetupFromAppSettings();
}

auto DebuggerWindowController::ScrollToAddress(
    const uint_fast16_t address) noexcept -> bool {
  if (!disasm_model_->SetStartAddress(address)) {
    return false;
  }

  const auto row = disasm_model_->GetRowFromAddress(address);
  view_.disasmView->scrollTo(disasm_model_->index(row, 0));

  return true;
}

void DebuggerWindowController::NotifyInvalidJumpAddress(
    const uint_fast16_t address) noexcept {
  const auto translated = tr("is not a valid address.");
  const auto address_str =
      QStringLiteral("$%1").arg(address, 4, 16, QLatin1Char('0')).toUpper();
  auto str = QString{"%1 %2"}.arg(address_str).arg(translated);

  QMessageBox::critical(this, tr("Invalid address"), str);
}

void DebuggerWindowController::NotifyValueConversionError(
    const QString& address_text) noexcept {
  const auto translated =
      tr("The value was unable to be converted due to an internal fault, "
         "please report this.");

  QMessageBox::critical(this, tr("Value conversion error"), translated);
}

void DebuggerWindowController::NotifyBreakpointHit(
    const uint_fast16_t address) noexcept {
  const auto message = tr("Breakpoint reached at");
  const auto address_str =
      QString{"%1"}.arg(address, 1, 16).rightJustified(4, '0').toUpper();

  view_.statusbar->showMessage(QString{"%1 %2."}.arg(message, address_str),
                               10000);

  ScrollToAddress(address);
}

void DebuggerWindowController::NotifyTraceStart(
    const QString& trace_file) noexcept {
  auto str = QString{tr("Tracing to")};
  str += QString{" %1 "}.arg(trace_file);
  str += tr("enabled.");

  QMessageBox::information(this, tr("Trace started"), str);
}

/// Notifies the user that tracing has ended.
void DebuggerWindowController::NotifyTraceEnded() noexcept {
  const auto str = tr("Tracing has stopped.");
  QMessageBox::information(this, tr("Trace ended"), str);
}

void DebuggerWindowController::NotifyTraceFileOpenError(
    const QString& file) noexcept {
  auto message = QString{tr("Unable to open trace file")};
  message += QString{"%1 "}.arg(file);
  message += tr("for writing");
  message += tr(": %1.").arg(strerror(errno));

  QMessageBox::critical(this, tr("I/O error"), message);
}

void DebuggerWindowController::EnableControls(const bool enabled) noexcept {
  view_.disasmView->setEnabled(enabled);
  view_.registerView->setEnabled(enabled);
  view_.stackView->setEnabled(enabled);
  view_.memoryView->setEnabled(enabled);
  view_.breakpointsWidget->setEnabled(enabled);

  view_.actionGo_to_Address->setEnabled(enabled);
  view_.actionGo_to_PC->setEnabled(enabled);
  view_.actionRun_to_Cursor->setEnabled(enabled);
  view_.actionStep_Into->setEnabled(enabled);
  view_.actionStep_Out->setEnabled(enabled);
  view_.actionStep_Over->setEnabled(enabled);
  view_.actionTrace->setEnabled(enabled);

  if (enabled) {
    ScrollToAddress(vm_instance_.impl_->program_counter_);
  }
}

void DebuggerWindowController::ConnectSignalsToSlots() noexcept {
  connect(view_.actionPause_Continue, &QAction::triggered,
          [this]() { emit ToggleRunState(); });

  connect(view_.actionRun_to_Cursor, &QAction::triggered, [this]() {
    const auto model = view_.disasmView->selectionModel();

    const auto indices = model->selectedIndexes();

    if (!indices.empty()) {
      vm_instance_.breakpoints_.push_back(
          {indices[0].row() * 2,
           chip8::VMInstance::BreakpointFlags::kClearAfterTrigger});
      emit ToggleRunState();
    }
  });

  connect(view_.actionStep_Into, &QAction::triggered, [this]() {
    EnableControls(false);
    const auto result = vm_instance_.Step();

    EnableControls(true);
    NotifyBreakpointHit(vm_instance_.impl_->program_counter_);
  });

  connect(view_.actionStep_Over, &QAction::triggered, [this]() {
    EnableControls(false);
    vm_instance_.PrepareForStepOver();

    emit ToggleRunState();
  });

  connect(view_.actionStep_Out, &QAction::triggered, [this]() {
    EnableControls(false);
    const auto result = vm_instance_.PrepareForStepOut();

    if (result == chip8::StepResult::kNotInSubroutine) {
      view_.statusbar->showMessage(
          tr("The guest program is not in any subroutine."));

      EnableControls(true);
      return;
    }
    emit ToggleRunState();
  });

  connect(view_.actionGo_to_PC, &QAction::triggered,
          [this]() { ScrollToAddress(vm_instance_.impl_->program_counter_); });

  connect(view_.actionGo_to_Address, &QAction::triggered, [this]() {
    QInputDialog address_dialog;
    address_dialog.setInputMode(QInputDialog::TextInput);
    address_dialog.setWindowTitle(tr("Go to address"));

    auto text_edit = address_dialog.findChild<QLineEdit*>();
    text_edit->setInputMask("$HHHH");

    if (address_dialog.exec() == QInputDialog::Accepted) {
      // We'll have to remove the prefix from the text to get the correct value.
      const auto address_text = text_edit->text().mid(1);

      if (address_text.isEmpty()) {
        return;
      }

      bool ok;
      const auto address = address_text.toUInt(&ok, 16);

      if (!ok) {
        NotifyValueConversionError(address_text);
        return;
      }

      if (!ScrollToAddress(address)) {
        NotifyInvalidJumpAddress(address);
      }
    }
  });

  connect(view_.actionTrace, &QAction::triggered, [this]() {
    if (vm_instance_.IsTracing()) {
      vm_instance_.StopTracing();
      NotifyTraceEnded();

      return;
    }

    const auto trace_file = QFileDialog::getSaveFileName(
        this, tr("Set trace file"), "",
        tr("Trace files (*.txt);;All files (*.*)"));

    if (!trace_file.isEmpty()) {
      if (!vm_instance_.StartTracing(trace_file.toStdString())) {
        NotifyTraceFileOpenError(trace_file);
        return;
      }
      NotifyTraceStart(trace_file);
    }
  });

  connect(view_.disasmView, &QTreeView::doubleClicked,
          [this](const QModelIndex& index) {
            const auto address = disasm_model_->GetAddressFromRow(index.row());

            const auto bp = vm_instance_.FindBreakpoint(address);

            if (bp) {
              vm_instance_.breakpoints_.erase(bp.value());
              return;
            }

            vm_instance_.breakpoints_.push_back(
                {address, chip8::VMInstance::BreakpointFlags::kPreserve});
          });
}

void DebuggerWindowController::SetupFromAppSettings() noexcept {
  const auto font = AppSettingsModel().GetDebuggerFont();

  view_.disasmView->setFont(font);
  view_.registerView->setFont(font);
  view_.stackView->setFont(font);
  view_.breakpointsWidget->setFont(font);
}