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

#include <core/logger.h>
#include <fmt/core.h>

#include <string>

/// This class provides a very basic facility to report messages from the core
/// to the frontend.
///
/// I'm sorry to say that this is a singleton, but for our purposes it is just
/// fine. An example of a widely used C++ logging library using a singleton is
/// spdlog (https://github.com/gabime/spdlog).
class Logger {
 public:
  /// Instantiates the logger if it doesn't exist.
  ///
  /// \returns An instance of \p Logger.
  static auto Get() noexcept -> Logger&;

  /// If a log message callback function has been specified, dispatches a
  /// message to the callback function.
  ///
  /// \param level The severity of the log message.
  /// \param fmt The format string of the message, see
  /// https://fmt.dev/latest/syntax.html for examples.
  /// \param args The arguments to the format string.
  template <class... Args>
  void Emit(const chip8::LogLevel level, std::string_view fmt,
            const Args&... args) const noexcept {
    if (log_message_func_) {
      std::string result;

      switch (level) {
        case chip8::LogLevel::kInfo:
          result = "[INFO]: ";
          break;

        case chip8::LogLevel::kWarning:
          result = "[WARNING]: ";
          break;

        case chip8::LogLevel::kDebug:
          result = "[DEBUG]: ";
          break;
      }

      result += fmt::format(fmt, args...);
      log_message_func_(result);
    }
  }

  /// The log level currently in use. This is an inclusive level system, for
  /// example if you specify the level is \ref chip8::LogLevel::kWarning, you
  /// will receive both warning and info messages.
  chip8::LogLevel level_ = chip8::LogLevel::kInfo;

  /// The current log message callback function. If this is \p nullptr, logging
  /// is disabled.
  chip8::LogMessageFunc log_message_func_ = nullptr;

 private:
  Logger() noexcept = default;
};