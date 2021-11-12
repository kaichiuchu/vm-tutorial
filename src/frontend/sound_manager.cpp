// vm-tutorial - Virtual machine tutorial targeting CHIP-8
//
// Written in 2021 by Michael Rodriguez aka kaichiuchu <mike@kaichiuchu.dev>
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#include "sound_manager.h"

#include <QDebug>

#include "models/app_settings.h"

std::optional<SoundManager*> SoundManager::Initialize(QObject* parent_object,
                                                      QString& error) noexcept {
  if (SDL_Init(SDL_INIT_AUDIO) == 0) {
    return new SoundManager{parent_object};
  }
  error = SDL_GetError();
  return std::nullopt;
}

SoundManager::SoundManager(QObject* parent_object) noexcept
    : QObject(parent_object) {
  SetupFromAppSettings();
}

void SoundManager::PlayTone(const double duration) noexcept {
  switch (tone_type_) {
    case ToneType::kSineWave:
      GenerateSineWave(duration, GenerationOptions::kDoNotUseCopySign);
      break;

    case ToneType::kSawtooth:
      GenerateSawtoothWave(duration);
      break;

    case ToneType::kSquare:
      GenerateSineWave(duration, GenerationOptions::kUseCopySign);
      break;

    case ToneType::kTriangle:
      GenerateTriangleWave(duration);
      break;
  }
}

void SoundManager::SetVolume(const unsigned int volume) noexcept {}

void SoundManager::SetAudioOutputDevice(
    const QString& audio_output_device) noexcept {
  SDL_AudioSpec audio_spec;
  memset(&audio_spec, 0, sizeof(SDL_AudioSpec));

  audio_spec.freq = kSampleRate;
  audio_spec.format = AUDIO_S16SYS;
  audio_spec.channels = 1;

  const auto audio_output_device_name =
      audio_output_device.isEmpty() ? nullptr
                                    : audio_output_device.toLocal8Bit().data();

  const auto audio_device_id_new =
      SDL_OpenAudioDevice(audio_output_device_name, 0, &audio_spec, nullptr, 0);

  if (audio_device_id_new == 0) {
    // We won't be able to do anything now, but we can at least still preserve
    // the current audio output device if there is one.
    emit ErrorEncountered(SDL_GetError());
    return;
  }

  SDL_CloseAudioDevice(audio_output_device_);

  audio_output_device_ = audio_device_id_new;

  SDL_PauseAudioDevice(audio_output_device_, 0);
}

auto SoundManager::GetAudioOutputDevices() noexcept -> std::vector<QString> {
  const auto audio_output_device_count = SDL_GetNumAudioDevices(0);

  // An explicit list cannot be determined.
  if (audio_output_device_count == -1) {
    return {};
  }

  std::vector<QString> audio_output_devices;

  for (auto audio_device_output_id = 0;
       audio_device_output_id < audio_output_device_count;
       ++audio_device_output_id) {
    const auto audio_output_device_name =
        SDL_GetAudioDeviceName(audio_device_output_id, 0);

    if (audio_output_device_name == nullptr) {
      emit ErrorEncountered(SDL_GetError());

      // We don't want to stop trying to get audio device names just because one
      // failed to be retrieved.
      continue;
    }
    audio_output_devices.push_back(audio_output_device_name);
  }
  return audio_output_devices;
}

void SoundManager::SetupFromAppSettings() noexcept {
  AppSettingsModel app_settings;

  SetAudioOutputDevice(app_settings.GetAudioDeviceID());

  tone_freq_ = app_settings.GetAudioToneFrequency();
  tone_type_ = static_cast<ToneType>(app_settings.GetAudioToneType());
  SetVolume(app_settings.GetAudioVolume());
}

void SoundManager::GenerateSineWave(const double duration,
                                    const GenerationOptions options) noexcept {
  const auto output_sample_count =
      static_cast<int>((duration / 1000) * kSampleRate);

  for (auto sample_num = 0; sample_num < output_sample_count; ++sample_num) {
    const auto t = sample_num / static_cast<double>(kSampleRate);
    auto amplitude = std::sin(M_PI * 2 * t * tone_freq_);

    if (options == GenerationOptions::kUseCopySign) {
      amplitude = std::copysign(1.0, amplitude);
    }

    const auto sample =
        static_cast<int16_t>(amplitude * std::numeric_limits<int16_t>::max());

    const auto return_code =
        SDL_QueueAudio(audio_output_device_, &sample, sizeof(int16_t));

    if (return_code < 0) {
      emit ErrorEncountered(SDL_GetError());

      // We don't want to stop generating samples just because one sample
      // wasn't queued properly. The audio may be choppy, but it's better than
      // nothing.
    }
  }
}

void SoundManager::GenerateSawtoothWave(const double duration) noexcept {
  const auto output_sample_count =
      static_cast<int>((duration / 1000) * kSampleRate);

  for (auto sample_num = 0; sample_num < output_sample_count; ++sample_num) {
    const auto t = sample_num / static_cast<double>(kSampleRate);

    const auto sample = static_cast<int16_t>(
        (-2 / M_PI) * std::atan(1 / std::tan(tone_freq_ * M_PI * t)) *
        std::numeric_limits<int16_t>::max());

    const auto return_code =
        SDL_QueueAudio(audio_output_device_, &sample, sizeof(int16_t));

    if (return_code < 0) {
      emit ErrorEncountered(SDL_GetError());

      // We don't want to stop generating samples just because one sample
      // wasn't queued properly. The audio may be choppy as a result, but it's
      // better than nothing.
    }
  }
}

void SoundManager::GenerateTriangleWave(const double duration) noexcept {
  const auto output_sample_count =
      static_cast<int>((duration / 1000) * kSampleRate);

  for (auto sample_num = 0; sample_num < output_sample_count; ++sample_num) {
    const auto t = sample_num / static_cast<double>(kSampleRate);

    const auto sample = static_cast<int16_t>(
        (2 / M_PI) * std::asin(std::sin(tone_freq_ * M_PI * 2 * t)) *
        std::numeric_limits<int16_t>::max());

    const auto return_code =
        SDL_QueueAudio(audio_output_device_, &sample, sizeof(int16_t));

    if (return_code < 0) {
      emit ErrorEncountered(SDL_GetError());

      // We don't want to stop generating samples just because one sample
      // wasn't queued properly. The audio may be choppy as a result, but it's
      // better than nothing.
    }
  }
}
