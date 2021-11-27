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

#include "settings_dialog.h"

SettingsDialogController::SettingsDialogController(
    QWidget* parent_widget) noexcept
    : QDialog(parent_widget),
      audio_settings_(new AudioSettingsController(this)),
      general_settings_(new GeneralSettingsController(this)),
      logger_settings_(new LoggerSettingsController(this)),
      machine_settings_(new MachineSettingsController(this)) {
  view_.setupUi(this);
  ConnectSignalsToSlots();
  AddSettingsToSettingsContainer();
}

void SettingsDialogController::ConnectSignalsToSlots() noexcept {
  connect(view_.settingsList, &QListWidget::currentRowChanged,
          [this](const int row) {
            view_.selectedSettingsWidget->setCurrentIndex(row);
          });
}

void SettingsDialogController::AddSettingsToSettingsContainer() noexcept {
  view_.selectedSettingsWidget->insertWidget(SettingsCategory::kGeneralSettings,
                                             general_settings_);

  view_.selectedSettingsWidget->insertWidget(SettingsCategory::kLoggerSettings,
                                             logger_settings_);

  view_.selectedSettingsWidget->insertWidget(SettingsCategory::kMachineSettings,
                                             machine_settings_);

  view_.selectedSettingsWidget->insertWidget(SettingsCategory::kAudioSettings,
                                             audio_settings_);
}
