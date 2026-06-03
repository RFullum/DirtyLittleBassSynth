#!/bin/bash
# ==============================================================================
# Dirty Little Bass Synth — Package + Notarize
# ==============================================================================
# Builds a signed, notarized, stapled .pkg installer for distribution.
# Ships VST3, AU, and the Standalone .app.
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

# Derived paths
readonly PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
readonly XCODE_PROJ="${PROJECT_ROOT}/Builds/MacOSX/Dirty Little Bass Synth.xcodeproj"
readonly DERIVED_DATA="${PROJECT_ROOT}/Builds/MacOSX/build/DerivedData"
# Final products land in SYMROOT (Builds/MacOSX/build/Release/), not in
# DerivedData/Build/Products — that's how the .jucer-generated pbxproj
# configures SYMROOT.
readonly BUILD_PRODUCTS="${PROJECT_ROOT}/Builds/MacOSX/build/Release"
readonly DIST_DIR="${PROJECT_ROOT}/dist"
readonly STAGING_DIR="${DIST_DIR}/staging"
readonly JUCER_FILE="${PROJECT_ROOT}/Wavetable5.jucer"

# ---- Helpers -----------------------------------------------------------------

err() { echo "ERROR: $*" >&2; exit 1; }
log() { echo "==> $*"; }

# ---- Preflight ---------------------------------------------------------------

command -v xcodebuild  >/dev/null || err "xcodebuild not found"
command -v codesign    >/dev/null || err "codesign not found"
command -v pkgbuild    >/dev/null || err "pkgbuild not found"
command -v xcrun       >/dev/null || err "xcrun not found"

security find-identity -v -p codesigning | grep -q "${SIGN_IDENTITY}" \
    || err "Application identity not found: ${SIGN_IDENTITY}"
security find-identity -v -p basic | grep -q "${INSTALLER_IDENTITY}" \
    || err "Installer identity not found: ${INSTALLER_IDENTITY}"

xcrun notarytool history --keychain-profile "${NOTARY_PROFILE}" >/dev/null 2>&1 \
    || err "Notarytool profile '${NOTARY_PROFILE}' not found in keychain"

# Parse version from .jucer (top-level <JUCERPROJECT version="X.Y.Z" ...>)
VERSION=$(grep -oE 'version="[0-9]+\.[0-9]+\.[0-9]+"' "${JUCER_FILE}" \
    | head -1 | sed -E 's/version="(.*)"/\1/')
[[ -n "${VERSION}" ]] || err "Could not parse version from ${JUCER_FILE}"
log "Building version ${VERSION}"

log "Application identity: ${SIGN_IDENTITY}"
log "Installer identity:   ${INSTALLER_IDENTITY}"

# ---- Step 1: Build Release (signed during build) -----------------------------
# CODE_SIGN_IDENTITY override is picked up by Projucer's per-target shell-script
# codesign phase via ${EXPANDED_CODE_SIGN_IDENTITY:-${CODE_SIGN_IDENTITY}}.
# ARCHS="arm64 x86_64" + ONLY_ACTIVE_ARCH=NO forces a universal binary even
# though the .jucer is configured 64BitIntel — the pbxproj's VALID_ARCHS allows
# arm64.

build_target() {
    local target="$1"
    log "Building target: ${target}"
    xcodebuild \
        -project "${XCODE_PROJ}" \
        -scheme "${target}" \
        -configuration Release \
        -derivedDataPath "${DERIVED_DATA}" \
        CODE_SIGN_IDENTITY="${SIGN_IDENTITY}" \
        CODE_SIGN_STYLE=Manual \
        DEVELOPMENT_TEAM="${TEAM_ID}" \
        ARCHS="arm64 x86_64" \
        ONLY_ACTIVE_ARCH=NO \
        CODE_SIGN_INJECT_BASE_ENTITLEMENTS=NO \
        2>&1 | grep -E "error:|fatal error|Signing Identity:|BUILD " | sed 's/^/    /' \
        || true
}

build_target "Dirty Little Bass Synth - VST3"
build_target "Dirty Little Bass Synth - AU"
build_target "Dirty Little Bass Synth - Standalone Plugin"

# ---- Step 2: Verify bundles & signatures -------------------------------------

verify_bundle() {
    local bundle="$1"
    [[ -e "${bundle}" ]] || err "Missing bundle: ${bundle}"
    log "Verifying signature: $(basename "${bundle}")"
    codesign --verify --strict --verbose=2 "${bundle}" 2>&1 | sed 's/^/    /'
    codesign -dv --verbose=2 "${bundle}" 2>&1 \
        | grep -E "Authority|TeamIdentifier|Identifier|flags" \
        | sed 's/^/    /'
}

VST3_PATH="${BUILD_PRODUCTS}/${BUNDLE_VST3}"
AU_PATH="${BUILD_PRODUCTS}/${BUNDLE_AU}"
APP_PATH="${BUILD_PRODUCTS}/${BUNDLE_APP}"

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

log "Done. Output: ${PKG_PATH}"
