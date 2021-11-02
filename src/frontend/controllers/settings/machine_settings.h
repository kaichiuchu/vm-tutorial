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

#include <QWidget>

#include "ui_machine_settings.h"

/// This class handles the logic of user actions that take place in the machine
/// settings widget.
class MachineSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit MachineSettingsController(QWidget* parent_widget) noexcept;

 private:
  void ConnectSignalsToSlots() noexcept;

  void PopulateDataFromAppSettings() noexcept;
  Ui::MachineSettings view_;

 signals:
  void MachineInstructionsPerSecondChanged(int value);
  void MachineFrameRateChanged(double value);
};