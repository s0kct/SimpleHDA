# SimpleHDA

**Bahasa Indonesia** · [English](README.md) · [Changelog](CHANGELOG.md)

**Audio AppleHDA untuk macOS Tahoe dengan pelokalan nama perangkat otomatis.**

[Instalasi](#instalasi) · [Argumen boot](#argumen-boot) · [HAL](#plugin-hal-applehda) · [Verifikasi](#pemeriksaan-setelah-boot-dan-rollback)

SimpleHDA 1.0.1 memulihkan audio AppleHDA melalui OpenCore. Universal Native memungkinkan nama perangkat di pengaturan Bunyi dan Audio MIDI Setup mengikuti bahasa pilihan macOS.

## Universal Native

Universal Native adalah fitur pelokalan otomatis nama perangkat audio pada proyek ini. SimpleHDA memublikasikan kunci nama AppleHDA dan mengarahkan `IOAudioDeviceLocalizedBundle` ke bundle berisi 41 katalog bahasa. CoreAudio menerjemahkan kunci tersebut berdasarkan bahasa pilihan macOS. Setelah bundle terpasang, pemilihan bahasa berlangsung otomatis. Lihat [Pemeriksaan setelah boot dan rollback](#pemeriksaan-setelah-boot-dan-rollback) untuk memverifikasi nama perangkat.

## Persyaratan

- macOS Tahoe 26 pada Intel atau AMD x86_64
- OpenCore dengan `Kernel → Quirks → DisableLinkeditJettison` aktif
- Lilu 1.7.1 atau lebih baru, AppleALC, dan `alcid` atau `layout-id` yang berfungsi

## Build

Setelah memasang Xcode, Git, dan Python 3, jalankan dari direktori utama repositori.

```sh
zsh Tools/build.zsh Release
python3 Tools/prepare_localization.py
```

| Hasil build | Fungsi |
| --- | --- |
| `Build/Release/SimpleHDA.kext` | Driver yang dimuat oleh OpenCore |
| `Build/SimpleHDA-Localization.pkg` | 41 katalog bahasa untuk Universal Native |
| `Build/SimpleHDA-HAL-1.0.1.pkg` | Plugin AppleHDA opsional untuk CoreAudio |

## Instalasi

1. Cadangkan EFI. Salin kext hasil build ke `EFI/OC/Kexts`. Di `Kernel → Add` OpenCore, gunakan urutan **Lilu → AppleALC → SimpleHDA** dan pertahankan layout audio yang berfungsi.

   | `Kernel → Add` | Nilai |
   | --- | --- |
   | `Enabled` | `true` |
   | `Arch` | `x86_64` |
   | `BundlePath` | `SimpleHDA.kext` |
   | `ExecutablePath` | `Contents/MacOS/SimpleHDA` |
   | `PlistPath` | `Contents/Info.plist` |
   | `MinKernel` | `25.0.0` |
   | `MaxKernel` | `25.99.99` |

2. Pasang bundle pelokalan jika belum tersedia. [Unduh paket instalasi](https://github.com/s0kct/SimpleHDA/raw/refs/heads/main/Universal-Native-Installer/SimpleHDA-Localization.pkg) atau gunakan paket hasil build di `Build/`. Jalankan perintah dari direktori tempat paket berada.

   ```sh
   sudo installer -pkg ./SimpleHDA-Localization.pkg -target /
   ```

3. Pertahankan konfigurasi HAL yang berfungsi. Paket opsional `SimpleHDA-HAL-1.0.1.pkg` menyediakan kontrol audio khusus AppleHDA. Pastikan hanya satu metode pemulihan AppleHDA yang aktif.
4. Mulai ulang Mac dan ikuti [langkah pemeriksaan setelah boot](#pemeriksaan-setelah-boot-dan-rollback).

Build ini tidak memerlukan `keepsyms=1` dan tidak memakai `shda-lang` atau `prev-lang:kbd` untuk memilih bahasa. Setelah mengganti bahasa pilihan di macOS, keluar dan masuk kembali atau mulai ulang Mac agar nama perangkat dimuat ulang.

## Plugin HAL AppleHDA

Hardware Abstraction Layer (HAL) adalah lapisan CoreAudio yang memberi aplikasi antarmuka yang seragam untuk mengakses perangkat audio. SimpleHDA dapat menyediakan plugin AppleHDA opsional di ruang pengguna melalui properti `IOAudioEngineCoreAudioPlugIn` pada mesin audio. Universal Native memublikasikan bundle bahasa secara terpisah melalui `IOAudioDeviceLocalizedBundle`. [Ikhtisar Core Audio dari Apple](https://developer.apple.com/library/archive/documentation/MusicAudio/Conceptual/CoreAudioOverview/WhatisCoreAudio/WhatisCoreAudio.html#//apple_ref/doc/uid/TP40003577-CH3-SW4)

Untuk memasang plugin opsional setelah build, jalankan `sudo installer -pkg Build/SimpleHDA-HAL-1.0.1.pkg -target /` lalu mulai ulang. Paket menempatkan bundle di `/Library/Audio/Plug-Ins/SimpleHDA/AppleHDAHALPlugIn.bundle`.

| `SimpleHDAHALMode` | Hasil pemeriksaan SimpleHDA |
| --- | --- |
| `auto` | Pengalihan HAL tersedia. Mesin audio belum memublikasikan properti plugin yang memicu deteksi. |
| `data-redirect` | `Info.plist` dan executable bundle terdeteksi sebagai berkas biasa yang tidak kosong. SimpleHDA mengarahkan properti mesin audio ke bundle terpasang. |
| `not-installed` | Pemeriksaan berkas gagal. SimpleHDA menghapus properti plugin sehingga jalur IOAudio standar tetap tersedia. |
| `disabled` | `-shdahaloff` menonaktifkan pengalihan dan jalur IOAudio standar tetap tersedia. |

Periksa properti ini setelah boot.

```sh
ioreg -l -w0 | grep -E 'SimpleHDAHAL(Mode|Path|Available)|IOAudioEngineCoreAudioPlugIn'
```

`data-redirect` menunjukkan bahwa driver sudah memublikasikan jalur plugin. Untuk menguji pengaruh plugin pada mesin Anda, pertahankan layout codec dan bundle pelokalan yang sama. Bandingkan pemutaran, perekaman, dan perpindahan input/output pada dua boot, pertama dengan deteksi otomatis dan berikutnya dengan `-shdahaloff`. [Kode deteksi HAL](Sources/SHDAHAL.cpp)

## Argumen boot

Di OpenCore, edit `NVRAM → Add → 7C436110-AB2A-4BBB-A880-FE41995C9F82 → boot-args`. Pertahankan argumen untuk komponen lain dan mulai ulang setelah perubahan.

| Argumen | Efek yang dapat diperiksa |
| --- | --- |
| `-shdaoff` | Menonaktifkan plugin SimpleHDA. Gunakan untuk membandingkan perilaku boot saat SimpleHDA dinonaktifkan. |
| `-shdahaloff` | Menonaktifkan pengalihan ke plugin HAL AppleHDA oleh SimpleHDA. Periksa apakah `SimpleHDAHALMode` bernilai `disabled`. |
| `-shdadbg` | Mengaktifkan log debug SimpleHDA pada kext Debug hasil `zsh Tools/build.zsh Debug`. |

## Pemeriksaan setelah boot dan rollback

```sh
ioreg -l -w0 | grep -E 'SimpleHDA(State|Detail|LocalizationMode|HALMode)'
ioreg -r -c IOAudioDevice -l -w0 | grep -E 'IOAudioDevice(Name|ShortName|LocalizedBundle)'
```

`stack-registered` menunjukkan stack AppleHDA terdaftar. `native-bundle-keys` menunjukkan mode pelokalan telah diinisialisasi. Periksa kunci nama dan jalur bundle yang berakhiran `SimpleHDALocalization.bundle` pada perangkat AppleHDA. Jika properti itu muncul, bandingkan nama perangkat di pengaturan Bunyi dengan dua bahasa pilihan setelah keluar dan masuk kembali. IORegistry menunjukkan kunci dan jalur bundle sudah dipublikasikan. Perbandingan di pengaturan Bunyi menguji apakah CoreAudio dapat menerjemahkannya.

Untuk rollback, boot dari cadangan EFI yang berfungsi atau pulihkan kext dan konfigurasi OpenCore sebelumnya, lalu mulai ulang.

## Kontribusi

- [laobamac](https://github.com/laobamac/SimpleHDA) membuat SimpleHDA dan integrasi AppleHDA awal.
- [s0kct](https://github.com/s0kct) memimpin pengerjaan pelokalan Universal Native dan menguji build SimpleHDA 1.0.1 di macOS Tahoe.

Lihat [pemberitahuan pihak ketiga](NOTICE.md).

Copyright © 2026 laobamac. All rights reserved.
