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

/// This header file provides information that is shared among different
/// components of the frontend.

/// Defines the different types of sound waves that we can generate.
enum class ToneType { kSineWave, kSawtooth, kSquare, kTriangle };

/// Defines the various run states of the virtual machine.
enum class RunState { kStopped, kRunning };