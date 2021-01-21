#pragma once 
#include <SDL_main.h>

#include <windows.h>
#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
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
