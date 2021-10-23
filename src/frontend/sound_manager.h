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

#include <QAudioSink>
#include <QMediaDevices>

#include "types.h"

/// This class handles the management of sound devices and tone generation.
class SoundManager : public QObject {
  Q_OBJECT

 public:
  using BytesPerSample = int;
  using BytesForDuration = int32_t;

  using SoundBufferInfo = std::pair<BytesPerSample, BytesForDuration>;

  explicit SoundManager(QObject* parent_object) noexcept;

  /// Plays a tone for the specified period.
  ///
  /// This method will do nothing if a tone generator was not specified, or if
  /// audio is muted.
  ///
  /// \param duration The length of the tone to generate, in milliseconds.
  void PlayTone(double duration) noexcept;

  /// Sets the audio output volume.
  ///
  /// \param volume The new audio output volume. This value cannot exceed 100
  /// and if any value over 100 is passed, the volume will be treated as 100.
  void SetVolume(unsigned int volume) noexcept;

  /// Sets the type of tone to generate.
  ///
  /// \param tone_type A tone type specified by the \ref ToneType enumeration.
  void SetToneType(ToneType tone_type) noexcept;

  /// Sets the frequency of the tone to be generated.
  ///
  /// \param tone_freq The frequency of the tone to be generated.
  void SetToneFrequency(int tone_freq) noexcept;

  /// Sets the audio output device to use.
  ///
  /// \param audio_device The audio device to send tone output to.
  void SetAudioOutputDevice(const QAudioDevice& audio_device) noexcept;

  /// Retrieves a list of audio outputs available on the system.
  ///
  /// \returns A list of available audio output devices.
  auto GetAudioOutputDevices() const noexcept -> QList<QAudioDevice>;

  /// Configures the sound buffer for new audio data.
  ///
  /// \param format The format of the audio device currently in use.
  /// \param duration The duration of the tone to generate.
  ///
  /// \returns The number of required bytes to represent one sample and the
  /// number of bytes required to represent the duration of the tone.
  auto ConfigureSoundBuffer(const QAudioFormat& format,
                            double duration) noexcept -> SoundBufferInfo;

 private:
  /// Connects signals to slots.
  void ConnectSignalsToSlots() noexcept;

  void SetupFromAppSettings() noexcept;

  /// Generates a sine wave.
  ///
  /// The frequency of the sine wave is governed by the last call to \ref
  /// SetToneFrequency().
  ///
  /// The sine wave data is stored into the \ref sound_buffer_ buffer.
  ///
  /// \param duration The duration of the sine wave.
  void GenerateSineWave(double duration) noexcept;

  /// The frequency of a generated tone as determined by the last call to \ref
  /// SetToneFrequency().
  int tone_freq_;

  /// The buffer which contains the generated sound wave data.
  QByteArray sound_buffer_;

  /// The type of tone to generate as determined by the last call to \ref
  /// SetToneType().
  ToneType tone_type_;

  /// The current interface to send audio data to an audio output device as
  /// determined by the last call to \ref SetAudioOutputDevice().
  QAudioSink* audio_output_ = nullptr;

  /// The interface to information about the audio devices available on the
  /// system.
  QMediaDevices* media_devices_;

  /// The I/O device used to transfer data to the audio output device as
  /// determined by the last call to \ref SetAudioOutputDevice().
  QIODevice* audio_io_;

 signals:
  void AudioDevicesUpdated();
};