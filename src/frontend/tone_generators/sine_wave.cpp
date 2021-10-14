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

#include "sine_wave.h"

SineWaveGenerator::SineWaveGenerator(QObject *parent_object) noexcept
    : ToneGeneratorInterface(parent_object) {}

void SineWaveGenerator::Generate(const QAudioFormat &format,
                                 const double duration,
                                 const int frequency) noexcept {
  auto [bytes_per_sample, bytes_for_duration] =
      SetupOutputBuffer(format, duration);

  auto ptr = reinterpret_cast<unsigned char *>(GetOutputBufferPtr());
  auto sample_index = 0;

  const auto sample_rate = format.sampleRate();

  while (bytes_for_duration) {
    const auto amplitude =
        std::sin(2 * M_PI * frequency * qreal(sample_index++ % sample_rate) /
                 sample_rate);

    for (auto i = 0; i < format.channelCount(); ++i) {
      switch (format.sampleFormat()) {
        case QAudioFormat::UInt8:
          *reinterpret_cast<uint8_t *>(ptr) =
              static_cast<uint8_t>((1.0 + amplitude) / 2 * 255);
          break;

        case QAudioFormat::Int16:
          *reinterpret_cast<int16_t *>(ptr) =
              static_cast<int16_t>(amplitude * 32767);
          break;

        case QAudioFormat::Int32:
          *reinterpret_cast<int32_t *>(ptr) = static_cast<int32_t>(
              amplitude * std::numeric_limits<int32_t>::max());
          break;

        case QAudioFormat::Float:
          *reinterpret_cast<float *>(ptr) = amplitude;
          break;

        default:
          break;
      }

      ptr += bytes_per_sample;
      bytes_for_duration -= bytes_per_sample;
    }
  }
}