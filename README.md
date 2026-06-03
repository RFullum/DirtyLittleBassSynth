# Dirty Little Bass Synth

[![Latest release](https://img.shields.io/github/v/release/RFullum/DirtyLittleBassSynth?label=download&style=for-the-badge)](https://github.com/RFullum/DirtyLittleBassSynth/releases/latest)

The Dirty Little Bass Synth is a monophonic wavetable Bass synth designed to get big, disgustingly aggressive Bass tones quickly.

---

## Download

Grab the latest macOS installer from the [Releases page](https://github.com/RFullum/DirtyLittleBassSynth/releases/latest). The `.pkg` installs the VST3, AU, and Standalone. Signed and notarized for macOS.

**Install locations:**

- VST3 → `/Library/Audio/Plug-Ins/VST3/Dirty Little Bass Synth.vst3`
- AU → `/Library/Audio/Plug-Ins/Components/Dirty Little Bass Synth.component`
- Standalone → `/Applications/Dirty Little Bass Synth.app`
- User patches → `~/Library/Application Support/FullumMusic/Dirty Little Bass Synth/Patches/`

**System requirements:** macOS 11 (Big Sur) or later. Universal binary (Apple Silicon + Intel).

---

## Title Bar

### Plugin Name (DIRTY LITTLE BASS SYNTH)

Hover for a hint. **Right-click** to open the app menu:

- **Show Tooltips** (F1) — toggle hover-help text on every control.
- **Echo CCs to Controller** (standalone only) — sends CC messages back to your MIDI controller when you move a mapped control via the UI or load a patch. Useful for motorized faders and LED-ring encoders.

### BPM Display

Shows the current tempo with an **INT** badge (standalone) or **HOST** badge (plugin in a DAW).

In standalone: drag the BPM number up/down to change it (Shift = fine). Double-click to type a value.

### LEARN

Click to enter MIDI Learn mode. The body of the synth dims and intercepts clicks.

- **Left-click** a control to arm it, then send a CC from your controller to bind. The control shows its mapped CC# as a small badge.
- **Right-click** a mapped control to unmap.
- **CLEAR MAPS** wipes every binding.

Click LEARN again to exit. Mappings persist globally across sessions. In standalone, CC1 is mapped to Filter LFO Amount by default.

### Patch Controls

Top row: prev/next arrows around the current patch name. **Click the patch name** to open a grid popup of all factory + user patches. Factory patches are read-only (teal); user patches are editable (purple). A `*` after the name = unsaved changes.

Bottom row:

- **INIT** — load the default initialised state.
- **SAVE** — overwrite the current user patch. Disabled on factory + Init.
- **SAVE AS** — name a new patch and write it to your user folder.
- **DELETE** — remove the current user patch (confirmation prompt). Disabled on factory + Init.
- **RANDOM** — randomise every parameter.

You can also drag a `.dlbs` file from Finder onto the patch area to import.

Standalone keyboard shortcuts: **Cmd+S** = SAVE, **Cmd+Shift+S** = SAVE AS.

### PANIC

Hard-stops every active voice. Use when a stuck note slips through.

---

## Oscillators Column

### Oscillator

- **MORPH** slider — morphs between Sine, Spike, and Sawtooth.
- **Bend** vertical slider — pitch-bend range, 0–24 semitones.

The visualiser above the morph slider shows the current waveshape.

### Sub Oscillator

- **MORPH** slider — morphs between Sine, Square, and Sawtooth.
- **Sub Gain** vertical slider — sub-osc gain in dB, −∞ to 0 dB.
- **0 / -1 / -2** buttons — sub octave relative to the main oscillator.

### Amp

ADSR envelope applied to both Osc and Sub:

- **A** — attack time
- **D** — decay time
- **S** — sustain level
- **R** — release time

The envelope shape visualiser above the sliders previews the current shape.

---

## Modifiers Column

The dry/wet knobs blend with the main Osc only — the Sub bypasses everything in this column. Dry/wet rotaries support **double-click = 0%**, **shift-click = 50%**, **Cmd-click = 100%**.

The chain order is **Foldback → Ring Mod → Freq Shift → Sample & Hold**.

### Portamento

Rotary glide time (instant to ~1 s). Two mode buttons below:

- **OFF / ON** — engages portamento.
- **ALWAYS / LEGATO** — glide on every note vs. only on overlapping notes.

When off, the knob dims but stays interactive so you can preset a time.

### Foldback Distortion

Rotary adds harmonic content by folding the Osc waveform back on itself. Affects only the main Osc.

### Ring Mod / Freq Shift / Sample & Hold

Three rows sharing a common column layout:

- **Tone** (Ring Mod only) — morphs the carrier between Sine and Square.
- **Pitch** — modifier frequency relative to the played note. −2 to +2 octaves, centre = unison.
- **Dry/Wet** — blends the modified signal with everything upstream in the chain.

Sample & Hold's Pitch counterclockwise produces heavy bit-crushing; fully counterclockwise produces a square-like waveform an octave down.

---

## Filter Column

### Filter

- Filter type buttons: **-12dB / -24dB / -48dB / Notch**.
- **Cutoff** horizontal slider below the visualiser.
- **Rez** (resonance) vertical slider to the right.

The filter visualiser animates in real time as the envelope and LFO modulate the cutoff and resonance.

### Filter Env

- **A / D / S / R** sliders — ADSR for the filter envelope.
- **To Cutoff** knob — how much the envelope pushes the cutoff.
- **To Rez** knob — how much the envelope pushes resonance.

### Filter LFO

- Shape morph slider below the visualiser — Sine, Square, Saw.
- **Freq** vertical slider — LFO speed.
- **Amount** vertical slider — how much the LFO drives the cutoff.
- **FRQ / SYNC** toggle — FRQ uses the Freq slider in Hz; SYNC swaps the slider for a tempo-synced subdivision picker that locks to the host or standalone BPM.

---

## Master Column

### Out Gain

Rotary master output gain, in dB.

### Ceiling Limiter

- **Ceiling** rotary — limiter ceiling in dB.
- **ON / OFF** button — bypass the limiter.

A gain-reduction meter sits beside the output meter (below) showing how hard the limiter is working.

### Scope

Oscilloscope of the post-master output.

### Output Meter

Stereo level meter with a clip strip at the top of each channel. Clip strips latch red whenever a sample crosses 0 dBFS.

### Wide

Bipolar Haas-style stereo widener. Centre = mono. The further from centre, the wider the stereo image.

### Mono Below

Crossover frequency rotary. Frequencies below this are summed to mono.

---

## Tooltips

Hover any control to see its description. Toggle on/off via the title-bar right-click menu, or **F1** in standalone.
