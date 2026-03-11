#pragma once

namespace Constants
{
    constexpr int    kFFTOrder       = 11;
    constexpr int    kFFTSize        = 1 << kFFTOrder;  // 2048
    constexpr int    kHopSize        = 512;
    constexpr double kSampleRate     = 44100.0;
    constexpr float  kYinThreshold   = 0.15f;
    constexpr int    kTicksPerBeat   = 480;
    constexpr float  kMinBpm         = 40.0f;
    constexpr float  kMaxBpm         = 220.0f;
    constexpr float  kA4Frequency    = 440.0f;
    constexpr float  kDbFloor        = -80.0f;
    constexpr float  kDbCeiling      = 0.0f;
    constexpr int    kMinMidiNote    = 21;   // A0
    constexpr int    kMaxMidiNote    = 108;  // C8
    constexpr float  kMinOnsetIntervalSec = 0.05f;
}
