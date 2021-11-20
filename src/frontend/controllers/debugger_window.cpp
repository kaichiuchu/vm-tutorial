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

void DebuggerWindowController::NotifyBreakpointHit(
    const uint_fast16_t address) noexcept {
  const auto message = tr("Breakpoint reached at");
  const auto address_str =
      QString{"%1"}.arg(address, 1, 16).rightJustified(4, '0').toUpper();

  view_.statusbar->showMessage(QString{"%1 %2."}.arg(message, address_str),
                               10000);
}

void DebuggerWindowController::EnableControls(const bool enabled) noexcept {
  view_.disasmView->setEnabled(enabled);
  view_.registerView->setEnabled(enabled);
  view_.stackView->setEnabled(enabled);
  view_.memoryView->setEnabled(enabled);

  view_.actionGo_to_Address->setEnabled(enabled);
  view_.actionGo_to_PC->setEnabled(enabled);
  view_.actionRun_to_Cursor->setEnabled(enabled);
  view_.actionStep_Into->setEnabled(enabled);
  view_.actionStep_Out->setEnabled(enabled);
  view_.actionStep_Over->setEnabled(enabled);
  view_.actionTrace->setEnabled(enabled);
}

void DebuggerWindowController::ConnectSignalsToSlots() noexcept {
  connect(view_.actionPause_Continue, &QAction::triggered,
          [this]() { emit ToggleRunState(); });

  connect(view_.actionRun_to_Cursor, &QAction::triggered, [this]() {
    const auto model = view_.disasmView->selectionModel();

    const auto indices = model->selectedIndexes();

    if (!indices.empty()) {
      emit AddBreakpoint(indices[0].row() * 2, BreakpointFlags::kSingleShot);
      emit ToggleRunState();
    }
  });

  connect(view_.actionStep_Into, &QAction::triggered, [this]() {
    EnableControls(false);
    const auto result = vm_instance_.PrepareForStepInto();

    if (result == chip8::StepResult::kNoSubroutine) {
      view_.statusbar->showMessage("No subroutines in this scope were found.");

      EnableControls(true);
      return;
    }
    emit ToggleRunState();
  });

  connect(view_.actionStep_Over, &QAction::triggered, [this]() {
    EnableControls(false);
    const auto result = vm_instance_.PrepareForStepOver();

    if (result == chip8::StepResult::kBadProgram) {
      view_.statusbar->showMessage(
          "Only CALL instructions are present in the guest program?");

      EnableControls(true);
      return;
    }
    emit ToggleRunState();
  });

  connect(view_.actionStep_Out, &QAction::triggered, [this]() {
    EnableControls(false);
    const auto result = vm_instance_.PrepareForStepOut();

    if (result == chip8::StepResult::kNotInSubroutine) {
      view_.statusbar->showMessage(
          "The guest program is not in any subroutine.");

      EnableControls(true);
      return;
    }
    emit ToggleRunState();
  });

  connect(view_.actionGo_to_PC, &QAction::triggered, [this]() {
    const auto row =
        disasm_model_->GetRowFromAddress(vm_instance_.impl_->program_counter_);

    view_.disasmView->scrollTo(disasm_model_->index(row, 0));
  });

  connect(view_.actionGo_to_Address, &QAction::triggered, [this]() {});
}

void DebuggerWindowController::SetupFromAppSettings() noexcept {
  const auto font = AppSettingsModel().GetDebuggerFont();

  view_.disasmView->setFont(font);
  view_.registerView->setFont(font);
  view_.stackView->setFont(font);
}