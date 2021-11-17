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
    : QSettings("vm-tutorial.ini", QSettings::IniFormat, parent_object) {}

auto AppSettingsModel::KeyBindingExists(const int physical_key) noexcept
    -> bool {
  return GetVMKeyBinding(physical_key).has_value();
}

auto AppSettingsModel::GetVMKeyBindings() noexcept -> VMKeyBindings {
  VMKeyBindings bindings;

  beginGroup("vm_keys");

  for (const auto& key : childKeys()) {
    bindings[key] = value(key).toInt();
  }
  endGroup();
  return bindings;
}

auto AppSettingsModel::GetVMKeyBinding(const int physical_key) noexcept
    -> std::optional<chip8::Key> {
  // We call \ref QSettings::beginGroup() here so we can grab all of the child
  // keys of the virtual machine key binding group without the group name in the
  // result of \ref QSettings::childKeys().
  beginGroup("vm_keys");

  // Grab all of the keys within the virual machine key binding group, and
  // iterate over them one at a time.
  for (const auto& key : childKeys()) {
    // Grab the value that the key has.
    const auto result = value(key).toInt();

    // The key that was pressed corresponds to a virtual machine key binding.
    if (result == physical_key) {
      // Restore the original group to what it was before we called \ref
      // QSettings::beginGroup().
      endGroup();

      // Now return the CHIP-8 key that the virtual machine key binding section
      // name refers to.
      return chip8_key_mapping_[key];
    }
  }

  // Key wasn't found, restore the original group to what it was before we
  // called \ref QSettings::beginGroup().
  endGroup();
  return std::nullopt;
}

auto AppSettingsModel::GetAudioDeviceID() const noexcept -> QByteArray {
  return value("audio/default_device").toByteArray();
}

auto AppSettingsModel::GetAudioToneFrequency() const noexcept -> int {
  return value("audio/tone_freq", 500).toInt();
}

auto AppSettingsModel::GetAudioVolume() const noexcept -> int {
  return value("audio/volume", 100).toInt();
}

auto AppSettingsModel::GetAudioToneType() const noexcept -> int {
  const auto default_wave = static_cast<int>(ToneType::kSineWave);
  return value("audio/tone_type", default_wave).toInt();
}

auto AppSettingsModel::GetProgramFilesPath() const noexcept -> QString {
  return value("paths/program_files", QDir::currentPath()).toString();
}

auto AppSettingsModel::BilinearFilteringEnabled() const noexcept -> bool {
  return value("graphics/bilinear_filtering", false).toBool();
}

auto AppSettingsModel::GetLogLevelColor(const QString& level) const noexcept
    -> QColor {
  return value(QString{"logger/%1_level_color"}.arg(level), QColor(Qt::white))
      .value<QColor>();
}

auto AppSettingsModel::GetLogFont() noexcept -> std::optional<QFont> {
  const auto font = value("logger/font").toString();

  if (font.isEmpty()) {
    return std::nullopt;
  }

  QFont f;
  f.fromString(font);

  return f;
}

auto AppSettingsModel::GetMachineFrameRate() const noexcept -> double {
  return value("machine/frame_rate", 60.0).toDouble();
}

auto AppSettingsModel::GetMachineInstructionsPerSecond() const noexcept -> int {
  return value("machine/instructions_per_second", 500).toInt();
}

auto AppSettingsModel::GetDebuggerFont() const noexcept -> QFont {
  const auto font = value("debugger/font").toString();

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
  setValue("machine/frame_rate", frame_rate);
}

void AppSettingsModel::SetMachineInstructionsPerSecond(
    int instructions_per_second) noexcept {
  setValue("machine/instructions_per_second", instructions_per_second);
}

void AppSettingsModel::SetProgramFilesPath(const QString& path) noexcept {
  setValue("paths/program_files", path);
}

void AppSettingsModel::SetBilinearFiltering(const bool enabled) noexcept {
  setValue("graphics/bilinear_filtering", enabled);
}

void AppSettingsModel::SetAudioToneFrequency(const unsigned int freq) noexcept {
  setValue("audio/tone_freq", freq);
}

void AppSettingsModel::SetAudioVolume(const unsigned int value) noexcept {
  setValue("audio/volume", value);
}

void AppSettingsModel::SetAudioToneType(const int tone_type) noexcept {
  setValue("audio/tone_type", tone_type);
}

void AppSettingsModel::SetAudioDeviceID(
    const QString& audio_device_id) noexcept {
  setValue("audio/default_device", audio_device_id);
}

void AppSettingsModel::SetVMKeyBinding(const chip8::Key chip8_key,
                                       const int physical_key) noexcept {
  const auto key_string =
      QString{"vm_keys/key_%1"}.arg(QString::number(chip8_key, 16).toUpper());

  setValue(key_string, physical_key);
}

void AppSettingsModel::SetLogFont(const QFont& font) noexcept {
  setValue("logger/font", font.toString());
}

void AppSettingsModel::SetLogLevelColor(const QString& level_name,
                                        const QColor& color) noexcept {
  const auto level_str = QString{"logger/%1_level_color"}.arg(level_name);
  setValue(level_str, color.name());
}
