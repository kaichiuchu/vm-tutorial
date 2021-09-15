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

#include <QMainWindow>

#include "ui_logger_window.h"

class LoggerWindowController : public QMainWindow {
  Q_OBJECT

 public:
  LoggerWindowController() noexcept;

 private:
  Ui::LoggerWindow view_;
};