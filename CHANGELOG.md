# Changelog

## Unreleased

### Native localization for SimpleHDA 1.0.1

- Publish AppleHDA device names as localization keys and point `IOAudioDeviceLocalizedBundle` to a resource bundle containing 41 AppleHDA catalogs. macOS selects the language without `shda-lang` or changes to `prev-lang:kbd`.
- Preserve the upstream 1.0.1 startup behavior. Enable OpenCore's `DisableLinkeditJettison`. The build does not require `keepsyms=1`.
- Build the x86_64 Release kext with `zsh Tools/build.zsh Release`. Generate `Build/SimpleHDA-Localization.pkg` with `python3 Tools/prepare_localization.py`. The package installs the resources under `/Library/Audio/Plug-Ins/SimpleHDA` and can be reused across builds with the same catalogs.
- Verified the Release build and Foundation lookup of `DeviceName` in all 41 catalogs. The user confirmed that this build booted and worked on their Tahoe setup.
