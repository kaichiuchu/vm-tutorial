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

enum class ToneType { kSineWave };

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
  /// \param volume The new audio output volume. This value cannot exceed 100.
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

  auto ConfigureSoundBuffer(const QAudioFormat& format,
                            double duration) noexcept -> SoundBufferInfo;

 private:
  void GenerateSineWave(double duration) noexcept;

  int tone_freq_;
  QByteArray sound_buffer_;
  ToneType tone_type_;
  QAudioSink* audio_output_ = nullptr;
  QMediaDevices* media_devices_;
  QIODevice* audio_io_;
};