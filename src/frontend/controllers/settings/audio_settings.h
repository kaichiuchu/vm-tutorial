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

#include <QAudioDevice>
#include <QWidget>

#include "types.h"
#include "ui_audio_settings.h"

/// This class handles the logic of user actions that take place in the audio
/// settings widget.
class AudioSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit AudioSettingsController(QWidget* parent_widget) noexcept;

  /// Updates the sound card devices list.
  ///
  /// \param audio_devices The list of output audio devices present on the
  /// system.
  void UpdateSoundCardList(const QList<QAudioDevice>& audio_devices) noexcept;

 private:
  /// Sets up the remainder of the UI.
  void SetupAdditionalUI() noexcept;

  /// Populates the widget with the current settings.
  void PopulateDataFromAppSettings() noexcept;

  /// Connects various signals from the interface to slots.
  void ConnectSignalsToSlots() noexcept;

  Ui::AudioSettings view_;

 signals:
  /// Emitted when the tone type has been changed by the user.
  void ToneTypeChanged(ToneType tone_type);

  /// Emitted when the frequency of the tone has been changed by the user.
  void FrequencyChanged(int value);

  /// Emitted when the volume has been changed by the user.
  void VolumeChanged(int value);

  /// Emitted when the audio device has been changed by the user.
  void AudioDeviceChanged(const QAudioDevice& device);
};