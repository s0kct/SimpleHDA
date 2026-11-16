#!/usr/bin/env python3
from pathlib import Path
import plistlib, shutil, subprocess
root = Path(__file__).resolve().parents[1]
stage = root / 'Build/LocalizationRoot'
if stage.exists(): shutil.rmtree(stage)
bundle = stage / 'Library/Audio/Plug-Ins/SimpleHDA/SimpleHDALocalization.bundle'
resources = bundle / 'Contents/Resources'
resources.mkdir(parents=True)
info = dict(CFBundleIdentifier='cn.laobamac.SimpleHDA.Localization', CFBundleName='SimpleHDALocalization',
            CFBundleVersion='1.0', CFBundlePackageType='BNDL', CFBundleDevelopmentRegion='en')
(bundle / 'Contents/Info.plist').write_bytes(plistlib.dumps(info))
for source in (root / 'Resources/AppleHDA.kext/Contents/Resources').glob('*.lproj'):
    if (source / 'Localizable.strings').exists():
        target = resources / source.name
        target.mkdir()
        shutil.copy2(source / 'Localizable.strings', target / 'Localizable.strings')
assert len(list(resources.glob('*.lproj'))) == 41
for path in stage.rglob('*'): path.chmod(0o755 if path.is_dir() else 0o644)
component = root / 'Build/LocalizationComponents.plist'
component.write_bytes(plistlib.dumps([dict(RootRelativeBundlePath=str(bundle.relative_to(stage)), BundleIsRelocatable=False, BundleIsVersionChecked=False, BundleHasStrictIdentifier=True, BundleOverwriteAction='upgrade')]))
subprocess.run(['pkgbuild', '--component-plist', str(component), '--root', str(stage), '--identifier', info['CFBundleIdentifier'],
                '--version', '1.0', '--install-location', '/', '--ownership', 'recommended',
                str(root / 'Build/SimpleHDA-Localization.pkg')], check=True)
