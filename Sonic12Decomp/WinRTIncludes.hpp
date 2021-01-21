#pragma once
#include <winrt/base.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Storage.h>

inline char *getResourcesPath()
{
    static char resourcePath[256] = { 0 };

    if (strlen(resourcePath) == 0) {
        auto folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        auto path   = to_string(folder.Path());

        std::copy(path.begin(), path.end(), resourcePath);
    }

    return resourcePath;
}

inline uint UAP_GetRetroGamePlatform()
{
    auto touchCaps = winrt::Windows::Devices::Input::TouchCapabilities();
    return touchCaps.TouchPresent() ? RETRO_MOBILE : RETRO_STANDARD;
}