#ifndef NATIVE_OBJECTS_H
#define NATIVE_OBJECTS_H

#define RSDK_THIS(type) NativeEntity_##type *entity = (NativeEntity_##type *)objPtr

#include "MenuBG.hpp"
#include "TextLabel.hpp"
#include "SubMenuButton.hpp"
#include "FadeScreen.hpp"
#include "VirtualDPad.hpp"
#include "VirtualDPadM.hpp"
#include "RetroGameLoop.hpp"
#include "PauseMenu.hpp"
#include "MenuControl.hpp"
#include "SegaSplash.hpp"
#include "CWSplash.hpp"
#include "TitleScreen.hpp"

#endif // !NATIVE_OBJECTS_H