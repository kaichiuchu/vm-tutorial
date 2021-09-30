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

#include "ui_audio_settings.h"

/// This class handles the logic of user actions that take place in the audio
/// settings widget.
///
/// This is a stub while we wait for the stable version of Qt 6.2 to be
/// released, which contains Qt Multimedia. This should be done according to
/// their road map by September 30th, 2021.
class AudioSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit AudioSettingsController(QWidget* parent_widget) noexcept;

 private:
  /// Populates the widget with the current settings.
  void PopulateDataFromAppSettings() noexcept;
  Ui::AudioSettings view_;
};