#!/bin/bash
# ==============================================================================
# Dirty Little Bass Synth — Package + Notarize  (CMake build)
# ==============================================================================
# Builds a signed, notarized, stapled .pkg installer for distribution.
# Ships VST3, AU, and the Standalone .app.
#
# Build system: CMake (the Projucer/.jucer build has been retired). The plugin
# bundles are Developer-ID signed *during the build* by the code-signing block in
# CMakeLists.txt, which activates when the DLBS_SIGN_IDENTITY environment
# variable is set. This script exports it automatically (from SIGN_IDENTITY
# below), so you do not need to export it yourself.
#
# Prereqs (one-time):
#   - Developer ID Application + Developer ID Installer certs in login keychain
#   - notarytool credentials stored under profile "centerspace-notary"
#     (reused from the Center-Space project — same Apple ID / team).
#     Re-create with:
#       xcrun notarytool store-credentials "centerspace-notary" \
#         --apple-id <you@email> --team-id A26T94CMW9 --password <app-spec-pw>
#
# Usage:
#   ./scripts/package-and-notarize.sh
#
# Output:
#   dist/DirtyLittleBassSynth-<version>.pkg  (signed, notarized, stapled)
# ==============================================================================

set -euo pipefail

# ---- Config ------------------------------------------------------------------

readonly PRODUCT_NAME="Dirty Little Bass Synth"
readonly BUNDLE_VST3="${PRODUCT_NAME}.vst3"
readonly BUNDLE_AU="${PRODUCT_NAME}.component"
readonly BUNDLE_APP="${PRODUCT_NAME}.app"
readonly PKG_IDENTIFIER="com.FullumMusic.DirtyLittleBassSynth.pkg"
readonly NOTARY_PROFILE="centerspace-notary"
readonly TEAM_ID="A26T94CMW9"
readonly SIGN_IDENTITY="Developer ID Application: Robert Fullum (${TEAM_ID})"
readonly INSTALLER_IDENTITY="Developer ID Installer: Robert Fullum (${TEAM_ID})"

# CMake targets to build (Release, universal). Standalone is shipped too, so —
# unlike Center Space — we build all three formats here.
readonly TARGET_VST3="DirtyLittleBassSynth_VST3"
readonly TARGET_AU="DirtyLittleBassSynth_AU"
readonly TARGET_APP="DirtyLittleBassSynth_Standalone"

# Derived paths
readonly PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly CMAKELISTS="${PROJECT_ROOT}/CMakeLists.txt"
readonly BUILD_DIR="${PROJECT_ROOT}/build/macos"
readonly ARTEFACTS_DIR="${BUILD_DIR}/DirtyLittleBassSynth_artefacts/Release"
readonly DIST_DIR="${PROJECT_ROOT}/dist"
readonly STAGING_DIR="${DIST_DIR}/staging"

# ---- Helpers -----------------------------------------------------------------

err() { echo "ERROR: $*" >&2; exit 1; }
log() { echo "==> $*"; }

# ---- Preflight ---------------------------------------------------------------

command -v cmake     >/dev/null || err "cmake not found"
command -v codesign  >/dev/null || err "codesign not found"
command -v pkgbuild  >/dev/null || err "pkgbuild not found"
command -v xcrun     >/dev/null || err "xcrun not found"

security find-identity -v -p codesigning | grep -q "${SIGN_IDENTITY}" \
    || err "Application identity not found: ${SIGN_IDENTITY}"
security find-identity -v -p basic | grep -q "${INSTALLER_IDENTITY}" \
    || err "Installer identity not found: ${INSTALLER_IDENTITY}"

xcrun notarytool history --keychain-profile "${NOTARY_PROFILE}" >/dev/null 2>&1 \
    || err "Notarytool profile '${NOTARY_PROFILE}' not found in keychain"

