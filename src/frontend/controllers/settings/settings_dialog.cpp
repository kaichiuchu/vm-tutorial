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
    : QDialog(parent_widget) {
  view_.setupUi(this);

  connect(view_.settingsList, &QListWidget::currentRowChanged, [this](int row) {
    view_.selectedSettingsWidget->setCurrentIndex(row);
  });
}

void SettingsDialogController::AddWidgetToSettingsContainer(
    const SettingsCategory index, QWidget* widget) noexcept {
  view_.selectedSettingsWidget->insertWidget(index, widget);
}