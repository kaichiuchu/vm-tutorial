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

#include "ui_general_settings.h"

/// This class handles the logic of user actions that take place in the general
/// settings widget. The general settings widget handles, so far, the default
/// location where to look for CHIP-8 programs.
class GeneralSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit GeneralSettingsController(QWidget* parent_widget) noexcept;

 private:
  /// Populates the widget with the current settings.
  void PopulateDataFromAppSettings() noexcept;
  Ui::GeneralSettings view_;
};