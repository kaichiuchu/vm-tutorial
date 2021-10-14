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

#include "interface.h"

/// This class handles the generation of sine waves.
class SineWaveGenerator : public ToneGeneratorInterface {
  Q_OBJECT

 public:
  explicit SineWaveGenerator(QObject* parent_object) noexcept;

  /// Generates a sine wave.
  ///
  /// \param format The format of the audio device, used to determine the bytes
  /// per sample and bytes for duration.
  ///
  /// \param duration The length of the sine wave, in milliseconds.
  ///
  /// \param frequency The frequency of the sine wave.
  void Generate(const QAudioFormat& format, double duration,
                int frequency) noexcept override;
};