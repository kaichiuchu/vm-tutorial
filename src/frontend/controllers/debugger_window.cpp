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
  SetupFromAppSettings();
}

void DebuggerWindowController::SetEnabled(const bool enabled) noexcept {
  view_.disasmView->setEnabled(enabled);
  view_.registerView->setEnabled(enabled);
  view_.stackView->setEnabled(enabled);
  view_.memoryView->setEnabled(enabled);
  setEnabled(enabled);
}

void DebuggerWindowController::SetupFromAppSettings() noexcept {
  const auto font = AppSettingsModel().GetDebuggerFont();

  view_.disasmView->setFont(font);
  view_.registerView->setFont(font);
  view_.stackView->setFont(font);
}