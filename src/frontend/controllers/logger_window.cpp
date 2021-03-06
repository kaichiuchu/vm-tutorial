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

#include "logger_window.h"

#include <QDateTime>

LoggerWindowController::LoggerWindowController() noexcept {
  view_.setupUi(this);

  ConnectSignalsToSlots();
  view_.plainTextEdit->setReadOnly(true);
}

void LoggerWindowController::ConnectSignalsToSlots() noexcept {}

void LoggerWindowController::AddMessage(const QString& msg) noexcept {
  const auto date =
      QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
  const auto formatted = QString{"[%1]: %2"}.arg(date).arg(msg);

  view_.plainTextEdit->appendPlainText(formatted);
}