# Dirty Little Bass Synth — Working Notes

JUCE 8 / C++20 mono bass synth, single voice, by Robert Fullum / FullumMusic.

Read this before doing anything in the repo.

---

## Build

```bash
# Resave the .jucer file after editing it (adds new files to the Xcode project).
"/Users/robertfullum/JUCE/Projucer.app/Contents/MacOS/Projucer" \
    --resave "/Users/robertfullum/JUCE Projects/DirtyLittleBassSynth/DirtyLittleBassSynth/Wavetable5.jucer"

# Build (debug):
xcodebuild \
    -project "/Users/robertfullum/JUCE Projects/DirtyLittleBassSynth/DirtyLittleBassSynth/Builds/MacOSX/Dirty Little Bass Synth.xcodeproj" \
    -configuration Debug \
    -target "Dirty Little Bass Synth - All" 2>&1 | grep -E "error:|fatal error|BUILD " | head -10
```

Don't push without the user's explicit ask. The user manages branches; we work on whatever branch they tell us they're on.

---

## Project layout

```
Source/
  Plugin/         PluginProcessor, PluginEditor
  DSP/            BassSynthVoice, MasterChain, ScopeBuffer, TempoSnapshot,
                  MidiLearnManager, Wavetable, Modifiers, FilterSection,
                  Oscillators, SubOscillator, DryWet
  GUI/
    Sources/      SourcesColumn, OscPanel, AmpAdsrPanel
    Filter/       FilterColumn, FilterPanel, FilterAdsrPanel, LfoPanel, FilterVisual
    Modifiers/    ModifiersColumn, ModifierPanel
    Master/       MasterColumn, OutMeter, ScopeVisual
    GUIUtils/     GuiResources, GuiHelpers, ColorPalette, OtherLookAndFeel,
                  TitleHeader, OscVisual, AdsrVisual, SegmentedControl,
                  OscillatorParameterControls, MidiLearnOverlay, DryWetSlider
```

Header search paths cover every subfolder, so `#include "Foo.h"` works from anywhere.

---

## Architecture summary

**4-column body layout**, each owning its sub-panels:
- `SourcesColumn` → `OscPanel` + `AmpAdsrPanel`
- `FilterColumn` → `FilterPanel` + `FilterAdsrPanel` + `LfoPanel`
- `ModifiersColumn` → `ModifierPanel` (foldback, ring mod, freq shift, S&H, portamento mode buttons)
- `MasterColumn` → out gain + ceiling/limiter + scope + output meter + GR meter + widener + bass mono-izer

**`GuiResources`** is a struct passed by `const ref` (or non-const ref) to every panel. Carries: APVTS, two `OtherLookAndFeel` instances, `Palette::Theme`, `ScopeBuffer*`, `TempoSnapshot*`, `isStandalone` bool, `MidiLearnManager*`. Add new shared resources here when needed.

**`MasterChain`** runs post-voice in `processBlock`. Signal flow:
1. Master HPF (fixed 20 Hz, hidden, no UI)
2. Linkwitz-Riley LR4 crossover at user-set frequency
3. Low band → forced mono. High band → Haas delay on one channel (widener).
4. Brick-wall limiter (when on). Stereo-linked.

**`BassSynthVoice`** — single voice, mono. **4× oversampling wraps only the foldback distortion + modifier chain.** Wavetable + sub osc + filter + master gain stay at base rate. Latency reported via `setLatencySamples(...)`. Modifier internal oscillators clocked at 4× base.

**`TempoSnapshot`** — atomic struct populated each block from `getPlayHead()->getPosition()`. Falls back to `tempo_fallback_bpm` APVTS param when no host BPM. Editable via title header drag/double-click in standalone only.

**`MidiLearnManager`** — `cc → paramIndex` lookup table (128 atomic ints). State machine: Idle / Listening / Armed. Persistence is **global** via `juce::ApplicationProperties` (per-user, not per-patch). Default mappings registered via `RegisterDefaultMapping(cc, paramIndex)` are re-applied by `ClearAllMappings`. Currently: standalone-only CC1 → `filtLFO_amt`.

