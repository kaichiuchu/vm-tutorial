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

#include <QColor>
#include <QFont>
#include <QSettings>

#include "types.h"

/// This class handles the application settings. This is not a model as it
/// relates to Qt's model/view architecture.
class AppSettingsModel : public QSettings {
  Q_OBJECT

 public:
  /// Constructs the application settings model.
  ///
  /// \param parent_object The parent widget of which this class is a child of
  /// it.
  explicit AppSettingsModel(QObject* parent_object = nullptr) noexcept;

  /// Tries to find the audio device to use by its name.
  ///
  /// \returns The audio device name within the configuration file. If the
  /// string is empty, the default audio output device on the system should be
  /// used, if any.
  auto GetAudioDeviceName() const noexcept -> QString;

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
  /// \ref ToneType::kSineWave as an \p int by default.
  auto GetAudioToneType() const noexcept -> int;

  /// Tries to find the default path of guest program files within the
  /// configuration file.
  ///
  /// \returns The default path of guest program files, or the current working
  /// directory by default.
  auto GetProgramFilesPath() const noexcept -> QString;

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

  /// Tries to determine the font of the debugger.
  ///
  /// \returns The font of the debugger. If no font was set, a default font is
  /// returned depending on the operating system:
  ///
  /// For Windows, a font of `Consolas 10pt` is returned. For all other systems,
  /// the default fixed font of the system is used.
  auto GetDebuggerFont() const noexcept -> QFont;

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
  /// \param tone_type The type of audio tone to use. This is one of the
  /// `ToneType` enums casted to an `int`.
  void SetAudioToneType(int tone_type) noexcept;

  /// Sets the default audio device name within the configuration file.
  ///
  /// \param audio_device_name The name of the audio device.
  void SetDefaultAudioDeviceName(const QString& audio_device_name) noexcept;

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
};
