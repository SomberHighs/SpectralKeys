#pragma once
#include <cmath>
#include "Constants.h"

namespace MathUtils
{
    inline float frequencyToMidiNote(float freq)
    {
        if (freq <= 0.0f) return -1.0f;
        return 69.0f + 12.0f * std::log2(freq / Constants::kA4Frequency);
    }

    inline int frequencyToMidiNoteRounded(float freq)
    {
        float note = frequencyToMidiNote(freq);
        if (note < 0.0f) return -1;
        return static_cast<int>(std::round(note));
    }

    inline float midiNoteToFrequency(int note)
    {
        return Constants::kA4Frequency * std::pow(2.0f, (note - 69.0f) / 12.0f);
    }

    inline int frequencyToPitchClass(float freq)
    {
        int note = frequencyToMidiNoteRounded(freq);
        if (note < 0) return -1;
        return note % 12;
    }

    inline float amplitudeToDb(float amplitude)
    {
        constexpr float epsilon = 1e-10f;
        return 20.0f * std::log10(amplitude + epsilon);
    }
}
