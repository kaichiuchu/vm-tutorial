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

#include "logger_settings.h"

#include <QColorDialog>
#include <QFontDialog>

#include "models/app_settings.h"

LoggerSettingsController::LoggerSettingsController(
    QWidget* parent_object) noexcept
    : QWidget(parent_object) {
  view_.setupUi(this);
  ConnectSignalsToSlots();
  PopulateDataFromAppSettings();
}

void LoggerSettingsController::ConnectSignalsToSlots() noexcept {
  connect(view_.infoColorSelect, &QPushButton::clicked,
          [this]() { SelectLevelColor(view_.infoColorSelect, "info"); });

  connect(view_.warningColorSelect, &QPushButton::clicked,
          [this]() { SelectLevelColor(view_.warningColorSelect, "warning"); });

  connect(view_.debugColorSelect, &QPushButton::clicked,
          [this]() { SelectLevelColor(view_.debugColorSelect, "debug"); });

  connect(view_.fontSelect, &QPushButton::clicked, [this]() {
    bool ok;

    const auto font = QFontDialog::getFont(&ok, this);

    if (ok) {
      SetLogFontInfo(font);
      AppSettingsModel().SetLogFont(font);
    }
  });

  connect(view_.bgColorSelect, &QPushButton::clicked, [this]() {

  });
}

void LoggerSettingsController::PopulateDataFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  SetButtonColor(view_.infoColorSelect, app_settings.GetLogLevelColor("info"));

  SetButtonColor(view_.warningColorSelect,
                 app_settings.GetLogLevelColor("warning"));

  SetButtonColor(view_.debugColorSelect,
                 app_settings.GetLogLevelColor("debug"));

  const auto font = AppSettingsModel().GetLogFont();

  if (font) {
    SetLogFontInfo(font.value());
  }
}

void LoggerSettingsController::SetLogFontInfo(const QFont& font) noexcept {
  view_.fontSelect->setFont(font);

  view_.fontSelect->setText(QString{"%1, %2, %3pt"}
                                .arg(font.family())
                                .arg(font.styleName())
                                .arg(font.pointSize()));
}

void LoggerSettingsController::SetButtonColor(QPushButton* const button,
                                              const QColor& color) noexcept {
  auto pal = button->palette();
  pal.setColor(QPalette::Button, color);
  button->setAutoFillBackground(true);
  button->setPalette(pal);
  button->update();
}

void LoggerSettingsController::SelectLevelColor(
    QPushButton* const level_button, const QString& level_name) noexcept {
  const auto color = QColorDialog::getColor(nullptr, this);

  if (color.isValid()) {
    AppSettingsModel().SetLogLevelColor(level_name, color);
    SetButtonColor(level_button, color);
  }
}