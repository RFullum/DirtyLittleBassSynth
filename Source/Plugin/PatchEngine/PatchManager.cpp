/*
  ==============================================================================

    PatchManager.cpp

  ==============================================================================
*/

#include "PatchManager.h"

//==============================================================================

PatchManager::PatchManager(juce::AudioProcessorValueTreeState &apvtsToManage)
: apvts(apvtsToManage)
, userPatchesDir   (ResolveUserPatchesDirectory())
, factoryPatchesDir(ResolveFactoryPatchesDirectory())
{}

//==============================================================================

void PatchManager::Init()
{
    // Create the user patches directory on first run. createDirectory walks the
    // full path, so missing parents (FullumMusic/, Dirty Little Bass Synth/)
    // are created in a single call. Result is ignored intentionally — if
    // creation fails (read-only volume, permissions), RefreshPatchList just
    // returns an empty user-side list and the UI degrades gracefully.
    if (! userPatchesDir.exists())
        userPatchesDir.createDirectory();

    RefreshPatchList();
}

//==============================================================================

void PatchManager::RefreshPatchList()
{
    std::vector<PatchInfo> next;

    // Factory first so they sort to the top of the menu when the UI iterates
    // the list in order.
    ScanDirectoryInto(factoryPatchesDir, Source::Factory, next);
    ScanDirectoryInto(userPatchesDir,    Source::User,    next);

    patchList = std::move(next);
}

//==============================================================================

void PatchManager::ScanDirectoryInto(const juce::File &dir, Source source, std::vector<PatchInfo> &out) const
{
    if (! dir.isDirectory())
        return;

    // findChildFiles is a one-shot enumeration; cheap enough at the patch-list
    // sizes we expect (dozens, not thousands).
    auto files = dir.findChildFiles(juce::File::findFiles
                                    , /*searchRecursively*/ false
                                    , juce::String("*") + patchFileExtension);

    for (const auto &f : files)
    {
        PatchInfo info;
        info.file   = f;
        info.name   = f.getFileNameWithoutExtension();
        info.source = source;
        out.push_back(std::move(info));
    }
}

//==============================================================================

juce::File PatchManager::ResolveUserPatchesDirectory()
{
    // ~/Library/Application Support/FullumMusic/Dirty Little Bass Synth/Patches/
    // (and the OS-equivalents on Windows/Linux). One folder per company so all
    // FullumMusic plugins can share neighbours, then per-plugin so they don't
    // step on each other.
    auto base = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

   #if JUCE_MAC
    base = base.getChildFile("Application Support");
   #endif

    return base.getChildFile("FullumMusic")
               .getChildFile("Dirty Little Bass Synth")
               .getChildFile("Patches");
}

//==============================================================================

juce::File PatchManager::ResolveFactoryPatchesDirectory()
{
    // Factory patches live inside the plugin bundle's Resources/ on macOS, or
    // alongside the binary on other platforms. Path is computed eagerly even
    // though the directory may not exist yet — RefreshPatchList tolerates a
    // missing factory dir.
    auto exe = juce::File::getSpecialLocation(juce::File::currentApplicationFile);

   #if JUCE_MAC
    return exe.getChildFile("Contents/Resources/Patches");
   #else
    return exe.getParentDirectory().getChildFile("Patches");
   #endif
}
