/*
  ==============================================================================

    TempoSnapshot.h

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <atomic>

//==============================================================================

/// Plain value-type snapshot of the current tempo / transport state. Updated
/// each block by the audio thread and read by the UI thread.
struct TempoInfo
{
    float bpm                = 120.0f;
    bool  isPlaying          = false;
    float ppqPosition        = 0.0f;
    int   timeSigNumerator   = 4;
    int   timeSigDenominator = 4;

    /// True when the BPM came from the host's playhead. False when the fallback
    /// param is in use (standalone, or any host that doesn't report tempo).
    bool  bpmFromHost        = false;
};

//==============================================================================

/// Atomic-backed snapshot that the audio thread writes to once per block and
/// the UI thread reads at refresh rate. Reads are not perfectly transactional
/// across all six fields (no locking, no version counter) — visually invisible
/// for tempo display, and accurate enough for the LFO sync math which only
/// needs the latest BPM/position pair.
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