# Parse version from CMakeLists.txt — the single source of truth now that the
# .jucer is gone. Line form: project(DirtyLittleBassSynth VERSION X.Y.Z ...).
VERSION=$(grep -E "^project\(DirtyLittleBassSynth VERSION" "${CMAKELISTS}" \
    | sed -E 's/.*VERSION ([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
[[ -n "${VERSION}" ]] || err "Could not parse version from ${CMAKELISTS}"
log "Building version ${VERSION}"

log "Application identity: ${SIGN_IDENTITY}"
log "Installer identity:   ${INSTALLER_IDENTITY}"

# ---- Step 1: Build Release (signed during build) -----------------------------
# Signing is driven by the DLBS_SIGN_IDENTITY env var, which the CMakeLists.txt
# code-signing block reads at configure time and turns into Xcode
# CODE_SIGN_IDENTITY / entitlements / hardened-runtime attributes on each target.
# Export it here so the configure below activates Developer-ID signing.
export DLBS_SIGN_IDENTITY="${SIGN_IDENTITY}"

log "Configuring CMake (Release; signing enabled via DLBS_SIGN_IDENTITY)"
# Full configure output is shown so the "DLBS: Developer ID signing ENABLED"
# status lines are visible and a configure failure aborts (set -e).
cmake --preset=macos

log "Building ${TARGET_VST3} (Release, signed)"
cmake --build --preset=macos-release --target "${TARGET_VST3}"
log "Building ${TARGET_AU} (Release, signed)"
cmake --build --preset=macos-release --target "${TARGET_AU}"
log "Building ${TARGET_APP} (Release, signed)"
cmake --build --preset=macos-release --target "${TARGET_APP}"

# ---- Step 2: Verify bundles & signatures -------------------------------------
# We display the signing authority + hardened-runtime flag for each bundle but
# do NOT try to extract entitlements with `codesign -d --entitlements`: its
# output format varies across macOS releases (false negatives on macOS 26+), and
# plugin bundles intentionally carry no embedded entitlements anyway (only the
# Standalone .app does — verified against the shipped, notarized Center Space
# pkg). If signing is actually broken, notarytool rejects the submission with a
# precise error and `xcrun notarytool log <id>` shows what to fix.

verify_bundle() {
    local bundle="$1"
    [[ -e "${bundle}" ]] || err "Missing bundle: ${bundle}"
    log "Verifying signature: $(basename "${bundle}")"
    codesign --verify --strict --verbose=2 "${bundle}" 2>&1 | sed 's/^/    /'
    codesign -dv --verbose=2 "${bundle}" 2>&1 \
        | grep -E "Authority|TeamIdentifier|Identifier|flags" \
        | sed 's/^/    /'
}

VST3_PATH="${ARTEFACTS_DIR}/VST3/${BUNDLE_VST3}"
AU_PATH="${ARTEFACTS_DIR}/AU/${BUNDLE_AU}"
APP_PATH="${ARTEFACTS_DIR}/Standalone/${BUNDLE_APP}"

verify_bundle "${VST3_PATH}"
verify_bundle "${AU_PATH}"
verify_bundle "${APP_PATH}"

# ---- Step 3: Stage -----------------------------------------------------------
# Layout mirrors install destinations:
#   /Library/Audio/Plug-Ins/VST3/Dirty Little Bass Synth.vst3
#   /Library/Audio/Plug-Ins/Components/Dirty Little Bass Synth.component
#   /Applications/Dirty Little Bass Synth.app

log "Staging bundles for pkg"
rm -rf "${STAGING_DIR}"
mkdir -p "${STAGING_DIR}/Library/Audio/Plug-Ins/VST3"
mkdir -p "${STAGING_DIR}/Library/Audio/Plug-Ins/Components"
mkdir -p "${STAGING_DIR}/Applications"

# ditto preserves extended attributes / code signatures across the copy.
ditto "${VST3_PATH}" "${STAGING_DIR}/Library/Audio/Plug-Ins/VST3/${BUNDLE_VST3}"
ditto "${AU_PATH}"   "${STAGING_DIR}/Library/Audio/Plug-Ins/Components/${BUNDLE_AU}"
ditto "${APP_PATH}"  "${STAGING_DIR}/Applications/${BUNDLE_APP}"

# ---- Step 4: pkgbuild --------------------------------------------------------
# pkgbuild --root packages the staging tree literally — VST3 bundles survive
# (productbuild --root would silently drop them). pkgbuild signs the pkg too,
# so no separate productbuild step.

mkdir -p "${DIST_DIR}"
PKG_NAME="DirtyLittleBassSynth-${VERSION}.pkg"
PKG_PATH="${DIST_DIR}/${PKG_NAME}"

log "Building ${PKG_NAME}"
pkgbuild \
    --root "${STAGING_DIR}" \
    --identifier "${PKG_IDENTIFIER}" \
    --version "${VERSION}" \
    --install-location "/" \
    --sign "${INSTALLER_IDENTITY}" \
    --timestamp \
    "${PKG_PATH}"

log "Verifying pkg signature"
pkgutil --check-signature "${PKG_PATH}" | sed 's/^/    /'

# ---- Step 5: Notarize --------------------------------------------------------

log "Submitting to Apple notary service (this can take a few minutes)"
xcrun notarytool submit "${PKG_PATH}" \
    --keychain-profile "${NOTARY_PROFILE}" \
    --wait

# ---- Step 6: Staple ----------------------------------------------------------

log "Stapling ticket"
xcrun stapler staple "${PKG_PATH}"
xcrun stapler validate "${PKG_PATH}"

# ---- Step 7: Gatekeeper assess ----------------------------------------------

log "Gatekeeper assessment"
spctl --assess --type install --verbose=2 "${PKG_PATH}" 2>&1 | sed 's/^/    /'

# ---- Done --------------------------------------------------------------------

log "Cleaning staging directory"
rm -rf "${STAGING_DIR}"

echo
echo "=============================================================="
echo "  SUCCESS"
echo "  ${PKG_PATH}"
echo "  Signed, notarized, stapled, Gatekeeper-approved."
echo "=============================================================="