**`MidiLearnOverlay`** — glass-pane component covering the body region. Hidden in idle, visible during learn mode. Intercepts clicks, walks up to find a `paramID` property, arms the param. Renders tint, hover halo, armed pulse, CC# badges, learn-flash. Right-click → unmap.

---

## Code style — non-negotiable

Match these exactly when writing or editing code. The user has called these out specifically.

### Spacing & punctuation

- **No space between cast type and value:** `(float)x` not `(float) x`. Same for `(int)`, `(size_t)`, `(juce::uint8)`. C-style casts in function signatures (`(int) override`) are the exception.
- **Pointer/reference symbols stick to the variable name:** `juce::Slider &slider`, `std::atomic<float> *ptr`. Not `juce::Slider& slider`.
- **No space between function name and parens.** `getBounds()` not `getBounds ()`.

### Initializer lists

Comma-first, leading colon on its own line, aligned:
```cpp
MyClass::MyClass()
: memberA(0)
, memberB(1)
, memberC(2)
{}
```

### Wrapped function arguments

When wrapping function call/declaration args, put the comma at the *start* of the next line, aligned with the first arg:
```cpp
DLBS::SetupSlider(this
                  , slider
                  , juce::Slider::SliderStyle::LinearVertical
                  , accent
                  , thumb
                  , txt);
```

### Naming

- Our methods use **CapitalCamelCase**: `Init`, `Update`, `SetupSlider`, `RegisterParam`, `ClearAllMappings`. JUCE overrides keep their JUCE casing (`paint`, `resized`, `mouseDown`).
- `auto bounds = getLocalBounds();` — never `area`, `totalArea`, etc.

### Method ordering inside classes

**Within each visibility section**, JUCE-inherited methods come first, then our methods. Constructor/destructor come before everything.

```cpp
public:
    MyClass();
    ~MyClass() override = default;

    // JUCE overrides first
    void paint(juce::Graphics &) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &) override;

    // Our methods after
    void Init();
    void Update();
    void SetColors(...);

private:
    // Same rule (JUCE first, ours second) per section.
```

**The .cpp method definition order must match the header order.** When reordering one, reorder the other to match.

### Section dividers

Use thin comment dividers between top-of-file includes and the first class, between unrelated classes in the same file, between major method groups. Style:

```cpp
//==============================================================================
```

Or the shorter version we use in panels:

```cpp
//============================================================
```

### .jucer file additions

When adding a new source file: **`.cpp` entry first, then `.h` entry** for that pair. `<FILE id="..." name="X.cpp" compile="1" .../>` then `<FILE id="..." name="X.h" compile="0" .../>`.

After editing the .jucer, run the Projucer resave command above before building.

---

## Patterns to follow

### Adding a new APVTS-bound control

Use the helpers in `GuiHelpers.h` so the component gets tagged with its `paramID` for MIDI Learn:

```cpp
mySliderAtt = DLBS::AttachSlider(*res.apvts, "my_param", mySlider);
myButtonAtt = DLBS::AttachButton(*res.apvts, "my_param", myButton);
```

`SegmentedControl::Setup` self-tags via the `parameterID` arg.

### Adding a new MIDI-learnable param

Add to the `learnableIDs` list in `PluginProcessor::RegisterMidiLearnableParams`. The order is the persisted-mapping index order; appending is safe, renaming/removing breaks existing user mappings (handled gracefully — unknown IDs drop on load).

**Don't add to the registry:** `pitch_bend_range`, `tempo_fallback_bpm`. Both are setup parameters, not live-tweak controls.

### Theme colors

Defined in `ColorPalette.h` → `Palette::DefaultTheme`:
- `primaryAccent` (teal) — Sources column, amp ADSR
- `secondaryAccent` (purplish) — Filter column, filter ADSR, LFO, MIDI Learn UI
- `pinkAccent` — Ring Mod, Limiter
- `orangeAccent` — Foldback, Freq Shift, S&H, Bass Mono-izer
- `textPrimary` — main text color
- `textSecondary` — dim text
- `structure` — track backgrounds, dividers
- `background` — base fill

