# vm-tutorial - Virtual machine tutorial targeting CHIP-8
#
# Written in 2021 by kaichiuchu <kaichiuchu@protonmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along
# with this software. If not, see
# <http://creativecommons.org/publicdomain/zero/1.0/>.

find_package(Qt6 COMPONENTS OpenGLWidgets Widgets REQUIRED)
set(CMAKE_AUTOUIC_SEARCH_PATHS views)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRCS main.cpp renderer.cpp sound_manager.cpp vm_thread.cpp vm_tutorial_app.cpp)
set(HDRS renderer.h sound_manager.h vm_thread.h vm_tutorial_app.h)

set(CONTROLLER_SRCS controllers/logger_window.cpp controllers/main_window.cpp controllers/settings_dialog.cpp)
set(CONTROLLER_HDRS controllers/logger_window.h controllers/main_window.h controllers/settings_dialog.h)

set(VIEWS_FILES views/logger_window.ui views/main_window.ui views/settings_dialog.ui)

add_executable(VMTutorialFrontend ${SRCS}
                                  ${HDRS}
                                  ${CONTROLLER_SRCS}
                                  ${CONTROLLER_HDRS}
                                  ${VIEWS_FILES})

target_link_libraries(VMTutorialFrontend core Qt6::OpenGLWidgets Qt6::Widgets)
target_include_directories(VMTutorialFrontend PRIVATE ../core/src/public .)