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

#include "tone_generators/sine_wave.h"

SoundManager::SoundManager(QObject* parent_object) noexcept
    : QObject(parent_object), media_devices_(new QMediaDevices(this)) {
  SetAudioOutputDevice(media_devices_->defaultAudioOutput());
  SetToneType(ToneType::kSineWave);
  SetToneFrequency(500);
  SetVolume(100);
}

void SoundManager::PlayTone(const double duration) noexcept {
  if (audio_output_->volume() <= 0.0) {
    return;
  }

  if (audio_output_->state() == QAudio::StoppedState) {
    return;
  }

  tone_generator_->Generate(audio_output_->format(), duration, tone_freq_);

  auto len = audio_output_->bytesFree();
  QByteArray buffer(len, 0);

  len = tone_generator_->read(buffer.data(), len);

  if (len) {
    audio_io_->write(buffer.data(), len);
  }
}

void SoundManager::SetVolume(const unsigned int volume) noexcept {
  if (audio_output_) {
    const auto linear_volume = QAudio::convertVolume(
        volume / qreal(100), QAudio::LogarithmicVolumeScale,
        QAudio::LinearVolumeScale);

    audio_output_->setVolume(linear_volume);
  }
}

void SoundManager::SetToneType(const ToneType tone_type) noexcept {
  switch (tone_type) {
    case ToneType::kSineWave:
      tone_generator_ = new SineWaveGenerator(this);
      break;

    default:
      break;
  }
  tone_generator_->start();
}

void SoundManager::SetToneFrequency(const int tone_freq) noexcept {
  tone_freq_ = tone_freq;
}

void SoundManager::SetAudioOutputDevice(
    const QAudioDevice& audio_device) noexcept {
  const auto audio_device_format = audio_device.preferredFormat();

  audio_output_ = new QAudioSink(audio_device, audio_device_format);
  audio_io_ = audio_output_->start();
}

auto SoundManager::GetAudioOutputDevices() const noexcept
    -> QList<QAudioDevice> {
  return media_devices_->audioOutputs();
}