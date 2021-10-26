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

#include "sound_manager.h"

#include "models/app_settings.h"

SoundManager::SoundManager(QObject *parent_object) noexcept
    : QObject(parent_object), media_devices_(new QMediaDevices(this)) {
  SetupFromAppSettings();
  ConnectSignalsToSlots();
}

void SoundManager::PlayTone(const double duration) noexcept {
  if (audio_output_->volume() <= 0.0) {
    return;
  }

  switch (tone_type_) {
    case ToneType::kSineWave:
      GenerateSineWave(duration);
      break;
  }
  audio_io_->write(sound_buffer_.data(), sound_buffer_.size());
}

void SoundManager::SetVolume(const unsigned int volume) noexcept {
  const auto linear_volume =
      QAudio::convertVolume(volume / qreal(100), QAudio::LogarithmicVolumeScale,
                            QAudio::LinearVolumeScale);

  audio_output_->setVolume(linear_volume);
}

void SoundManager::SetAudioOutputDevice(
    const QAudioDevice &audio_device) noexcept {
  const auto audio_device_format = audio_device.preferredFormat();

  audio_output_ = new QAudioSink(audio_device, audio_device_format);
  audio_io_ = audio_output_->start();
}

auto SoundManager::GetAudioOutputDevices() const noexcept
    -> QList<QAudioDevice> {
  return media_devices_->audioOutputs();
}

auto SoundManager::ConfigureSoundBuffer(const QAudioFormat &format,
                                        const double duration) noexcept
    -> SoundBufferInfo {
  // The bytes per sample is the number of bytes required to represent one
  // sample.
  const auto bytes_per_sample = format.bytesPerSample();

  // The \ref QAudioFormat::bytesForDuration() method takes microseconds, and
  // the duration of a tone is returned as milliseconds from the core, so we
  // have to convert from milliseconds to microseconds. It returns the number of
  // bytes necessary to play a tone of `duration` length.
  const auto bytes_for_duration = format.bytesForDuration(duration * 1000);

  // Adjust the size of the audio buffer.
  sound_buffer_.resize(bytes_for_duration);

  return {bytes_per_sample, bytes_for_duration};
}

void SoundManager::ConnectSignalsToSlots() noexcept {
  connect(media_devices_, &QMediaDevices::audioOutputsChanged, this,
          [this]() { emit AudioDevicesUpdated(); });
}

void SoundManager::SetupFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  const auto audio_output_devices = media_devices_->audioOutputs();

  const auto audio_device_id = app_settings.GetAudioDeviceID();

  const auto result =
      std::find_if(audio_output_devices.cbegin(), audio_output_devices.cend(),
                   [audio_device_id](const QAudioDevice &audio_device) {
                     return audio_device.id() == audio_device_id;
                   });

  if (result != audio_output_devices.cend()) {
    SetAudioOutputDevice(*result);
  } else {
    const auto default_audio_device = media_devices_->defaultAudioOutput();

    if (!default_audio_device.isNull()) {
      SetAudioOutputDevice(default_audio_device);
    } else {
      // No audio devices on the system; stop.
      return;
    }
  }

  tone_freq_ = app_settings.GetAudioToneFrequency();
  tone_type_ = app_settings.GetAudioToneType();
  SetVolume(app_settings.GetAudioVolume());
}

void SoundManager::GenerateSineWave(double duration) noexcept {
  const auto format = audio_output_->format();

  auto [bytes_per_sample, bytes_for_duration] =
      ConfigureSoundBuffer(format, duration);

  auto ptr = reinterpret_cast<unsigned char *>(sound_buffer_.data());
  auto sample_index = 0;

  const auto sample_rate = format.sampleRate();

  while (bytes_for_duration) {
    const auto amplitude =
        std::sin(2 * M_PI * tone_freq_ * qreal(sample_index++ % sample_rate) /
                 sample_rate);

    for (auto i = 0; i < format.channelCount(); ++i) {
      switch (format.sampleFormat()) {
        case QAudioFormat::UInt8:
          *reinterpret_cast<uint8_t *>(ptr) =
              static_cast<uint8_t>((1.0 + amplitude) / 2 * 255);
          break;

        case QAudioFormat::Int16:
          *reinterpret_cast<int16_t *>(ptr) =
              static_cast<int16_t>(amplitude * 32767);
          break;

        case QAudioFormat::Int32:
          *reinterpret_cast<int32_t *>(ptr) = static_cast<int32_t>(
              amplitude * std::numeric_limits<int32_t>::max());
          break;

        case QAudioFormat::Float:
          *reinterpret_cast<float *>(ptr) = amplitude;
          break;

        default:
          break;
      }

      ptr += bytes_per_sample;
      bytes_for_duration -= bytes_per_sample;
    }
  }
}