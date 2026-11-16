//
//  SHDALocalization.hpp
//  SimpleHDA
//
//  Created by laobamac on 2026/8/28.
//  Copyright © 2026 laobamac. All rights reserved.
//
#ifndef SHDA_LOCALIZATION_HPP
#define SHDA_LOCALIZATION_HPP
#include <stddef.h>
#include <stdint.h>
class KernelPatcher;
namespace shda {
inline constexpr const char *localizationImports[] {
    "__ZN13IOAudioDevice13setDeviceNameEPKc",
    "__ZN13IOAudioDevice18setDeviceShortNameEPKc"

};
inline constexpr unsigned localizationImportCount = sizeof(localizationImports) / sizeof(localizationImports[0]);
const char *localized(const char *);
void configureLocalization();
bool bindLocalizationSymbol(const char *, uint64_t &);
}
#endif
