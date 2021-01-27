# DirtyLittleBassSynth

The Dirty Little Bass Synth is a monophonic wavetable Bass synth designed to get big, disgustingly aggressive Bass tones quickly.



## Oscillator Section

There are two oscillators, each morphing between three different wave-shapes: The Oscillator and Sub Oscillator. The wavetables are made using only sine waves at the appropriate harmonic intervals and amplitudes. I'm using 56 harmonic sines for the square, spike, and sawtooth because i love how aggressively dirty they sound. The spike is essentially a resonantly high-passed square wave. There's a visual representation of the actual wave-shapes as you morph their shape.

### Oscillator

Horizontal slider morphs between Sine, Spike, and Sawtooth wave-shapes. Pitch-bend anywhere from 0 to 24 semitones via the vertical slider.

### Sub Oscillator

Horizontal slider morphs between Sine, Square, and Sawtooth wave-shapes. Vertical slider controls the Sub Osc's gain. ComboBox selects the Sub Osc's octave: 0 is the same octave as the Osc; -1 and -2 are one and two octaves below the Osc, respectively.



## Oscillator Control Section

Here we have the controls that directly affect the oscillators' sound generation: ADSR envelope, Portamento, and Foldback Distortion.

### ADSR

The envelope control vertical sliders works as you'd expect: Attack time; Decay time; Sustain level; Release time.

### Portamento

Rotary dial that controls the amount of time it takes to slide from one note to the next, from instantly to a long time.

### Fold-back Distortion

Only affects the Oscillator, not the Sub Oscillator. Rotary Dial adds harmonic content to the main Osc by applying fold-back distortion to the morphed wave-shape.



## Modifier Section

Three different modifiers: Ring Modulation, Frequency Shifter, Sample & Hold Distortion. These are only applied to the main Osc via the dry/wet knobs.

### Ring Mod

Tone controls the wave-shape of the ring modulator, morphing from Sine to Square. The Pitch range is -2 to +2 octaves, defaulting to unison. Dry/Wet blends with the main Osc signal.

### Freq Shift

Pitch range is -2 to +2 octaves, defaulting to unison. Dry/Wet blends with the main Osc signal and the Ring Mod.

### S&H Distortion

Sample & Hold distortion grabs sample values and holds them for an amount of time depending on the Pitch. With the Pitch fully clockwise, it's taking the sample values at the current frequency for minimal bit-crushing. Pitch fully-counterclockwise essentially creates a square wave at unison by taking a sample at a rate an octave down and holding it. In between is serious bit crushing. Dry/Wet blends with the main Osc, the Ring Mod, and the Freq Shift.



## Filter Section

### The Filter
Filter mode is selectable: -12dB, -24dB, -48dB, and Notch. The Cutoff Frequency is controlled by a horizontal slider below the filter visualizer. The Filter Resonance is controlled by a vertical slider to the right of the visualizer.

### Filter ADSR

ADSR works just like you'd expect: Attack time, Decay time, Sustain frequency, Release time. Two rotary knobs control how much the ADSR is sent to the Cutoff Frequency, and Resonance amount.

### Filter LFO

LFO Shape morphs three shapes via horizontal slider below the visualizer: Sine wave, Square wave, Sawtooth wave. The Frequency vertical slider controls the speed of the LFO. The To Cutoff vertical slider controls how much the LFO affects the Filter Cutoff.



## Out

### Out Gain

Vertical slider controls the output gain from 0 to 2. Defaults to unity gain: 1.0

### Out Meter

Shows the output level. Any samples greater than 1 magnitude (digital 0 dB) causes the big red Clipping light to turn bright red.
