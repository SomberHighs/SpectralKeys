# SpectralKeys

### Audio Intelligence for Your DAW

**SpectralKeys** is a VST3 plugin that analyzes any audio file and gives you the **key**, **BPM**, a full **spectrogram**, and a **MIDI transcription** you can drag straight into your piano roll. No more guessing — just drop, detect, and drag.

---

## What It Does

```
 Audio File ──► Spectrogram ──► Key & BPM Detection ──► MIDI Conversion ──► Drag to Piano Roll
```

| Feature | Description |
|---|---|
| **Spectrogram** | Real-time FFT visualization with a heat-mapped frequency display |
| **Key Detection** | Krumhansl-Schmuckler algorithm — detects major and minor keys with confidence scoring |
| **BPM Detection** | Onset autocorrelation with perceptual weighting for accurate tempo analysis |
| **Audio → MIDI** | YIN pitch detection converts audio into playable MIDI notes |
| **MIDI Drag-Out** | Drag the generated `.mid` file directly from the plugin into your DAW |

---

## Supported Formats

`.WAV` `.MP3` `.FLAC` `.AIFF` `.OGG`

Drop any of these onto the plugin or use the **Browse** button to load.

---

## How It Works Under The Hood

**Spectrogram** — 2048-sample FFT with Hann windowing and 75% overlap, rendered as a color-mapped frequency-time image (black → blue → cyan → yellow → white).

**Key Detection** — Builds a chromagram from the FFT magnitudes, then correlates against the Krumhansl-Kessler major and minor key profiles across all 24 keys. The highest Pearson correlation wins.

**BPM Detection** — Computes a spectral flux onset strength signal, autocorrelates it across tempo candidates (40–220 BPM), and applies a Gaussian weighting centered at 120 BPM to resolve octave ambiguity.

**Pitch Detection** — YIN algorithm with cumulative mean normalized difference, parabolic interpolation for sub-sample accuracy, and a voicing threshold of 0.15.

**MIDI Conversion** — Segments audio at detected onsets, takes the median pitch per segment, maps to the nearest MIDI note, and derives velocity from RMS energy.

---

## UI

The interface is designed to be **dark-themed** and **production-friendly**:

- **Drop Zone** — drag audio files directly onto the plugin
- **Spectrogram View** — scrollable FFT heatmap of your audio
- **Piano Roll** — color-coded MIDI notes by pitch class (C=red, E=lime, G=cyan, B=magenta...)
- **Info Panel** — key, BPM, and duration with glowing accent text
- **MIDI Drag Button** — click and drag to export MIDI into your DAW
- **Transport** — play/stop controls for previewing loaded audio

---

## Build From Source

### Requirements

- **CMake** 3.22+
- **JUCE** (included as a git submodule)
- **Visual Studio 2022** (Windows) or **Xcode** (macOS)
- C++17 compiler

### Steps

```bash
git clone --recurse-submodules https://github.com/SomberHighs/SpectralKeys.git
cd SpectralKeys
cmake -B build
cmake --build build --config Release
```

The VST3 plugin is automatically installed to your system VST3 directory.

---

## DAW Compatibility

Tested with VST3 hosts including:

- Ableton Live
- FL Studio
- Reaper
- Bitwig Studio
- Studio One

> **Note:** Some DAWs may intercept file drops before they reach the plugin. Use the **Browse** button as a fallback.

---

## Limitations

- Pitch detection is **monophonic** (YIN algorithm) — works best on isolated instruments, vocals, and single-note melodies rather than full mixes or chords
- BPM detection assumes a relatively steady tempo
- Key detection is optimized for tonal music

---

## License

MIT

---

Built by [SomberHighs](https://github.com/SomberHighs)
