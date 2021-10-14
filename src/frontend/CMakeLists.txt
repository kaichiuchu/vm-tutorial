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

find_package(Qt6 COMPONENTS Multimedia OpenGLWidgets Widgets REQUIRED)
set(CMAKE_AUTOUIC_SEARCH_PATHS views views/settings)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRCS main.cpp renderer.cpp sound_manager.cpp vm_thread.cpp vm_tutorial_app.cpp)
set(HDRS renderer.h sound_manager.h vm_thread.h vm_tutorial_app.h)

set(CONTROLLER_SRCS controllers/logger_window.cpp controllers/main_window.cpp)
set(CONTROLLER_HDRS controllers/logger_window.h controllers/main_window.h)

set(CONTROLLER_SETTINGS_SRCS controllers/settings/audio_settings.cpp
                             controllers/settings/general_settings.cpp
                             controllers/settings/graphics_settings.cpp
                             controllers/settings/keypad_settings.cpp
                             controllers/settings/machine_settings.cpp
                             controllers/settings/settings_dialog.cpp)

set(CONTROLLER_SETTINGS_HDRS controllers/settings/audio_settings.h
                             controllers/settings/general_settings.h
                             controllers/settings/graphics_settings.h
                             controllers/settings/keypad_settings.h
                             controllers/settings/machine_settings.h
                             controllers/settings/settings_dialog.h)

set(MODELS_SRCS models/app_settings.cpp)
set(MODELS_HDRS models/app_settings.h)

set(TONE_GENERATORS_SRCS tone_generators/sine_wave.cpp)
set(TONE_GENERATORS_HDRS tone_generators/interface.h
                         tone_generators/sine_wave.h)

set(VIEWS_FILES views/logger_window.ui views/main_window.ui)

set(VIEWS_SETTINGS_FILES views/settings/audio_settings.ui
                         views/settings/general_settings.ui
                         views/settings/graphics_settings.ui
                         views/settings/keypad_settings.ui
                         views/settings/machine_settings.ui
                         views/settings/settings_dialog.ui)

set(ASSET_FILE assets/assets.qrc)

add_executable(VMTutorialFrontend ${SRCS}
                                  ${HDRS}
                                  ${CONTROLLER_SRCS}
                                  ${CONTROLLER_SETTINGS_SRCS}
                                  ${CONTROLLER_HDRS}
                                  ${CONTROLLER_SETTINGS_HDRS}
                                  ${MODELS_SRCS}
                                  ${MODELS_HDRS}
                                  ${TONE_GENERATORS_SRCS}
                                  ${TONE_GENERATORS_HDRS}
                                  ${VIEWS_FILES}
                                  ${VIEWS_SETTINGS_FILES}
                                  ${ASSET_FILE})

target_link_libraries(VMTutorialFrontend core
                                         Qt6::Multimedia
                                         Qt6::OpenGLWidgets
                                         Qt6::Widgets)

target_include_directories(VMTutorialFrontend PRIVATE ../core/src/public .)