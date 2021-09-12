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

#include <QObject>

#include "controllers/main_window.h"
#include "controllers/settings_dialog.h"
#include "vm_thread.h"

class VMTutorialApplication : public QObject {
  Q_OBJECT

 public:
  VMTutorialApplication() noexcept;

 private:
  MainWindowController main_window_;
  SettingsDialogController settings_dialog_;
  VMThread vm_thread_;
};