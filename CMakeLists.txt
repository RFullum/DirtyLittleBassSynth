cmake_minimum_required(VERSION 3.22)

# Must be set before project() so compiler/SDK checks pick them up.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures (Universal Binary)")

project(DirtyLittleBassSynth VERSION 2.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- JUCE ---------------------------------------------------------------------
# JUCE 8 is vendored as a git submodule at ./JUCE (pinned to the same commit as
# the Center Space project for cross-project consistency).
add_subdirectory(JUCE)

# --- Plugin target ------------------------------------------------------------
# Settings mirror Wavetable5.jucer:
#   pluginCharacteristicsValue="pluginIsSynth,pluginProducesMidiOut,pluginWantsMidiIn"
#   pluginManufacturerCode="FulM"  pluginCode="Dlbs"
# AU is listed for macOS completeness; on Linux only the VST3 and Standalone
# targets are built (AU is an Apple-only format and JUCE skips it off-Apple).
juce_add_plugin(DirtyLittleBassSynth
    PRODUCT_NAME                "Dirty Little Bass Synth"
    VERSION                     2.0.0
    COMPANY_NAME                "FullumMusic"
    COMPANY_WEBSITE             "https://www.fullummusic.com/"
    BUNDLE_ID                   "com.FullumMusic.DirtyLittleBassSynth"
    PLUGIN_MANUFACTURER_CODE    FulM
    PLUGIN_CODE                 Dlbs
    FORMATS                     VST3 AU Standalone
    DESCRIPTION                 "Monophonic wavetable synth for getting quick and dirty bass and sub sounds."
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           TRUE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             Instrument Synth
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    HARDENED_RUNTIME_ENABLED    TRUE
    COPY_PLUGIN_AFTER_BUILD     TRUE)

# Compile-time JUCE options. Mirrors the .jucer JUCEOPTIONS plus a couple of
# build-system specifics:
#   JUCE_VST3_CAN_REPLACE_VST2=0   — DLBS never had a VST2 ancestor (from .jucer).
#   JUCE_STRICT_REFCOUNTEDPOINTER=1 — from .jucer.
#   JUCE_WEB_BROWSER=0             — DLBS has no WebBrowserComponent. Disabling it
#       drops the GTK/WebKit dependency that juce_gui_extra otherwise pulls in on
#       Linux (juce_gui_extra.cpp -> <gtk/gtk.h>). Inert on macOS.
#   JUCE_DISPLAY_SPLASH_SCREEN=0 / JUCE_REPORT_APP_USAGE=0
#       — disabled under the FullumMusic JUCE Pro license. (The existing macOS
#         release still ships with the splash; it should be rebuilt to match.)
target_compile_definitions(DirtyLittleBassSynth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    JUCE_WEB_BROWSER=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0)

# --- Sources ------------------------------------------------------------------
# Glob the Source/ tree (CONFIGURE_DEPENDS re-globs when files are added/removed).
# 38 .cpp + 44 .h on disk; all live under Source/.
file(GLOB_RECURSE DLBS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")

target_sources(DirtyLittleBassSynth PRIVATE ${DLBS_SOURCES})

# Mirror on-disk folder structure in IDE navigators (Xcode, Visual Studio).
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source" FILES ${DLBS_SOURCES})

# Source subdirectories on the include path so existing #include "Foo.h"
# directives keep working without per-file path prefixes (mirrors the .jucer
# headerPath list).
target_include_directories(DirtyLittleBassSynth PRIVATE
    Source/Plugin
    Source/Plugin/PatchEngine
    Source/DSP
    Source/GUI
    Source/GUI/Sources
    Source/GUI/Filter
    Source/GUI/Modifiers
    Source/GUI/Master
    Source/GUI/GUIUtils
    Source/GUI/TitleHeaderFooter
    Source/GUI/MIDI)

# --- JuceHeader.h -------------------------------------------------------------
# Generated header lets sources keep using <JuceHeader.h> (41 files do).
juce_generate_juce_header(DirtyLittleBassSynth)

# --- Module linkage -----------------------------------------------------------
# juce_audio_plugin_client and juce_audio_processors are linked automatically by
# juce_add_plugin. The rest mirror the .jucer MODULES list.
# Warning suppressions are applied AFTER juce_recommended_warning_flags so they
# sit last on the command line. Clang-gated, so inert under Linux GCC.
add_library(dlbs_warning_overrides INTERFACE)
target_compile_options(dlbs_warning_overrides INTERFACE
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-shadow-field-in-constructor>
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-Wno-inconsistent-missing-destructor-override>)

target_link_libraries(DirtyLittleBassSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors_headless
        juce::juce_dsp
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_cryptography
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        dlbs_warning_overrides)

# --- Factory presets ----------------------------------------------------------
# 30 *.dlbs files in Resources/FactoryPresets/ get copied into each built
# artifact so PatchManager::ResolveFactoryPatchesDirectory() finds them at
# runtime:
#   macOS    — <bundle>/Contents/Resources/Patches  (VST3, AU, Standalone .app)
#   Linux    — VST3:  <…>.vst3/Contents/Resources/Patches  (bundle layout)
#              Standalone: Patches/ next to the executable
# The Linux paths match the dual-probe logic in ResolveFactoryPatchesDirectory().
set(FACTORY_PRESETS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FactoryPresets")

if(APPLE)
    foreach(plugin_target IN ITEMS DirtyLittleBassSynth_VST3 DirtyLittleBassSynth_AU DirtyLittleBassSynth_Standalone)
        if(TARGET ${plugin_target})
            add_custom_command(TARGET ${plugin_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                        "$<TARGET_BUNDLE_DIR:${plugin_target}>/Contents/Resources/Patches"
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${FACTORY_PRESETS_SRC}"
                        "$<TARGET_BUNDLE_DIR:${plugin_target}>/Contents/Resources/Patches"
                VERBATIM
                COMMENT "Copying factory presets into ${plugin_target}")
        endif()
    endforeach()
elseif(UNIX)
    # VST3: the .so lands in <…>.vst3/Contents/<arch>-linux/, so ../Resources
    # resolves to the bundle's Contents/Resources.
    if(TARGET DirtyLittleBassSynth_VST3)
        add_custom_command(TARGET DirtyLittleBassSynth_VST3 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_VST3>/../Resources/Patches"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${FACTORY_PRESETS_SRC}"
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_VST3>/../Resources/Patches"
            VERBATIM
            COMMENT "Copying factory presets into DirtyLittleBassSynth_VST3 (Linux)")

        # COPY_PLUGIN_AFTER_BUILD installs the bundle to ~/.vst3 via JUCE's own
        # POST_BUILD step, which runs BEFORE this one and so misses the presets
        # staged above. Sync them into the installed copy too, so a freshly
        # built VST3 loaded straight from ~/.vst3 has its factory patches.
        # (Release tarballs are built from the build-tree artifact, which is
        # already complete — this only fixes the dev auto-install.)
        if(DEFINED ENV{HOME})
            set(_installed_vst3_patches "$ENV{HOME}/.vst3/Dirty Little Bass Synth.vst3/Contents/Resources/Patches")
            add_custom_command(TARGET DirtyLittleBassSynth_VST3 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory "${_installed_vst3_patches}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory
                        "${FACTORY_PRESETS_SRC}" "${_installed_vst3_patches}"
                VERBATIM
                COMMENT "Syncing factory presets into installed ~/.vst3 copy (Linux dev convenience)")
        endif()
    endif()
    # Standalone: bare executable, presets live next to the binary.
    if(TARGET DirtyLittleBassSynth_Standalone)
        add_custom_command(TARGET DirtyLittleBassSynth_Standalone POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_Standalone>/Patches"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${FACTORY_PRESETS_SRC}"
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_Standalone>/Patches"
            VERBATIM
            COMMENT "Copying factory presets next to DirtyLittleBassSynth_Standalone (Linux)")
    endif()
endif()

# --- Per-config Xcode build settings ------------------------------------------
# Mirrors the .jucer Xcode configurations:
#   Debug   — host arch only (fast iteration), -O0
#   Release — Universal Binary (arm64 + x86_64), -O3, LTO
foreach(plugin_target IN ITEMS DirtyLittleBassSynth DirtyLittleBassSynth_Standalone
                               DirtyLittleBassSynth_VST3 DirtyLittleBassSynth_AU)
    if(TARGET ${plugin_target})
        set_target_properties(${plugin_target} PROPERTIES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Debug]          YES
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH[variant=Release]        NO
            XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Debug]    0
            XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release]  3)
    endif()
endforeach()