### Look-and-feel

`OtherLookAndFeel` handles rotary + linear slider drawing. Two instances:
- `dialLookAndFeel` — standard dials.
- `dryWetLookAndFeel` — instance reused for dry/wet rotaries (currently visually identical, but kept separate so dry/wet behavior can diverge later).

Slider properties the L&F respects:
- `bipolarFill` — fills from track centre to thumb (used by the Wide slider).
- `snapAt50` — draws a tick at the track center, brighter when within 1.5% of centre. Used by Dry/Wet rotaries.

`DryWetSlider` adds gestures: drag, double-click → 0%, shift-click → 50%, Cmd/Ctrl-click → 100%.

### Audio thread / message thread split

The audio thread runs `processBlock` and everything reachable from it (voice render, master chain, MIDI Learn CC dispatch).

**Never on the audio thread:**
- File I/O (use the dirty-flag pattern: audio thread sets atomic, UI thread polls and saves)
- Allocations in `renderNextBlock` (everything pre-allocated in `Init` / `prepareToPlay`)
- Calls to `juce::AlertWindow`, message-thread-only JUCE methods
- Mutex locks (use atomics)

**The dirty-flag pattern** (e.g. `MidiLearnManager`): audio thread mutates state and flips an atomic dirty flag. Editor's 60Hz timer polls the flag, performs the heavy work (file save, repaint, etc.) on the message thread, clears the flag.

### Editor's 60Hz timer

`PluginEditor::timerCallback` drives:
- All column `Update()` calls (osc visuals, ADSR visuals, scope, filter visual, LFO visual)
- Title header BPM display refresh
- Master column meters + GR meter + scope
- MIDI Learn overlay visibility + animation
- MIDI Learn dirty-flag persistence

Add new timer-driven UI work here. Don't spin up additional 60Hz timers.

---

## Build verification

After any code change, run the build. The `xcodebuild ... | grep -E "error:|BUILD "` snippet at the top of this file is the canonical check — only show output that matters.

If the .jucer changed (new file, new header path), Projucer-resave first.

---

## Things to ask the user about, not assume

- Branch creation / switching
- Force pushes
- Anything that touches global git config
- Adding new APVTS params (param IDs, ranges, defaults are sound design decisions)
- UI layout / sizing changes (the user iterates visually and gives explicit pixel directions)
- Whether something should go into the patch state vs. global app properties

---

## Common gotchas

- **`std::atomic<float>` ternary ambiguity:** `cond ? *atomicPtr : 0.0f` errors. Use `cond ? atomicPtr->load() : 0.0f`.
- **`getComponentAt` returns the overlay itself** if the overlay is in the hit-test path. The MidiLearnOverlay temporarily flips `setInterceptsMouseClicks(false, false)` before querying.
- **Slider attached to `AudioParameterChoice`** — use `slider.textFromValueFunction = ...` to display choice strings; the SliderAttachment alone won't.
- **`Synthesiser::stopNote(0, false)`** is called before `startNote` when stealing voices (legato note-on while previous still held). Check `allowTailOff` to distinguish real note-offs from voice-stealing.
- **`stopNote` with `allowTailOff=false`** doesn't envelope-release; it hard-stops. Mode-aware code (e.g. portamento legato) should branch on this.

---

## Phases done (just for context)

1. Master Mono+Widen
2. Master Limiter + GR meter
3. Output Scope
4. Master HPF
5. 4× Oversampling (foldback + modifiers)
6. Host BPM sync infrastructure
7. LFO Sync (FRQ / SYNC modes, subdivisions, phase lock to ppqPosition)
8. Filter visualization animated by ADSR/LFO — *not yet done*
9. Portamento mode buttons (On/Off, Always/Legato)
10. Dry/Wet snap-at-50 visual cue
11. Auto-MIDI map (CC1 → LFO Amount, standalone only)
12. MIDI Learn (full feature: glass-pane overlay, persistence, right-click unmap)
