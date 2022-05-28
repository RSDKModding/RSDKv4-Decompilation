#ifndef NATIVE_OBJECTS_H
#define NATIVE_OBJECTS_H

#define RSDK_THIS(type)     NativeEntity_##type *self = (NativeEntity_##type *)objPtr
#define CREATE_ENTITY(type) ((NativeEntity_##type *)CreateNativeObject(type##_Create, type##_Main))

extern bool usePhysicalControls;
extern byte timeAttackTex;
extern ushort helpText[0x1000];

#include "MenuBG.hpp"
#include "TextLabel.hpp"
#include "PushButton.hpp"
#include "SubMenuButton.hpp"
#include "DialogPanel.hpp"
#include "FadeScreen.hpp"
#include "VirtualDPad.hpp"
#include "VirtualDPadM.hpp"
#include "SettingsScreen.hpp"
#include "RetroGameLoop.hpp"
#include "PauseMenu.hpp"
#include "SegaSplash.hpp"
#include "CWSplash.hpp"
#include "TitleScreen.hpp"
#include "StartGameButton.hpp"
#include "TimeAttackButton.hpp"
#include "AchievementsButton.hpp"
#include "MultiplayerButton.hpp"
#include "LeaderboardsButton.hpp"
#if RETRO_USE_MOD_LOADER
#include "ModsButton.hpp"
#include "ModInfoButton.hpp"
#include "ModsMenu.hpp"
#endif
#include "OptionsButton.hpp"
#include "BackButton.hpp"
#include "SegaIDButton.hpp"
#include "MenuControl.hpp"
#include "SaveSelect.hpp"
#include "PlayerSelectScreen.hpp"
#include "ZoneButton.hpp"
#include "RecordsScreen.hpp"
#include "TimeAttack.hpp"
#if !RETRO_USE_ORIGINAL_CODE
#include "AchievementDisplay.hpp"
#include "AchievementsMenu.hpp"
#endif
#include "InstructionsScreen.hpp"
#include "AboutScreen.hpp"
#include "CreditText.hpp"
#include "StaffCredits.hpp"
#include "OptionsMenu.hpp"
#if RETRO_USE_NETWORKING
#include "MultiplayerHandler.hpp"
#include "MultiplayerScreen.hpp"
#endif

#endif // !NATIVE_OBJECTS_H