# SimpleHDA

**English** · [Bahasa Indonesia](README.id.md) · [Changelog](CHANGELOG.md)

**AppleHDA audio for macOS Tahoe with automatic device-name localization.**

[Installation](#installation) · [Boot arguments](#boot-arguments) · [HAL](#applehda-hal-plug-in) · [Verification](#runtime-checks-and-rollback)

SimpleHDA 1.0.1 restores AppleHDA audio through OpenCore. Universal Native lets the names shown in Sound and Audio MIDI Setup follow the preferred macOS language.

## Universal Native

Universal Native is this project's automatic audio device-name localization. SimpleHDA publishes the AppleHDA name keys and points `IOAudioDeviceLocalizedBundle` to the installed bundle of 41 language catalogs. CoreAudio resolves those keys using the preferred macOS language. Once the bundle is installed, language selection is automatic. See [Runtime checks and rollback](#runtime-checks-and-rollback) to verify the device names.

## Requirements

- macOS Tahoe 26 on Intel or AMD x86_64
- OpenCore with `Kernel → Quirks → DisableLinkeditJettison` enabled
- Lilu 1.7.1 or later, AppleALC, and a working `alcid` or `layout-id`

## Build

With Xcode, Git, and Python 3 installed, run from the repository root.

```sh
zsh Tools/build.zsh Release
python3 Tools/prepare_localization.py
```

| Build output | Purpose |
| --- | --- |
| `Build/Release/SimpleHDA.kext` | Driver loaded by OpenCore |
| `Build/SimpleHDA-Localization.pkg` | 41 language catalogs for Universal Native |
| `Build/SimpleHDA-HAL-1.0.1.pkg` | Optional AppleHDA plug-in for CoreAudio |

## Installation

1. Back up your EFI. Copy the built kext to `EFI/OC/Kexts`. In OpenCore's `Kernel → Add`, keep the order **Lilu → AppleALC → SimpleHDA** and retain your working audio layout.

   | `Kernel → Add` | Value |
   | --- | --- |
   | `Enabled` | `true` |
   | `Arch` | `x86_64` |
   | `BundlePath` | `SimpleHDA.kext` |
   | `ExecutablePath` | `Contents/MacOS/SimpleHDA` |
   | `PlistPath` | `Contents/Info.plist` |
   | `MinKernel` | `25.0.0` |
   | `MaxKernel` | `25.99.99` |

2. Install the localization bundle if it is missing. [Download the installer](https://github.com/s0kct/SimpleHDA/raw/refs/heads/main/Universal-Native-Installer/SimpleHDA-Localization.pkg) or use the package generated in `Build/`. Run the command from the directory containing the package.

   ```sh
   sudo installer -pkg ./SimpleHDA-Localization.pkg -target /
   ```

3. Keep your working HAL configuration. The optional `SimpleHDA-HAL-1.0.1.pkg` provides AppleHDA-specific audio controls. Check that only one AppleHDA restoration method is active.
4. Reboot and follow the [runtime checks](#runtime-checks-and-rollback).

The build does not require `keepsyms=1` and does not use `shda-lang` or `prev-lang:kbd` to select a language. After changing the preferred macOS language, sign out and back in or reboot to reload the audio names.

## AppleHDA HAL plug-in

The Hardware Abstraction Layer (HAL) is the part of CoreAudio that gives applications a common interface to audio devices. SimpleHDA can expose the optional AppleHDA user-space plug-in through the audio engine's `IOAudioEngineCoreAudioPlugIn` property. Universal Native publishes the language bundle separately through `IOAudioDeviceLocalizedBundle`. [Apple Core Audio overview](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/CoreAudioOverview/WhatisCoreAudio/WhatisCoreAudio.html#//apple_ref/doc/uid/TP40003577-CH3-SW4)

To install the optional plug-in after building, run `sudo installer -pkg Build/SimpleHDA-HAL-1.0.1.pkg -target /` and reboot. The installer places the bundle at `/Library/Audio/Plug-Ins/SimpleHDA/AppleHDAHALPlugIn.bundle`.

| `SimpleHDAHALMode` | What SimpleHDA observed |
| --- | --- |
| `auto` | HAL redirection is enabled. The audio engine has yet to publish the plug-in property that triggers detection. |
| `data-redirect` | The bundle's `Info.plist` and executable are regular, nonempty files. SimpleHDA redirects the engine property to the installed bundle. |
| `not-installed` | The file checks failed. SimpleHDA removes the plug-in property, leaving the ordinary IOAudio path available. |
| `disabled` | `-shdahaloff` disables the redirect and leaves the ordinary IOAudio path available. |

Check these properties after boot.

```sh
ioreg -l -w0 | grep -E 'SimpleHDAHAL(Mode|Path|Available)|IOAudioEngineCoreAudioPlugIn'
```

`data-redirect` shows that the driver published the path. To test whether the plug-in makes a difference on your machine, keep the codec layout and localization bundle fixed. Compare playback, recording, and input/output switching across two boots, first with automatic detection and then with `-shdahaloff`. [HAL detection code](Sources/SHDAHAL.cpp)

## Boot arguments

In OpenCore, edit `NVRAM → Add → 7C436110-AB2A-4BBB-A880-FE41995C9F82 → boot-args`. Keep existing arguments for other components and reboot after a change.

| Argument | Observable effect |
| --- | --- |
| `-shdaoff` | Disables the SimpleHDA plug-in. Use it to compare boot behavior with SimpleHDA disabled. |
| `-shdahaloff` | Disables SimpleHDA’s AppleHDA HAL plug-in redirection. Check `SimpleHDAHALMode` for `disabled`. |
| `-shdadbg` | Enables SimpleHDA debug logging with a Debug kext built using `zsh Tools/build.zsh Debug`. |

## Runtime checks and rollback

```sh
ioreg -l -w0 | grep -E 'SimpleHDA(State|Detail|LocalizationMode|HALMode)'
ioreg -r -c IOAudioDevice -l -w0 | grep -E 'IOAudioDevice(Name|ShortName|LocalizedBundle)'
```

`stack-registered` confirms registration of the AppleHDA stack. `native-bundle-keys` confirms that the localization mode initialized. Check the actual AppleHDA device for the keys and a bundle path ending in `SimpleHDALocalization.bundle`. If those properties appear, compare the displayed device names in Sound under two preferred languages after signing out and back in. IORegistry records publication of the keys and bundle path. The Sound comparison checks whether CoreAudio resolves them.

For rollback, boot from the working EFI backup or restore the previous kext and OpenCore configuration, then reboot.

## Contributions

- [laobamac](https://github.com/laobamac/SimpleHDA) created SimpleHDA and the original AppleHDA integration.
- [s0kct](https://github.com/s0kct) led the Universal Native localization work and verified the SimpleHDA 1.0.1 build on macOS Tahoe.

See the [third-party notices](NOTICE.md).

Copyright © 2026 laobamac. All rights reserved.
