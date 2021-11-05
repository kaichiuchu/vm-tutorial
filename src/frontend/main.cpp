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

#include <QApplication>
#include <QSurfaceFormat>

#include "vm_tutorial_app.h"

/// Program entry point.
///
/// \param argc The number of arguments passed to the program from the
/// environment in which the program is run.
///
/// \param argv The arguments passed to the program from the environment in
/// which the program is run.
auto main(int argc, char* argv[]) -> int {
  /// Per Qt documentation at https://doc.qt.io/qt-6/qopenglwidget.html:
  ///
  /// Note: Calling \ref QSurfaceFormat::setDefaultFormat() before constructing
  /// the QApplication instance is mandatory on some platforms (for example,
  /// macOS) when an OpenGL core profile context is requested. This is to ensure
  /// that resource sharing between contexts stays functional as all internal
  /// contexts are created using the correct version and profile.
  QSurfaceFormat default_surface_format;
  default_surface_format.setMajorVersion(4);
  default_surface_format.setMinorVersion(1);
  default_surface_format.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(default_surface_format);

  QApplication qt_app_instance{argc, argv};

  QApplication::setApplicationName("vm-tutorial");
  QApplication::setApplicationVersion("1.0.0");

  VMTutorialApplication app;
  return QApplication::exec();
}