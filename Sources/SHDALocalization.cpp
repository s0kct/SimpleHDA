// Native user-space localization: publish keys, never choose a kernel locale.
#include "SHDALocalization.hpp"
#include <Headers/kern_util.hpp>
#include <IOKit/IORegistryEntry.h>
namespace {
constexpr char resourceBundle[] = "../../../Library/Audio/Plug-Ins/SimpleHDA/SimpleHDALocalization.bundle";
using Setter = void (*)(IORegistryEntry *, const char *);
Setter nameSetter, shortSetter;
void publish(IORegistryEntry *device) {
    device->setProperty("IOAudioDeviceLocalizedBundle", resourceBundle);
}
void name(IORegistryEntry *device, const char *key) {
    publish(device);
    nameSetter(device, key && !strcmp(key, "DeviceN") ? "DeviceName" : key);
}
void shortName(IORegistryEntry *device, const char *key) {
    publish(device);
    shortSetter(device, key && !strcmp(key, "DeviceS") ? "DeviceShortName" : key);
}
}
namespace shda {
const char *localized(const char *key) { return key; }
void configureLocalization() {
    auto root = IORegistryEntry::getRegistryRoot();
    if (root) root->setProperty("SimpleHDALocalizationMode", "native-bundle-keys");
}
bool bindLocalizationSymbol(const char *symbol, uint64_t &address) {
    if (!strcmp(symbol, localizationImports[0])) {
        nameSetter = reinterpret_cast<Setter>(address);
        address = reinterpret_cast<uintptr_t>(name);
        return true;
    }
    if (!strcmp(symbol, localizationImports[1])) {
        shortSetter = reinterpret_cast<Setter>(address);
        address = reinterpret_cast<uintptr_t>(shortName);
        return true;
    }
    return false;
}
}
