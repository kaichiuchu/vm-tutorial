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

#include <QMainWindow>

#include "core/impl.h"
#include "ui_debugger_window.h"

class DebuggerWindowController : public QMainWindow {
  Q_OBJECT

 public:
  DebuggerWindowController() noexcept;

  void UpdateMemoryView(
       std::array<uint_fast8_t, chip8::data_size::kInternalMemory>&
          mem) noexcept;

 private:
  Ui::DebuggerWindow view_;
};