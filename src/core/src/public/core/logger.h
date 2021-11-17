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

#pragma once

#include <fmt/core.h>

#include <functional>
#include <string>

namespace chip8 {
/// This class provides a very basic facility to report messages from the core
/// to the frontend.
///
/// I'm sorry to say that this is a singleton, but for our purposes it is just
/// fine. An example of a widely used C++ logging library using a singleton is
/// spdlog (https://github.com/gabime/spdlog).
class Logger {
 public:
  /// Defines the various log levels that we support.
  enum class LogLevel { kInfo, kWarning, kError, kDebug };

  /// Defines the function prototype that the log message callback must be.
  using LogMessageFunc = std::function<void(const std::string&)>;

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
  void Emit(const LogLevel level, std::string_view fmt,
            const Args&... args) const noexcept {
    if (log_message_func_) {
      std::string result;

      switch (level) {
        case LogLevel::kInfo:
          result = "[INFO]: ";
          break;

        case LogLevel::kWarning:
          result = "[WARNING]: ";
          break;

        case LogLevel::kError:
          result = "[ERROR]: ";
          break;

        case LogLevel::kDebug:
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
  LogLevel level_ = LogLevel::kInfo;

  /// The current log message callback function. If this is \p nullptr, logging
  /// is disabled.
  LogMessageFunc log_message_func_ = nullptr;

 private:
  Logger() noexcept = default;
};
}  // namespace chip8