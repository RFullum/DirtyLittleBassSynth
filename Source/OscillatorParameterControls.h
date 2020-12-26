/*
  ==============================================================================

    OscillatorParameterControls.h
    Created: 28 Apr 2020 4:17:05pm
    Author:  B150987

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


/// Class to control parameters used by Main Osc
class OscParamControl
{
public:
    /**
     Takes the oscillatorMorph value and converts to sinWT gain level:
     value 0 = amplitude 1; val 1 = amp 0; val 2 = amp 0
     */
    float sinMorphGain(std::atomic<float>* oscMorphVal);

    
    /**
     Takes the oscillatorMorph value and converts to spikeWT gain level:
     val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
     */
    float spikeMorphGain(std::atomic<float>* oscMorphVal);
    
    /**
     Takes the oscillatorMorph value and converts to sawWT gain level
     val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
     */
    float sawMorphGain(std::atomic<float>* oscMorphVal);
    
private:
    /**
     Maps parameter value to gain value based on controlVal
     Values 0 to 2 :: amplitudes 0 to 1
     */
    float levelFormula(std::atomic<float>* MV, float CV);

};



//=================================================================
//=================================================================



/// Class to control parameters used by Sub Osc. Child of OscParamControl
class SubOscParamControl : public OscParamControl
{
public:
    /**
    Takes the subOscMorph value and converts to sinSub's gain level:
    val 0 = amp 1; val 1 = amp 0; val 2 = amp 0
    */
    float sinSubGain(std::atomic<float>* subMorphVal);
    
    /**
    Takes the subOscMorph value and converts to squareSub gain level:
    val 0 = amp 0; val 1 = amp 1; val 2 = amp 0
    */
    float squareSubGain(std::atomic<float>* subMorphVal);
    
    /**
    Takes the subOscMorph value and converts to sawSub gain level
    val 0 = amp 0; val 1 = amp 0; val 2 = amp 1
    */
    float sawSubGain(std::atomic<float>* subMorphVal);
    
    /**
     Converts parameter to denominator for octave frequency: 1, 2, or 4
     */
    int subOctaveSelector(std::atomic<float>* subOctVal);
    
};
