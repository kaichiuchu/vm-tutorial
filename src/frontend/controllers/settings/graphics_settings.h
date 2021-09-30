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

#include "ui_graphics_settings.h"

/// This class handles the logic of user actions that take place in the graphics
/// settings widget.
class GraphicsSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit GraphicsSettingsController(QWidget* parent_widget) noexcept;

 private:
  /// Populates the widget with the current graphics settings.
  void PopulateDataFromAppSettings() noexcept;
  Ui::GraphicsSettings view_;
};