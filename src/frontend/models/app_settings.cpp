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

#include "app_settings.h"

#include <QDir>
#include <QFontDatabase>

AppSettingsModel::AppSettingsModel(QObject* parent_object) noexcept
    : QSettings(QStringLiteral("vm-tutorial.ini"), QSettings::IniFormat,
                parent_object) {}

auto AppSettingsModel::GetAudioDeviceName() const noexcept -> QString {
  return value(QStringLiteral("audio/default_device")).toString();
}

auto AppSettingsModel::GetAudioToneFrequency() const noexcept -> int {
  return value(QStringLiteral("audio/tone_freq"), 500).toInt();
}

auto AppSettingsModel::GetAudioVolume() const noexcept -> int {
  return value(QStringLiteral("audio/volume"), 100).toInt();
}

auto AppSettingsModel::GetAudioToneType() const noexcept -> int {
  const auto default_wave = static_cast<int>(ToneType::kSineWave);
  return value(QStringLiteral("audio/tone_type"), default_wave).toInt();
}

auto AppSettingsModel::GetProgramFilesPath() const noexcept -> QString {
  return value(QStringLiteral("paths/program_files"), QDir::currentPath())
      .toString();
}

auto AppSettingsModel::GetLogLevelColor(const QString& level) const noexcept
    -> QColor {
  return value(QString{"logger/%1_level_color"}.arg(level), QColor(Qt::white))
      .value<QColor>();
}

auto AppSettingsModel::GetLogFont() noexcept -> std::optional<QFont> {
  const auto font = value(QStringLiteral("logger/font")).toString();

  if (font.isEmpty()) {
    return std::nullopt;
  }

  QFont f;
  f.fromString(font);

  return f;
}

auto AppSettingsModel::GetMachineFrameRate() const noexcept -> double {
  return value(QStringLiteral("machine/frame_rate"), 60.0).toDouble();
}

auto AppSettingsModel::GetMachineInstructionsPerSecond() const noexcept -> int {
  return value(QStringLiteral("machine/instructions_per_second"), 500).toInt();
}

auto AppSettingsModel::GetDebuggerFont() const noexcept -> QFont {
  const auto font = value(QStringLiteral("debugger/font")).toString();

  if (font.isEmpty()) {
#ifdef __WIN64
    QFont font;
    font.setFamily(QStringLiteral("Consolas"));
    font.setFixedPitch(true);
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(10);

    return font;
#else
    return QFontDatabase::systemFont(QFontDatabase::FixedFont);
#endif
  }

  QFont f;
  f.fromString(font);

  return f;
}

void AppSettingsModel::SetMachineFrameRate(double frame_rate) noexcept {
  setValue(QStringLiteral("machine/frame_rate"), frame_rate);
}

void AppSettingsModel::SetMachineInstructionsPerSecond(
    int instructions_per_second) noexcept {
  setValue(QStringLiteral("machine/instructions_per_second"),
           instructions_per_second);
}

void AppSettingsModel::SetProgramFilesPath(const QString& path) noexcept {
  setValue(QStringLiteral("paths/program_files"), path);
}

void AppSettingsModel::SetAudioToneFrequency(const unsigned int freq) noexcept {
  setValue(QStringLiteral("audio/tone_freq"), freq);
}

void AppSettingsModel::SetAudioVolume(const unsigned int value) noexcept {
  setValue(QStringLiteral("audio/volume"), value);
}

void AppSettingsModel::SetAudioToneType(const int tone_type) noexcept {
  setValue(QStringLiteral("audio/tone_type"), tone_type);
}

void AppSettingsModel::SetDefaultAudioDeviceName(
    const QString& audio_device_name) noexcept {
  setValue(QStringLiteral("audio/default_device"), audio_device_name);
}

void AppSettingsModel::SetLogFont(const QFont& font) noexcept {
  setValue(QStringLiteral("logger/font"), font.toString());
}

void AppSettingsModel::SetLogLevelColor(const QString& level_name,
                                        const QColor& color) noexcept {
  const auto level_str = QString{"logger/%1_level_color"}.arg(level_name);
  setValue(level_str, color.name());
}
