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

SoundManager::SoundManager(QObject *parent_object) noexcept
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

  switch (tone_type_) {
    case ToneType::kSineWave:
      GenerateSineWave(duration);
      break;
  }
  audio_io_->write(sound_buffer_.data(), sound_buffer_.size());
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
  tone_type_ = tone_type;
}

void SoundManager::SetToneFrequency(const int tone_freq) noexcept {
  tone_freq_ = tone_freq;
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

  // The \ref QAudioFormat::bytesForDuration() method takes microseconds, so
  // we have to convert from milliseconds to microseconds. It returns the
  // number of bytes necessary to play a tone of `duration`.
  const auto bytes_for_duration = format.bytesForDuration(duration * 1000);

  // Adjust the size of the audio buffer.
  sound_buffer_.resize(bytes_for_duration);

  return {bytes_per_sample, bytes_for_duration};
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