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

#include <QSettings>
#include <unordered_map>

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
  auto KeyBindingExists(const int physical_key) noexcept -> bool;

  /// Returns virtual machine key associations.
  auto GetVMKeyBindings() noexcept -> VMKeyBindings;

  /// Tries to find the virtual machine key the physical key corresponds to.
  ///
  /// \param physical_key The physical key that the user pressed.
  ///
  /// \returns The CHIP-8 key that the physical key corresponds to, if any.
  std::optional<chip8::Key> GetVMKeyBinding(const int physical_key) noexcept;

  void SetVMKeyBinding(const chip8::Key chip8_key,
                       const int physical_key) noexcept;

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