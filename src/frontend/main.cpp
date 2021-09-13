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

#include <QApplication>

#include "vm_tutorial_app.h"

/// Program entry point.
///
/// \param argc The number of arguments passed to the program from the
/// environment in which the program is run.
///
/// \param argv The arguments passed to the program from the environment in
/// which the program is run.
auto main(int argc, char* argv[]) -> int {
  QApplication qt_app_instance(argc, argv);

  qt_app_instance.setApplicationName("vm-tutorial");
  qt_app_instance.setApplicationVersion("1.0.0");

  VMTutorialApplication app;
  return qt_app_instance.exec();
}