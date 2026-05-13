/*
  ==============================================================================

    DryWet.h
    Created: 29 Apr 2020 10:13:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

#include <cmath>

//============================================================

/// Linear dry/wet crossfade. mix=0 returns dry, mix=1 returns wet.
/// Sums correctly for correlated/phase-related signals; gives a ~6 dB dip
/// at mix=0.5 for decorrelated signals.
inline float DryWetLinear(float dry, float wet, float mix)
{
    return dry * (1.0f - mix) + wet * mix;
}

//============================================================

/// Equal-power dry/wet crossfade using sin/cos curves. mix=0 returns dry,
/// mix=1 returns wet. Keeps perceived loudness roughly flat across the knob
/// for decorrelated signals (where linear would dip mid-travel).
inline float DryWetEP(float dry, float wet, float mix)
{
    const float theta = mix * 1.5707963267948966f; // mix * pi/2
    return dry * std::cos(theta) + wet * std::sin(theta);
}

