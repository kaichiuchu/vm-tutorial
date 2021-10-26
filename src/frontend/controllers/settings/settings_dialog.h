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

#include <QDialog>

#include "ui_settings_dialog.h"

/// This class handles the interaction between the user and the settings
/// dialog, which is used to configure the application.
class SettingsDialogController : public QDialog {
  Q_OBJECT

 public:
  enum SettingsCategory {
    kGeneralSettings = 0,
    kLoggerSettings = 1,
    kMachineSettings = 2,
    kGraphicsSettings = 3,
    kKeypadSettings = 4,
    kAudioSettings = 5
  };

  explicit SettingsDialogController(QWidget* parent_widget) noexcept;

  /// Adds a widget to the settings container area.
  ///
  /// \param index The setting category to associate the widget with.
  /// \param widget The widget that will be displayed when the specified setting
  /// category has been clicked.
  void AddWidgetToSettingsContainer(const SettingsCategory index,
                                    QWidget* widget) noexcept;

 private:
  Ui::SettingsDialog view_;
};