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

#include "keypad_settings.h"

#include <QMessageBox>

#include "models/app_settings.h"

KeypadSettingsController::KeypadSettingsController(
    QWidget* parent_widget) noexcept
    : QWidget(parent_widget) {
  view_.setupUi(this);
  PopulateDataFromAppSettings();
  ConnectKeyBindingSignals();
}

void KeypadSettingsController::ReportKeyConfliction(
    const int physical_key) noexcept {
  QMessageBox::critical(this, tr("Key confliction"), tr("This key conflicts."));
}

void KeypadSettingsController::PopulateDataFromAppSettings() noexcept {
  const QMap<QString, QPushButton*> button_map = {
      {"key_0", view_.button_0}, {"key_1", view_.button_1},
      {"key_2", view_.button_2}, {"key_3", view_.button_3},
      {"key_4", view_.button_4}, {"key_5", view_.button_5},
      {"key_6", view_.button_6}, {"key_7", view_.button_7},
      {"key_8", view_.button_8}, {"key_9", view_.button_9},
      {"key_A", view_.button_A}, {"key_B", view_.button_B},
      {"key_C", view_.button_C}, {"key_D", view_.button_D},
      {"key_E", view_.button_E}, {"key_F", view_.button_F}};

  const auto current_key_bindings = AppSettingsModel().GetVMKeyBindings();

  for (const auto& [key, value] : current_key_bindings) {
    UpdateButtonText(button_map[key], value);
  }
}

void KeypadSettingsController::UpdateButtonText(
    QPushButton* const button, const int physical_key) noexcept {
  button->setText(QKeySequence(physical_key).toString());
}

void KeypadSettingsController::ConnectKeyBindingSignals() noexcept {
  connect(view_.button_1, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_1, chip8::Key::k1); });

  connect(view_.button_2, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_2, chip8::Key::k2); });

  connect(view_.button_3, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_3, chip8::Key::k3); });

  connect(view_.button_C, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_C, chip8::Key::kC); });

  connect(view_.button_4, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_4, chip8::Key::k4); });

  connect(view_.button_5, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_5, chip8::Key::k5); });

  connect(view_.button_6, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_6, chip8::Key::k6); });

  connect(view_.button_D, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_D, chip8::Key::kD); });

  connect(view_.button_7, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_7, chip8::Key::k7); });

  connect(view_.button_8, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_8, chip8::Key::k8); });

  connect(view_.button_9, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_9, chip8::Key::k9); });

  connect(view_.button_E, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_E, chip8::Key::kE); });

  connect(view_.button_A, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_A, chip8::Key::kA); });

  connect(view_.button_0, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_0, chip8::Key::k0); });

  connect(view_.button_B, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_B, chip8::Key::kB); });

  connect(view_.button_F, &QPushButton::clicked,
          [this]() { HandleKeyBindingChange(view_.button_F, chip8::Key::kF); });
}

void KeypadSettingsController::HandleKeyBindingChange(
    QPushButton* button, const chip8::Key chip8_key) noexcept {
  // We need to preserve information about the button pressed.
  //
  // We'll need to update the button text regardless of whether or not the user
  // presses a key, because the button's text is overwritten with the
  // notification to press a key. If the user doesn't press a key, we'll want to
  // return the button text to what it was before.
  //
  // We also need to store what CHIP-8 key this button refers to.
  selected_key_.button_ = button;
  selected_key_.original_button_text_ = button->text();
  selected_key_.chip8_key_ = chip8_key;

  // The button in question should be disabled while it's being modified.
  button->setEnabled(false);
  button->setText(tr("Press any key... [5]"));
  grabKeyboard();

  countdown_.timer_ = new QTimer(this);

  connect(countdown_.timer_, &QTimer::timeout, this,
          &KeypadSettingsController::HandleKeyBindingCountdown);

  countdown_.timer_->setSingleShot(false);

  // Users have 5 seconds to press a key.
  countdown_.seconds_remaining_ = 5;

  // Fire the timer every second.
  countdown_.timer_->start(1000);
}

void KeypadSettingsController::HandleKeyBindingCountdown() noexcept {
  countdown_.seconds_remaining_--;

  // The countdown expired, and the user didn't press anything.
  if (countdown_.seconds_remaining_ == 0) {
    selected_key_.button_->setEnabled(true);
    selected_key_.button_->setText(selected_key_.original_button_text_);

    // We no longer need the countdown timer.
    delete countdown_.timer_;
    countdown_.timer_ = nullptr;

    releaseKeyboard();
    return;
  }

  auto binding_text =
      QString{"Press any key... [%1]"}.arg(countdown_.seconds_remaining_);
  selected_key_.button_->setText(binding_text);
};

void KeypadSettingsController::keyPressEvent(QKeyEvent* event) noexcept {
  // The user hasn't selected a button to bind to. Pass the event along.
  if (!countdown_.timer_) {
    QWidget::keyPressEvent(event);
    return;
  }

  // The user has selected a key; we no longer need the timer.
  delete countdown_.timer_;
  countdown_.timer_ = nullptr;
  releaseKeyboard();

  AppSettingsModel().SetVMKeyBinding(selected_key_.chip8_key_, event->key());

  selected_key_.button_->setText(QKeySequence(event->key()).toString());
  selected_key_.button_->setEnabled(true);
}