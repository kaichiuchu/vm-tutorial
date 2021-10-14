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

#include <QAudioFormat>
#include <QIODevice>

/// This class defines an interface to a tone generator.
///
/// The tone generator implementation should only have to implement the \ref
/// ToneGeneratorInterface::Generate() method.
class ToneGeneratorInterface : public QIODevice {
  Q_OBJECT

 public:
  using BytesPerSample = int;
  using BytesForDuration = int32_t;

  using OutputBufferInfo = std::pair<BytesPerSample, BytesForDuration>;

  /// Opens the tone generator for reading its data only.
  void start() noexcept { open(QIODevice::ReadOnly); }

  /// Closes the tone generator device.
  void stop() noexcept {
    pos_ = 0;
    close();
  }

  /// Generates a sound wave.
  ///
  /// Implementations *must* call the \ref SetupOutputBuffer() method to
  /// properly configure the audio output buffer and to acquire the necessary
  /// information to properly generate a sound wave.
  ///
  /// \param format The audio output format of the audio device.
  /// \param duration The duration of the sound wave to generate.
  /// \param frequency The frequency of the sound wave.
  virtual void Generate(const QAudioFormat &format, double duration,
                        int frequency) noexcept = 0;

 protected:
  explicit ToneGeneratorInterface(QObject *parent_object) noexcept
      : QIODevice(parent_object) {}

  /// Retrieves the raw pointer to the audio buffer.
  ///
  /// \returns The raw pointer to the audio buffer.
  auto GetOutputBufferPtr() noexcept { return buffer_.data(); }

  /// Configures the output buffer.
  ///
  /// This method must be called by the tone generator implementation, as it
  /// adjusts the size of the audio output buffer.
  ///
  /// \param format The audio output format of the audio device. This is
  /// necessary to calculate the appropriate bytes per sample and duration.
  ///
  /// \param duration The length of the tone, in milliseconds.
  auto SetupOutputBuffer(const QAudioFormat &format, double duration) noexcept
      -> OutputBufferInfo {
    // The bytes per sample is the number of bytes required to represent one
    // sample.
    const auto bytes_per_sample = format.bytesPerSample();

    // The \ref QAudioFormat::bytesForDuration() method takes microseconds, so
    // we have to convert from milliseconds to microseconds. It returns the
    // number of bytes necessary to play a tone of `duration`.
    const auto bytes_for_duration = format.bytesForDuration(duration * 1000);

    // Adjust the size of the audio buffer.
    buffer_.resize(bytes_for_duration);
    pos_ = buffer_.size();

    // Generator methods will need this information.
    return {bytes_per_sample, bytes_for_duration};
  }

 private:
  /// From Qt's documentation:
  ///
  /// Reads up to `maxSize` bytes from the device into `data`, and returns the
  /// number of bytes read or -1 if an error occurred.
  ///
  /// If there are no bytes to be read and there can never be more bytes
  /// available (examples include socket closed, pipe closed, sub-process
  /// finished), this function returns `-1`.
  ///
  /// This function is called by \ref QIODevice. Reimplement this function when
  /// creating a subclass of \ref QIODevice.
  ///
  /// When reimplementing this function it is important that this function
  /// reads all the required data before returning. This is required in order
  /// for \ref QDataStream to be able to operate on the class. \ref QDataStream
  /// assumes all the requested information was read and therefore does not
  /// retry reading if there was a problem.
  ///
  /// This function might be called with a `maxSize` of 0, which can be used to
  /// perform post-reading operations.
  auto readData(char *data, int64_t maxSize) noexcept -> int64_t override {
    auto total = 0;

    if (!buffer_.isEmpty()) {
      while ((maxSize - total) > 0) {
        const auto chunk = std::min((buffer_.size() - pos_), maxSize - total);

        memcpy(data + total, buffer_.constData() + pos_, chunk);

        pos_ = (pos_ + chunk) % buffer_.size();
        total += chunk;
      }
    }
    return total;
  }

  /// From Qt's documentation:
  ///
  /// Writes up to `maxSize` bytes from data to the device. Returns the number
  /// of bytes written, or `-1` if an error occurred.
  ///
  /// This function is called by \ref QIODevice. Reimplement this function
  /// when creating a subclass of \ref QIODevice.
  ///
  /// When reimplementing this function it is important that this function
  /// writes all the data available before returning. This is required in order
  /// for \ref QDataStream to be able to operate on the class. \ref QDataStream
  /// assumes all the information was written and therefore does not retry
  /// writing if there was a problem.
  ///
  /// We won't write any data to the I/O device, so we can say that this
  /// method always succeeds.
  auto writeData(const char *, int64_t) -> int64_t override { return 0; }

  /// From Qt's documentation:
  ///
  /// Returns the number of bytes that are available for reading. This function
  /// is commonly used with sequential devices to determine the number of bytes
  /// to allocate in a buffer before reading.
  ///
  /// Subclasses that reimplement this function must call the base
  /// implementation in order to include the size of the buffer of
  /// \ref QIODevice.
  auto bytesAvailable() const noexcept -> int64_t override {
    return buffer_.size() + QIODevice::bytesAvailable();
  }

  /// From Qt's documentation:
  ///
  /// For open random-access devices, this function returns the size of the
  /// device. For open sequential devices, \ref bytesAvailable() is returned.
  ///
  /// If the device is closed, the size returned will not reflect the actual
  /// size of the device.
  auto size() const -> int64_t override { return buffer_.size(); }

  QByteArray buffer_;
  int64_t pos_ = 0;
};