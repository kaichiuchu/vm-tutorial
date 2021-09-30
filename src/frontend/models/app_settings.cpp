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

#include "app_settings.h"

AppSettingsModel::AppSettingsModel(QObject* parent_object) noexcept
    : QSettings("vm-tutorial.ini", QSettings::IniFormat, parent_object) {}

auto AppSettingsModel::KeyBindingExists(const int physical_key) noexcept
    -> bool {
  if (GetVMKeyBinding(physical_key)) {
    return true;
  }
  return false;
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

std::optional<chip8::Key> AppSettingsModel::GetVMKeyBinding(
    const int physical_key) noexcept {
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

void AppSettingsModel::SetVMKeyBinding(const chip8::Key chip8_key,
                                       const int physical_key) noexcept {
  const auto key_string =
      QString{"vm_keys/key_%1"}.arg(QString::number(chip8_key, 16).toUpper());

  setValue(key_string, physical_key);
}