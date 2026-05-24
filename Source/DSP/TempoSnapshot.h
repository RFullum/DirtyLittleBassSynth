/*
  ==============================================================================

    TempoSnapshot.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>

//==============================================================================

struct TempoInfo
{
    float bpm                = 120.0f;
    bool  isPlaying          = false;
    float ppqPosition        = 0.0f;
    int   timeSigNumerator   = 4;
    int   timeSigDenominator = 4;

    // True when the BPM came from the host's playhead. False when the fallback
    // param is in use (standalone, or any host that doesn't report tempo).
    bool  bpmFromHost        = false;
};

//==============================================================================

class TempoSnapshot
{
public:
    void Update(const TempoInfo &info) noexcept
    {
        bpm                .store(info.bpm,                std::memory_order_relaxed);
        isPlaying          .store(info.isPlaying,          std::memory_order_relaxed);
        ppqPosition        .store(info.ppqPosition,        std::memory_order_relaxed);
        timeSigNumerator   .store(info.timeSigNumerator,   std::memory_order_relaxed);
        timeSigDenominator .store(info.timeSigDenominator, std::memory_order_relaxed);
        bpmFromHost        .store(info.bpmFromHost,        std::memory_order_relaxed);
    }

    TempoInfo Read() const noexcept
    {
        TempoInfo info;
        info.bpm                = bpm                .load(std::memory_order_relaxed);
        info.isPlaying          = isPlaying          .load(std::memory_order_relaxed);
        info.ppqPosition        = ppqPosition        .load(std::memory_order_relaxed);
        info.timeSigNumerator   = timeSigNumerator   .load(std::memory_order_relaxed);
        info.timeSigDenominator = timeSigDenominator .load(std::memory_order_relaxed);
        info.bpmFromHost        = bpmFromHost        .load(std::memory_order_relaxed);
        return info;
    }

private:
    std::atomic<float> bpm                 { 120.0f };
    std::atomic<bool>  isPlaying           { false  };
    std::atomic<float> ppqPosition         { 0.0f   };
    std::atomic<int>   timeSigNumerator    { 4      };
    std::atomic<int>   timeSigDenominator  { 4      };
    std::atomic<bool>  bpmFromHost         { false  };
};

//==============================================================================

inline float SubdivisionInQuarters(int index) noexcept
{
    constexpr float table[] = {
        4.0f,           // 1/1
        2.0f,           // 1/2
        1.0f,           // 1/4
        1.5f,           // 1/4D (dotted = ×1.5)
        2.0f / 3.0f,    // 1/4T (triplet = ×2/3)
        0.5f,           // 1/8
        0.75f,          // 1/8D
        1.0f / 3.0f,    // 1/8T
        0.25f,          // 1/16
        0.375f,         // 1/16D
        1.0f / 6.0f,    // 1/16T
        0.125f          // 1/32
    };

    constexpr int count = (int) (sizeof(table) / sizeof(table[0]));
    return table[juce::jlimit(0, count - 1, index)];
}
