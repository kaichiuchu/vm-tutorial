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

#include <core/spec.h>

#include <QAudioDevice>
#include <QSettings>
#include <unordered_map>

#include "types.h"

/// This class handles the application settings. This is not a model as it
/// relates to Qt's model/view architecture.
class AppSettingsModel : public QSettings {
  Q_OBJECT

 public:
  using VMKeyBindings = std::unordered_map<QString, int>;

  explicit AppSettingsModel(QObject* parent_object = nullptr) noexcept;

  /// Checks to see if a physical key is bound to either an application setting
  /// or CHIP-8 key.
  ///
  /// \param physical_key The scancode of the key to check.
  ///
  /// \returns true if the key is already bound, or false otherwise.
  auto KeyBindingExists(int physical_key) noexcept -> bool;

  /// Returns virtual machine key associations.
  auto GetVMKeyBindings() noexcept -> VMKeyBindings;

  /// Tries to find the virtual machine key the physical key corresponds to.
  ///
  /// \param physical_key The physical key that the user pressed.
  ///
  /// \returns The CHIP-8 key that the physical key corresponds to, if any.
  auto GetVMKeyBinding(const int physical_key) noexcept
      -> std::optional<chip8::Key>;

  /// Tries to find the audio device to use by its ID.
  ///
  /// \returns The audio device ID within the configuration file. If the byte
  /// array is empty, the default audio output device should be used.
  auto GetAudioDeviceID() const noexcept -> QByteArray;

  /// Tries to find the frequency of the tone to generate within the
  /// configuration file.
  ///
  /// \returns The audio tone frequency within the configuration file, or 500 as
  /// a default value.
  auto GetAudioToneFrequency() const noexcept -> int;

  /// Tries to find the audio volume within the configuration file.
  ///
  /// \returns The audio volume within the configuration file, or 100 as a
  /// default value.
  auto GetAudioVolume() const noexcept -> int;

  /// Tries to find the audio tone type within the configuration file.
  ///
  /// \returns The audio tone type specified within the configuration file, or
  /// \ref ToneType::kSineWave by default.
  auto GetAudioToneType() const noexcept -> ToneType;

  /// Tries to find the default path of guest program files within the
  /// configuration file.
  ///
  /// \returns The default path of guest program files, or the current working
  /// directory by default.
  auto GetProgramFilesPath() const noexcept -> QString;

  /// Tries to find if bilinear filtering is enabled according to the
  /// configuration file.
  ///
  /// \returns true if bilinear filtering is enabled, or false otherwise. If the
  /// bilinear filtering state is not within the configuration file, false is
  /// returned by default.
  auto BilinearFilteringEnabled() const noexcept -> bool;

  /// Sets the default path of the guest program files.
  ///
  /// \param path The new default path of the guest program files.
  void SetProgramFilesPath(const QString& path) noexcept;

  /// Sets the state of bilinear filtering.
  ///
  /// \param enabled true if bilinear filtering should be enabled, or false
  /// otherwise.
  void SetBilinearFiltering(bool enabled) noexcept;

  /// Sets the frequency of the tone to generate.
  ///
  /// \param freq The frequency of the audio tone to generate.
  void SetAudioToneFrequency(unsigned int freq) noexcept;

  /// Sets the volume of the audio output.
  ///
  /// \param value The volume of the audio output. If this value exceeds 100,
  /// the result is 100.
  void SetAudioVolume(unsigned int value) noexcept;

  /// Sets the type of audio tone.
  ///
  /// \param tone_type The type of audio tone to use.
  void SetAudioToneType(ToneType tone_type) noexcept;

  /// Sets the audio device ID.
  ///
  /// \param audio_device_id The ID of the audio device to use.
  void SetAudioDeviceID(const QByteArray& audio_device_id) noexcept;

  /// Sets the virtual machine key the physical key corresponds to.
  void SetVMKeyBinding(chip8::Key chip8_key, int physical_key) noexcept;

 private:
  /// Maps the names of virtual machine key binding sections to CHIP-8 keys.
  const QMap<QString, chip8::Key> chip8_key_mapping_ = {
      {"key_0", chip8::Key::k0}, {"key_1", chip8::Key::k1},
      {"key_2", chip8::Key::k2}, {"key_3", chip8::Key::k3},
      {"key_4", chip8::Key::k4}, {"key_5", chip8::Key::k5},
      {"key_6", chip8::Key::k6}, {"key_7", chip8::Key::k7},
      {"key_8", chip8::Key::k8}, {"key_9", chip8::Key::k9},
      {"key_A", chip8::Key::kA}, {"key_B", chip8::Key::kB},
      {"key_C", chip8::Key::kC}, {"key_D", chip8::Key::kD},
      {"key_E", chip8::Key::kE}, {"key_F", chip8::Key::kF}};
};