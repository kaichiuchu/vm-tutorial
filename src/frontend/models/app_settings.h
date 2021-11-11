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

#include <core/spec.h>

#include <QAudioDevice>
#include <QColor>
#include <QFont>
#include <QSettings>
#include <unordered_map>

#include "types.h"

/// This class handles the application settings. This is not a model as it
/// relates to Qt's model/view architecture.
class AppSettingsModel : public QSettings {
  Q_OBJECT

 public:
  // Type alias for the virtual machine key bindings.
  using VMKeyBindings = std::map<QString, int>;

  /// Constructs the application settings model.
  ///
  /// \param parent_object The parent widget of which this class is a child of
  /// it.
  explicit AppSettingsModel(QObject* parent_object = nullptr) noexcept;

  /// Checks to see if a physical key is bound to either an application setting
  /// or CHIP-8 key.
  ///
  /// \param physical_key The scancode of the key to check.
  ///
  /// \returns \p true if the key is already bound, or \p false otherwise.
  auto KeyBindingExists(int physical_key) noexcept -> bool;

  /// Gets the mapping of virtual machine key bindings.
  ///
  /// \returns The virtual machine key bindings.
  auto GetVMKeyBindings() noexcept -> VMKeyBindings;

  /// Tries to find the virtual machine key the physical key corresponds to.
  ///
  /// \param physical_key The scancode of the key that the user pressed.
  ///
  /// \returns The CHIP-8 key that the physical key corresponds to, if any.
  auto GetVMKeyBinding(const int physical_key) noexcept
      -> std::optional<chip8::Key>;

  /// Tries to find the audio device to use by its ID.
  ///
  /// \returns The audio device ID within the configuration file. If the byte
  /// array is empty, the default audio output device on the system should be
  /// used, if any.
  auto GetAudioDeviceID() const noexcept -> QByteArray;

  /// Tries to find the frequency of the tone to generate within the
  /// configuration file.
  ///
  /// \returns The audio tone frequency within the configuration file, or \p 500
  /// as a default value.
  auto GetAudioToneFrequency() const noexcept -> int;

  /// Tries to find the audio volume within the configuration file.
  ///
  /// \returns The audio volume within the configuration file, or \p 100 as a
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
  /// \returns \p true if bilinear filtering is enabled, or \p false otherwise.
  /// If the bilinear filtering state is not within the configuration file, \p
  /// false is returned by default.
  auto BilinearFilteringEnabled() const noexcept -> bool;

  /// Tries to find the color a level belongs to.
  ///
  /// \param level The name of the level to search for.
  ///
  /// \returns The color that the log level is associated with, if any. If no
  /// color exists, the default text color on the system should be used.
  auto GetLogLevelColor(const QString& level) const noexcept -> QColor;

  /// Tries to find the font of the logger.
  ///
  /// \returns The font of the logger within the configuration file, if any. If
  /// no font exists, the default font on the system should be used.
  auto GetLogFont() noexcept -> std::optional<QFont>;

  /// Tries to find the desired frame rate for the virtual machine.
  ///
  /// \returns The desired frame rate of the virtual machine, if any, or \p 60.0
  /// by default.
  auto GetMachineFrameRate() const noexcept -> double;

  /// Tries to find the desired number of instructions per second for the
  /// virtual machine.
  ///
  /// \returns The desired number of instructions per second of the virtual
  /// machine, if any, or \p 500 by default.
  auto GetMachineInstructionsPerSecond() const noexcept -> int;

  /// Tries to determine the font of the memory view.
  ///
  /// \returns The font of the memory view. If no font was set, a default font
  /// of Consolas is selected.
  auto GetMemoryViewFont() const noexcept -> std::optional<QFont>;

  /// Sets frame rate within the configuration file.
  ///
  /// \param frame_rate The new desired frame rate of the virtual machine.
  void SetMachineFrameRate(double frame_rate) noexcept;

  /// Sets the number of instructions per second to execute within the
  /// configuration file.
  ///
  /// \param instructions_per_second The new desired number of instructions to
  /// execute per second.
  void SetMachineInstructionsPerSecond(int instructions_per_second) noexcept;

  /// Sets the default path of the guest program files within the configuration
  /// file.
  ///
  /// \param path The new default path of the guest program files.
  void SetProgramFilesPath(const QString& path) noexcept;

  /// Sets the state of bilinear filtering within the configuration file.
  ///
  /// \param enabled \p true if bilinear filtering should be enabled, or \p
  /// false otherwise.
  void SetBilinearFiltering(bool enabled) noexcept;

  /// Sets the frequency of the tone to generate within the configuration file.
  ///
  /// \param freq The frequency of the audio tone to generate.
  void SetAudioToneFrequency(unsigned int freq) noexcept;

  /// Sets the volume of the audio output within the configuration file.
  ///
  /// \param value The volume of the audio output.
  void SetAudioVolume(unsigned int value) noexcept;

  /// Sets the type of audio tone within the configuration file.
  ///
  /// \param tone_type The type of audio tone to use.
  void SetAudioToneType(ToneType tone_type) noexcept;

  /// Sets the audio device ID within the configuration file.
  ///
  /// \param audio_device_id The ID of the audio device to use.
  void SetAudioDeviceID(const QByteArray& audio_device_id) noexcept;

  /// Sets the virtual machine key the physical key corresponds to within the
  /// configuration file.
  ///
  /// \param chip8_key The CHIP-8 key to associate with the physical key.
  /// \param physical_key The scancode of the physical key to associate with the
  /// CHIP-8 key.
  void SetVMKeyBinding(chip8::Key chip8_key, int physical_key) noexcept;

  /// Sets the font of the logger within the configuration file.
  ///
  /// \param font The log font to use.
  void SetLogFont(const QFont& font) noexcept;

  /// Sets the color of a log level within the configuration file.
  ///
  /// \param level_name The name of a level to associate a color with.
  /// \param color The color to associate with the level.
  void SetLogLevelColor(const QString& level_name,
                        const QColor& color) noexcept;

 private:
  /// Maps the names of virtual machine key binding sections to CHIP-8 keys.
  std::map<QString, chip8::Key> chip8_key_mapping_ = {
      {"key_0", chip8::Key::k0}, {"key_1", chip8::Key::k1},
      {"key_2", chip8::Key::k2}, {"key_3", chip8::Key::k3},
      {"key_4", chip8::Key::k4}, {"key_5", chip8::Key::k5},
      {"key_6", chip8::Key::k6}, {"key_7", chip8::Key::k7},
      {"key_8", chip8::Key::k8}, {"key_9", chip8::Key::k9},
      {"key_A", chip8::Key::kA}, {"key_B", chip8::Key::kB},
      {"key_C", chip8::Key::kC}, {"key_D", chip8::Key::kD},
      {"key_E", chip8::Key::kE}, {"key_F", chip8::Key::kF}};
};
