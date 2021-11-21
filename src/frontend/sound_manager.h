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

#pragma once

#include <SDL2/SDL.h>

#include <QObject>
#include <optional>

#include "types.h"

/// This class handles the management of sound devices and tone generation.
class SoundManager : public QObject {
  Q_OBJECT

 public:
  ~SoundManager() noexcept;

  /// Attempts to initialize the sound manager.
  ///
  /// This is required because SDL initialization has the potential to fail, and
  /// in such case the state of the sound manager would be invalid. It's not a
  /// fatal program error because no one *needs* sound. If an initialization
  /// failure occurs, the user can restart the program or attempt to initialize
  /// sound through the settings dialog.
  ///
  /// \param parent_object The parent object of which this class is a child of
  /// it.
  ///
  /// \param error The error message from SDL, if any.
  ///
  /// \returns Either an empty object, or a SoundManager object.
  static std::optional<SoundManager*> Initialize(QObject* parent_object,
                                                 QString& error) noexcept;

  /// Plays a tone for the specified period.
  ///
  /// This method will do nothing if the audio is muted.
  ///
  /// \param duration The length of the tone to generate, in milliseconds.
  void PlayTone(double duration) noexcept;

  /// Sets the audio output volume.
  ///
  /// \param volume The new audio output volume. This value cannot exceed 100
  /// and if any value over 100 is passed, the volume will be treated as 100.
  void SetVolume(unsigned int volume) noexcept;

  /// Sets the audio output device to use.
  ///
  /// \param audio_device The audio device to send tone output to.
  void SetAudioOutputDevice(const QString& audio_device) noexcept;

  /// Retrieves a list of audio outputs available on the system.
  ///
  /// \returns A list of available audio output devices.
  auto GetAudioOutputDevices() noexcept -> std::vector<QString>;

  /// The frequency of a generated tone.
  unsigned int tone_freq_;

  /// The type of tone to generate.
  ToneType tone_type_;

 private:
  /// Constructs the sound manager.
  ///
  /// \param parent_widget The parent object of which this class is a child of
  /// it.
  explicit SoundManager(QObject* parent_object) noexcept;

  /// Configures the sound manager based on the current application settings.
  void SetupFromAppSettings() noexcept;

  /// The current interface to send audio data to an audio output device as
  /// determined by the last call to \ref SetAudioOutputDevice().
  SDL_AudioDeviceID audio_output_device_;

  /// The default sample rate.
  const unsigned int kSampleRate = 44100;

 signals:
  /// Emitted when an internal error has been encountered.
  void ErrorEncountered(const QString& error_str);

  /// Emitted when the system declares a new audio output device or removes
  /// one.
  void AudioDevicesUpdated();
};
