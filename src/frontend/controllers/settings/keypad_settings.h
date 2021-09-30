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

#include <QKeyEvent>
#include <QTimer>
#include <QWidget>

#include "ui_keypad_settings.h"

/// This class handles the logic of user actions that take place in the keypad
/// settings widget.
class KeypadSettingsController : public QWidget {
  Q_OBJECT

 public:
  explicit KeypadSettingsController(QWidget* parent_widget) noexcept;

  /// Reports to the user that the key they selected is already bound for
  /// another use within the program.
  ///
  /// \param physical_key The physical key which conflicts with another key
  /// binding.
  void ReportKeyConfliction(const int physical_key) noexcept;

 private:
  /// Populates the widget with the current keypad settings.
  void PopulateDataFromAppSettings() noexcept;

  ///
  void UpdateButtonText(QPushButton* const button,
                        const int physical_key) noexcept;

  /// Connects the buttons to set bindings for the CHIP-8 keypad to \ref
  /// HandleKeyBindingChange().
  void ConnectKeyBindingSignals() noexcept;

  /// This method is called when the user presses a button. This will start a
  /// timer ticking every second to begin a countdown. The user has to press a
  /// button on the keyboard to bind the corresponding CHIP-8 key to a physical
  /// key.
  void HandleKeyBindingChange(QPushButton* const button,
                              const chip8::Key chip8_key) noexcept;

  /// This method is called every second by the countdown timer to handle the
  /// countdown. Users have 5 seconds to press a key to bind a physical key to a
  /// CHIP-8 key. If no key is pressed, the original key binding is preserved.
  void HandleKeyBindingCountdown() noexcept;

  /// From Qt documentation:
  ///
  /// This event handler, for event event, can be reimplemented in a subclass to
  /// receive key press events for the widget.
  ///
  /// A widget must call \ref QWidget::setFocusPolicy() to accept focus
  /// initially and have focus in order to receive a key press event.
  ///
  /// If you reimplement this handler, it is very important that you call the
  /// base class implementation if you do not act upon the key.
  ///
  /// The default implementation closes popup widgets if the user presses the
  /// key sequence for QKeySequence::Cancel (typically the Escape key).
  /// Otherwise the event is ignored, so that the widget's parent can interpret
  /// it.
  ///
  /// Note that QKeyEvent starts with isAccepted() == true, so you do not need
  /// to call \ref QKeyEvent::accept() - just do not call the base class
  /// implementation if you act upon the key.
  ///
  /// We override this method to handle input for key bindings.
  void keyPressEvent(QKeyEvent* event) noexcept override;

  Ui::KeypadSettings view_;

  /// This struct stores the countdown timer.
  struct {
    /// The actual timer instance. This should run every second.
    QTimer* timer_;

    /// The number of seconds remaining before the timer should be stopped.
    unsigned int seconds_remaining_;
  } countdown_;

  /// This struct stores information about the button the user pressed.
  struct {
    /// We store a pointer to the button that the user pressed to update the
    /// text iff the user has chosen to rebind the key.
    QPushButton* button_;

    // We store the original button text if the user canceled the rebinding,
    // or if the countdown timer fully expired.
    QString original_button_text_;

    /// The actual CHIP-8 key that the button refers to.
    chip8::Key chip8_key_;
  } selected_key_;
};