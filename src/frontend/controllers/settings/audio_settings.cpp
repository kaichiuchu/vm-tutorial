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

#include "audio_settings.h"

#include "models/app_settings.h"

AudioSettingsController::AudioSettingsController(
    QWidget* parent_widget) noexcept
    : QWidget(parent_widget) {
  view_.setupUi(this);
  ConnectSignalsToSlots();
  PopulateDataFromAppSettings();
}

void AudioSettingsController::UpdateSoundCardList(
    const QList<QAudioDevice>& audio_devices) noexcept {
  view_.soundCards->clear();

  for (auto& audio_device : audio_devices) {
    view_.soundCards->addItem(audio_device.description(),
                              QVariant::fromValue(audio_device));
  }
}

void AudioSettingsController::PopulateDataFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  const auto tone_freq = app_settings.GetAudioToneFrequency();
  const auto audio_volume = app_settings.GetAudioVolume();

  view_.volumeSlider->setValue(audio_volume);
  view_.volumeSpinBox->setValue(audio_volume);

  view_.frequencySlider->setValue(tone_freq);
  view_.frequencySpinBox->setValue(tone_freq);
}

void AudioSettingsController::ConnectSignalsToSlots() noexcept {
  connect(view_.waveType, QOverload<int>::of(&QComboBox::activated),
          [this](const int index) {
            AppSettingsModel().SetAudioToneType(static_cast<ToneType>(index));

            emit ToneTypeChanged(
                view_.soundCards->itemData(index).value<ToneType>());
          });

  connect(view_.frequencySlider, &QSlider::sliderMoved,
          [this](const int value) {
            view_.frequencySpinBox->setValue(value);

            AppSettingsModel().SetAudioToneFrequency(value);
            emit FrequencyChanged(value);
          });

  connect(view_.volumeSlider, &QSlider::sliderMoved, [this](const int value) {
    view_.volumeSpinBox->setValue(value);

    AppSettingsModel().SetAudioVolume(value);
    emit VolumeChanged(value);
  });

  connect(view_.frequencySpinBox, &QSpinBox::valueChanged,
          [this](const int value) {
            view_.frequencySlider->setValue(value);

            AppSettingsModel().SetAudioToneFrequency(value);
            emit FrequencyChanged(value);
          });

  connect(view_.volumeSpinBox, &QSpinBox::valueChanged,
          [this](const int value) {
            view_.volumeSlider->setValue(value);

            AppSettingsModel().SetAudioVolume(value);
            emit VolumeChanged(value);
          });

  connect(view_.soundCards, QOverload<int>::of(&QComboBox::activated),
          [this](const int index) {
            const auto audio_device =
                view_.soundCards->itemData(index).value<QAudioDevice>();

            AppSettingsModel().SetAudioDeviceID(audio_device.id());
            emit AudioDeviceChanged(audio_device);
          });
}