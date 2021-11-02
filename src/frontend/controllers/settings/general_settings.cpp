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

#include "general_settings.h"

#include <QFileDialog>

#include "models/app_settings.h"

GeneralSettingsController::GeneralSettingsController(
    QWidget* parent_widget) noexcept
    : QWidget(parent_widget) {
  view_.setupUi(this);
  ConnectSignalsToSlots();
  PopulateDataFromAppSettings();
}

void GeneralSettingsController::ConnectSignalsToSlots() noexcept {
  connect(view_.programFilesPath, &QPushButton::clicked, [this]() {
    const auto dir = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), "/home",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty()) {
      return;
    }

    AppSettingsModel app_settings;
    app_settings.SetProgramFilesPath(dir);
    view_.programFilesPath->setText(dir);
  });
}

void GeneralSettingsController::PopulateDataFromAppSettings() noexcept {
  AppSettingsModel app_settings;
  view_.programFilesPath->setText(app_settings.GetProgramFilesPath());
}