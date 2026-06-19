cmake_minimum_required(VERSION 3.22)

# Must be set before project() so compiler/SDK checks pick them up.
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS deployment target")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS architectures (Universal Binary)")

project(DirtyLittleBassSynth VERSION 2.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Windows C++ runtime ------------------------------------------------------
# Statically link the MSVC runtime (/MT, /MTd for Debug) so the shipped VST3 and
# Standalone are self-contained and do NOT require end users to have the Visual
# C++ Redistributable installed. Set before add_subdirectory(JUCE) so every
# target — JUCE's static libs and our own sources alike — links the runtime the
# same way; a mismatch produces LNK4098 / runtime-heap corruption. Relies on
# policy CMP0091 (MSVC runtime selected via abstraction), default-NEW on
# CMake 3.15+.
if(WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# --- JUCE ---------------------------------------------------------------------
# JUCE 8 is vendored as a git submodule at ./JUCE (pinned to the same commit as
# the Center Space project for cross-project consistency).
add_subdirectory(JUCE)

# --- Plugin target ------------------------------------------------------------
# COPY_PLUGIN_AFTER_BUILD auto-installs the freshly-built plugin so it loads in a
# DAW without a manual copy. On macOS/Linux JUCE targets a user-writable dir
# (~/Library/Audio/Plug-Ins, ~/.vst3), so it just works. On Windows the only
# standard VST3 location is the SYSTEM path C:\Program Files\Common Files\VST3,
# which needs administrator rights — an unelevated build fails the post-build
# copy (copyDir.cmake / MSB3073). We package the zips straight from the build
# artefacts, so the auto-install is unnecessary on Windows; disable it there to
# keep non-admin builds green.
if(WIN32)
    set(DLBS_COPY_AFTER_BUILD FALSE)
else()
    set(DLBS_COPY_AFTER_BUILD TRUE)
endif()

# Plugin characteristics: synth, MIDI in + MIDI out.
#   manufacturer code "FulM"  plugin code "Dlbs"
# AU is listed for macOS completeness; on Linux only the VST3 and Standalone
# targets are built (AU is an Apple-only format and JUCE skips it off-Apple).
juce_add_plugin(DirtyLittleBassSynth
    PRODUCT_NAME                "Dirty Little Bass Synth"
    # Single-sourced from project(... VERSION) above. ProjectInfo::versionString
    # (used by the title footer) and JucePlugin_VersionString both derive from
    # this, so bumping the project() version is the only edit needed per release.
    VERSION                     ${PROJECT_VERSION}
    COMPANY_NAME                "FullumMusic"
    COMPANY_WEBSITE             "https://www.fullummusic.com/"
    BUNDLE_ID                   "com.FullumMusic.DirtyLittleBassSynth"
    PLUGIN_MANUFACTURER_CODE    FulM
    PLUGIN_CODE                 Dlbs
    # App/plugin icon. Primarily used by the Standalone (window/taskbar icon)
    # and baked into the macOS bundle.
    ICON_BIG                    "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FullumMusic_Icon_Spectrum_1024.png"
    ICON_SMALL                  "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FullumMusic_Icon_Spectrum_1024.png"
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
    COPY_PLUGIN_AFTER_BUILD     ${DLBS_COPY_AFTER_BUILD})

# Compile-time JUCE options:
#   JUCE_VST3_CAN_REPLACE_VST2=0   — DLBS never had a VST2 ancestor.
#   JUCE_STRICT_REFCOUNTEDPOINTER=1
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
    JUCE_REPORT_APP_USAGE=0
    # MSVC's <cmath> only exposes M_PI etc. when _USE_MATH_DEFINES is set before
    # <cmath> is first included. Oscillators.h defines it locally, but JuceHeader.h
    # pulls in <cmath> earlier in the translation unit, so the local define lands
    # after the include guard has already closed. Set it project-wide for MSVC so
    # it is in effect before any include. Inert on Clang/GCC (POSIX defines M_PI).
    $<$<CXX_COMPILER_ID:MSVC>:_USE_MATH_DEFINES>)

# --- Sources ------------------------------------------------------------------
# Glob the Source/ tree (CONFIGURE_DEPENDS re-globs when files are added/removed).
# 38 .cpp + 44 .h on disk; all live under Source/.
file(GLOB_RECURSE DLBS_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h")

target_sources(DirtyLittleBassSynth PRIVATE ${DLBS_SOURCES})

# Mirror on-disk folder structure in IDE navigators (Xcode, Visual Studio).
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Source" PREFIX "Source" FILES ${DLBS_SOURCES})

# --- FullumModules (navigator visibility only, NOT yet built) ------------------
# Shared modules live in the ./FullumModules git submodule. This block surfaces
# them in IDE navigators (Xcode, Visual Studio) as a top-level "FullumModules"
# group alongside "Source", so the tree is browsable while the migration that
# actually adopts each module proceeds incrementally.
#
# The files are added to the target purely for display: every .cpp is flagged
# HEADER_FILE_ONLY so the compiler skips it. Until a module is wired in (its
# include dir added and the in-tree Source/ copy deleted — see FullumModules/
# CLAUDE.md), compiling these would duplicate symbols already provided by Source/.
# As each module is adopted, drop its files from this glob (or remove the
# HEADER_FILE_ONLY flag once they replace the Source/ copy).
#
# Migration/ holds the migration prompts/notes (Markdown), not module code — the
# glob only picks up .cpp/.h, so it is naturally excluded.
file(GLOB_RECURSE FULLUM_MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/*.h")

target_sources(DirtyLittleBassSynth PRIVATE ${FULLUM_MODULE_SOURCES})

# Display-only: do not compile any FullumModules .cpp yet.
foreach(_fm_src IN LISTS FULLUM_MODULE_SOURCES)
    if(_fm_src MATCHES "\\.cpp$")
        set_source_files_properties("${_fm_src}" PROPERTIES HEADER_FILE_ONLY TRUE)
    endif()
endforeach()

# Adopted modules — compile these (overrides the display-only flag above).
# Add each module's .cpp here as it is migrated in and its Source/ copy deleted.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/State/PatchManager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/GUI/PatchControls.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/GUI/PatchSelectionPopup.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/GUI/FullumLookAndFeel.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/GUI/TitleFooter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/GUI/Selector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/Oscillators.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/Wavetable.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/SubOscillator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/Effects/RingMod.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/Effects/FrequencyShifter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules/DSP/Effects/SampleAndHold.cpp"
    PROPERTIES HEADER_FILE_ONLY FALSE)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/FullumModules" PREFIX "FullumModules"
    FILES ${FULLUM_MODULE_SOURCES})

# Source subdirectories on the include path so existing #include "Foo.h"
# directives keep working without per-file path prefixes.
target_include_directories(DirtyLittleBassSynth PRIVATE
    FullumModules            # root → shared modules referenced folder-rooted, e.g. "State/PatchManager.h"
    FullumModules/Libraries
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
    Source/GUI/MIDI
    FullumModules/State
    FullumModules/DSP)

# --- Embedded binary data (icon) ----------------------------------------------
# JUCE has no Linux app-icon mechanism (juceaide only emits .icns/.ico), so the
# Standalone window icon is set at runtime from this embedded PNG — see
# PluginEditor::ApplyStandaloneWindowIconIfNeeded(). juce_add_binary_data derives
# the symbol name from the file name (FullumMusic_Icon_Spectrum_1024_png), which
# is what that source references.
juce_add_binary_data(DirtyLittleBassSynth_Data
    SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/FullumMusic_Icon_Spectrum_1024.png")
# Static lib linked into the .so plugin targets — must be PIC on Linux.
set_target_properties(DirtyLittleBassSynth_Data PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# --- JuceHeader.h -------------------------------------------------------------
# Generated header lets sources keep using <JuceHeader.h> (41 files do).
juce_generate_juce_header(DirtyLittleBassSynth)

# --- Module linkage -----------------------------------------------------------
# juce_audio_plugin_client and juce_audio_processors are linked automatically by
# juce_add_plugin. The rest are the JUCE modules this plugin uses.
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
        DirtyLittleBassSynth_Data
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
elseif(WIN32)
    # Windows mirrors the Linux layout (cmake -E commands are generator-agnostic).
    # VST3: the .dll lands in <…>.vst3/Contents/x86_64-win/, so ../Resources
    # resolves to the bundle's Contents/Resources — the second probe path in
    # ResolveFactoryPatchesDirectory().
    if(TARGET DirtyLittleBassSynth_VST3)
        add_custom_command(TARGET DirtyLittleBassSynth_VST3 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_VST3>/../Resources/Patches"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${FACTORY_PRESETS_SRC}"
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_VST3>/../Resources/Patches"
            VERBATIM
            COMMENT "Copying factory presets into DirtyLittleBassSynth_VST3 (Windows)")
    endif()
    # Standalone: bare .exe, presets live next to the binary (first probe path).
    if(TARGET DirtyLittleBassSynth_Standalone)
        add_custom_command(TARGET DirtyLittleBassSynth_Standalone POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_Standalone>/Patches"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${FACTORY_PRESETS_SRC}"
                    "$<TARGET_FILE_DIR:DirtyLittleBassSynth_Standalone>/Patches"
            VERBATIM
            COMMENT "Copying factory presets next to DirtyLittleBassSynth_Standalone (Windows)")
    endif()
endif()

# --- Code signing (opt-in) ----------------------------------------------------
# Enables Developer ID signing + hardened runtime entitlements on each plugin
# bundle. Opt-in via the DLBS_SIGN_IDENTITY environment variable so contributors
# without an Apple Developer cert still get clean (ad-hoc "Sign to Run Locally")
# builds.
#
# Usage:
#   export DLBS_SIGN_IDENTITY="Developer ID Application: Robert Fullum (A26T94CMW9)"
#   cmake --build --preset=macos-release --target DirtyLittleBassSynth_All
#
# This step ONLY signs the .vst3 / .component / .app bundles. The .pkg installer
# is built and signed separately by scripts/package-and-notarize.sh (which also
# handles notarization + stapling). HARDENED_RUNTIME_ENABLED in juce_add_plugin
# above writes the matching Info.plist flag; the entitlements file below carries
# the disable-library-validation entitlement that AU/VST3 hosts require when
# loading us into their address space.
#
# Implementation note: JUCE's Xcode generator emits a built-in CodeSign build
# phase that ad-hoc signs the bundle ("Sign to Run Locally") AFTER any
# add_custom_command(POST_BUILD) we attach. So a custom codesign step gets
# silently overwritten. Instead, we override the Xcode CODE_SIGN_IDENTITY /
# entitlements attributes on the target — Xcode's built-in CodeSign step then
# applies our Developer ID with hardened runtime + entitlements directly.
if(APPLE)
    set(DLBS_SIGN_IDENTITY_ENV "$ENV{DLBS_SIGN_IDENTITY}")
    if(DLBS_SIGN_IDENTITY_ENV)
        set(DLBS_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/DirtyLittleBassSynth.entitlements")
        # Parse Team ID out of identity string of form
        # "Developer ID Application: Robert Fullum (A26T94CMW9)".
        string(REGEX MATCH "\\(([A-Z0-9]+)\\)" _team_match "${DLBS_SIGN_IDENTITY_ENV}")
        set(DLBS_TEAM_ID "${CMAKE_MATCH_1}")
        message(STATUS "DLBS: Developer ID signing ENABLED")
        message(STATUS "  Identity: ${DLBS_SIGN_IDENTITY_ENV}")
        message(STATUS "  Team ID:  ${DLBS_TEAM_ID}")

        foreach(plugin_target IN ITEMS DirtyLittleBassSynth_VST3 DirtyLittleBassSynth_AU DirtyLittleBassSynth_Standalone)
            if(TARGET ${plugin_target})
                set_target_properties(${plugin_target} PROPERTIES
                    XCODE_ATTRIBUTE_CODE_SIGN_STYLE                 "Manual"
                    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY              "${DLBS_SIGN_IDENTITY_ENV}"
                    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM                "${DLBS_TEAM_ID}"
                    XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS          "${DLBS_ENTITLEMENTS}"
                    XCODE_ATTRIBUTE_OTHER_CODE_SIGN_FLAGS           "--timestamp --options=runtime"
                    XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME         "YES")
            endif()
        endforeach()
    else()
        message(STATUS "DLBS: Developer ID signing disabled (set DLBS_SIGN_IDENTITY to enable)")
    endif()
endif()

# --- Per-config Xcode build settings ------------------------------------------
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
