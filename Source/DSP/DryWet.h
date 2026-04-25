/*
  ==============================================================================

    DryWet.h
    Created: 29 Apr 2020 10:13:04pm
    Author:  Robert Fullum

  ==============================================================================
*/

#pragma once

//============================================================

/// Linear dry/wet crossfade. mix=0 returns dry, mix=1 returns wet.
inline float DryWetMix(float dry, float wet, float mix)
{
    return dry * (1.0f - mix) + wet * mix;
}
