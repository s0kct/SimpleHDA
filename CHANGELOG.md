# Changelog

Changes to Universal Native, the automatic device-name localization for SimpleHDA.

## [v1.0.1-universal-native](https://github.com/s0kct/SimpleHDA/tree/v1.0.1-universal-native)

Based on SimpleHDA 1.0.1 for macOS Tahoe 26 (x86_64).

### Added

- **Universal Native** publishes AppleHDA device-name keys and an `IOAudioDeviceLocalizedBundle` path. CoreAudio can use the preferred macOS language to resolve those keys.
- **Localization installer** provides 41 AppleHDA language catalogs under `/Library/Audio/Plug-Ins/SimpleHDA`. [Download SimpleHDA-Localization.pkg](https://github.com/s0kct/SimpleHDA/raw/refs/heads/main/Universal-Native-Installer/SimpleHDA-Localization.pkg)

### Changed

- Language selection follows macOS without using `shda-lang` or changing `prev-lang:kbd`.
- The port retains the SimpleHDA 1.0.1 boot requirements. Enable OpenCore's `DisableLinkeditJettison`. The build does not require `keepsyms=1`.

### Implementation changes from upstream 1.0.1

| File | Change |
| --- | --- |
| `Sources/SHDALocalization.cpp` and `.hpp` | Replace kernel locale selection with wrappers for `setDeviceName` and `setDeviceShortName`. Expand the truncated keys `DeviceN` and `DeviceS` and publish the localization bundle path. |
| `Sources/SHDAHAL.cpp` | Redirect writes to `IOAudioDeviceLocalizedBundle` from the AppleHDA payload to the installed resource bundle. The optional HAL plug-in has a separate path. |
| `Sources/kern_start.cpp` | Remove the upstream selector localization hook while retaining validation of the two device-name symbol bindings before payload linking. |
| `Tools/prepare_localization.py` and `Tests/BundleLookup.m` | Package 41 AppleHDA catalogs and verify Foundation lookup of `DeviceName` in each catalog. |

The fork starts from a source snapshot identical to upstream v1.0.1. [View the Universal Native implementation diff](https://github.com/s0kct/SimpleHDA/commit/c3cb9fedd85af2496d7a7957bd6b53f8614e5860).

### Verified

- The x86_64 Release build passed the kext metadata and startup-symbol checks. Foundation resolved `DeviceName` in all 41 catalogs.
- s0kct confirmed that the build booted and worked on their macOS Tahoe setup.

### Documentation

- English and Indonesian guides cover EFI installation, boot arguments, HAL detection, runtime checks, and rollback. [English](README.md) · [Bahasa Indonesia](README.id.md)
