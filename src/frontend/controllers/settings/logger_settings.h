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

#include <QWidget>

#include "ui_logger_settings.h"

class LoggerSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit LoggerSettingsController(QWidget* parent_object) noexcept;

 private:
  Ui::LoggerSettings view_;

  void ConnectSignalsToSlots() noexcept;

  /// Populates the widget with the current settings.
  void PopulateDataFromAppSettings() noexcept;

  void SetLogFontInfo(const QFont& font) noexcept;

  void SetButtonColor(QPushButton* const button, const QColor& color) noexcept;

  void SelectLevelColor(QPushButton* const level_button,
                        const QString& level_name) noexcept;
};