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

#include "graphics_settings.h"

#include "models/app_settings.h"

GraphicsSettingsController::GraphicsSettingsController(
    QWidget* parent_widget) noexcept
    : QWidget(parent_widget) {
  view_.setupUi(this);
  PopulateDataFromAppSettings();
  ConnectSignalsToSlots();
}

void GraphicsSettingsController::ConnectSignalsToSlots() noexcept {
  connect(view_.enableBilinearFiltering, &QCheckBox::stateChanged,
          [this](const int state) {
            AppSettingsModel().SetBilinearFiltering(state);
            emit BilinearFilteringStateChanged(state);
          });
}

void GraphicsSettingsController::PopulateDataFromAppSettings() noexcept {
  view_.enableBilinearFiltering->setChecked(
      AppSettingsModel().BilinearFilteringEnabled());
}
