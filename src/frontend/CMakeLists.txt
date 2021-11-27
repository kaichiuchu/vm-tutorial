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
set(CMAKE_AUTOUIC_SEARCH_PATHS views views/settings)

find_package(SDL2 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRCS main.cpp
         memory_view.cpp
         renderer.cpp
         sound_manager.cpp
         vm_thread.cpp
         vm_tutorial_app.cpp)

set(HDRS memory_view.h
         renderer.h
         sound_manager.h
         vm_thread.h
         vm_tutorial_app.h
         types.h)

set(CONTROLLER_SRCS controllers/debugger_window.cpp
                    controllers/logger_window.cpp
                    controllers/main_window.cpp)

set(CONTROLLER_HDRS controllers/debugger_window.h
                    controllers/logger_window.h
                    controllers/main_window.h)

set(CONTROLLER_SETTINGS_SRCS controllers/settings/audio_settings.cpp
                             controllers/settings/general_settings.cpp
                             controllers/settings/keypad_settings.cpp
                             controllers/settings/logger_settings.cpp
                             controllers/settings/machine_settings.cpp
                             controllers/settings/settings_dialog.cpp)

set(CONTROLLER_SETTINGS_HDRS controllers/settings/audio_settings.h
                             controllers/settings/general_settings.h
                             controllers/settings/keypad_settings.h
                             controllers/settings/logger_settings.h
                             controllers/settings/machine_settings.h
                             controllers/settings/settings_dialog.h)

set(MODELS_SRCS models/app_settings.cpp
                models/debugger_disasm.cpp
                models/debugger_registers.cpp
                models/debugger_stack.cpp)

set(MODELS_HDRS models/app_settings.h
                models/debugger_disasm.h
                models/debugger_registers.h
                models/debugger_stack.h)

set(VIEWS_FILES views/debugger_window.ui
                views/logger_window.ui
                views/main_window.ui)

set(VIEWS_SETTINGS_FILES views/settings/audio_settings.ui
                         views/settings/general_settings.ui
                         views/settings/keypad_settings.ui
                         views/settings/logger_settings.ui
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
                                  ${VIEWS_FILES}
                                  ${VIEWS_SETTINGS_FILES}
                                  ${ASSET_FILE})

target_link_libraries(VMTutorialFrontend core
                                         Qt6::OpenGLWidgets
                                         Qt6::Widgets
                                         SDL2::SDL2)

target_include_directories(VMTutorialFrontend PRIVATE ../core/src/public .)

# On Windows, we'll need to execute windeployqt.
# XXX: I have no idea if this will work outside of vcpkg.
if (WIN32)
  get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
  get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

  if (CMAKE_BUILD_TYPE STREQUAL "Release" OR "RelWithDebInfo")
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET VMTutorialFrontend POST_BUILD
                       COMMAND "${CMAKE_COMMAND}" -E
                               env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                               \"$<TARGET_FILE:VMTutorialFrontend>\"
                       COMMENT "Deploying Qt...")
  else()
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt.debug.bat HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET VMTutorialFrontend POST_BUILD
                       COMMAND "${CMAKE_COMMAND}" -E
                               env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
                               \"$<TARGET_FILE:VMTutorialFrontend>\"
                       COMMENT "Deploying Qt...")
  endif()
endif()
