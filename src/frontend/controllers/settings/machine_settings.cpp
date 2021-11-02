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

#include "machine_settings.h"

#include "models/app_settings.h"

MachineSettingsController::MachineSettingsController(
    QWidget* parent_widget) noexcept
    : QWidget(parent_widget) {
  view_.setupUi(this);
  PopulateDataFromAppSettings();
  ConnectSignalsToSlots();
}

void MachineSettingsController::ConnectSignalsToSlots() noexcept {
  connect(view_.instructionsPerSecondSpinBox, &QSpinBox::valueChanged,
          [this](const int value) {
            AppSettingsModel().SetMachineInstructionsPerSecond(value);
            emit MachineInstructionsPerSecondChanged(value);
          });

  connect(view_.frameRateSpinBox, &QSpinBox::valueChanged,
          [this](const int value) {
            AppSettingsModel().SetMachineFrameRate(static_cast<double>(value));
            emit MachineFrameRateChanged(static_cast<double>(value));
          });
}

void MachineSettingsController::PopulateDataFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  view_.instructionsPerSecondSpinBox->setValue(
      app_settings.GetMachineInstructionsPerSecond());

  view_.frameRateSpinBox->setValue(
      static_cast<int>(app_settings.GetMachineFrameRate()));
}