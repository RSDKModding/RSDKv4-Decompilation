.DEFAULT_GOAL := all

NAME		=  s122013
SUFFIX		= 
PKGCONFIG	=  pkg-config
DEBUG		?= 0
STATIC		?= 1
VERBOSE		?= 0
PROFILE		?= 0
STRIP		?= strip

# =============================================================================
# Detect default platform if not explicitly specified
# =============================================================================

ifeq ($(OS),Windows_NT)
	PLATFORM ?= Windows
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		PLATFORM ?= Linux
	endif

	ifeq ($(UNAME_S),Darwin)
		PLATFORM ?= macOS
	endif

endif

ifdef EMSCRIPTEN
	PLATFORM = Emscripten
endif

PLATFORM ?= Unknown

# =============================================================================

OUTDIR = bin/$(PLATFORM)
OBJDIR = obj/$(PLATFORM)

include Makefile_cfgs/Platforms/$(PLATFORM).cfg

# =============================================================================

ifeq ($(STATIC),1)
	PKGCONFIG +=  --static
endif

ifeq ($(DEBUG),1)
	CXXFLAGS += -g
	STRIP = :
else
	CXXFLAGS += -O3
endif


ifeq ($(STATIC),1)
	CXXFLAGS += -static
endif

CXXFLAGS_ALL = $(shell pkg-config --cflags --static sdl2 vorbisfile vorbis glew) $(CXXFLAGS) \
               -DBASE_PATH='"$(BASE_PATH)"' \
               --std=c++17 \
               -fsigned-char

LDFLAGS_ALL = $(LDFLAGS)
LIBS_ALL = $(shell pkg-config --libs --static sdl2 vorbisfile vorbis glew) -pthread $(LIBS)

SOURCES = dependencies/all/tinyxml2/tinyxml2.cpp \
          RSDKv4/Animation.cpp     \
          RSDKv4/Audio.cpp         \
          RSDKv4/Collision.cpp     \
          RSDKv4/Debug.cpp         \
          RSDKv4/Drawing.cpp       \
          RSDKv4/Ini.cpp           \
          RSDKv4/Input.cpp         \
          RSDKv4/main.cpp          \
          RSDKv4/Math.cpp          \
          RSDKv4/ModAPI.cpp        \
          RSDKv4/Networking.cpp	   \
          RSDKv4/Object.cpp        \
          RSDKv4/Palette.cpp       \
          RSDKv4/Reader.cpp        \
          RSDKv4/Renderer.cpp      \
          RSDKv4/RetroEngine.cpp   \
          RSDKv4/Scene.cpp         \
          RSDKv4/Scene3D.cpp       \
          RSDKv4/Script.cpp        \
          RSDKv4/Sprite.cpp        \
          RSDKv4/String.cpp        \
          RSDKv4/Text.cpp          \
          RSDKv4/Userdata.cpp      \
          RSDKv4/NativeObjects/AboutScreen.cpp \
          RSDKv4/NativeObjects/AchievementDisplay.cpp \
          RSDKv4/NativeObjects/AchievementsButton.cpp \
          RSDKv4/NativeObjects/AchievementsMenu.cpp \
          RSDKv4/NativeObjects/BackButton.cpp \
          RSDKv4/NativeObjects/CWSplash.cpp \
          RSDKv4/NativeObjects/CreditText.cpp \
          RSDKv4/NativeObjects/DialogPanel.cpp \
          RSDKv4/NativeObjects/FadeScreen.cpp \
          RSDKv4/NativeObjects/InstructionsScreen.cpp \
          RSDKv4/NativeObjects/LeaderboardsButton.cpp \
          RSDKv4/NativeObjects/MenuBG.cpp \
          RSDKv4/NativeObjects/MenuControl.cpp \
          RSDKv4/NativeObjects/ModInfoButton.cpp \
          RSDKv4/NativeObjects/ModsButton.cpp \
          RSDKv4/NativeObjects/ModsMenu.cpp \
          RSDKv4/NativeObjects/MultiplayerButton.cpp \
          RSDKv4/NativeObjects/MultiplayerHandler.cpp \
          RSDKv4/NativeObjects/MultiplayerScreen.cpp \
          RSDKv4/NativeObjects/OptionsButton.cpp \
          RSDKv4/NativeObjects/OptionsMenu.cpp \
          RSDKv4/NativeObjects/PauseMenu.cpp \
          RSDKv4/NativeObjects/PlayerSelectScreen.cpp \
          RSDKv4/NativeObjects/PushButton.cpp \
          RSDKv4/NativeObjects/RecordsScreen.cpp \
          RSDKv4/NativeObjects/RetroGameLoop.cpp \
          RSDKv4/NativeObjects/SaveSelect.cpp \
          RSDKv4/NativeObjects/SegaIDButton.cpp \
          RSDKv4/NativeObjects/SegaSplash.cpp \
          RSDKv4/NativeObjects/SettingsScreen.cpp \
          RSDKv4/NativeObjects/StaffCredits.cpp \
          RSDKv4/NativeObjects/StartGameButton.cpp \
          RSDKv4/NativeObjects/SubMenuButton.cpp \
          RSDKv4/NativeObjects/TextLabel.cpp \
          RSDKv4/NativeObjects/TimeAttack.cpp \
          RSDKv4/NativeObjects/TimeAttackButton.cpp \
          RSDKv4/NativeObjects/TitleScreen.cpp \
          RSDKv4/NativeObjects/VirtualDPad.cpp \
          RSDKv4/NativeObjects/VirtualDPadM.cpp \
          RSDKv4/NativeObjects/ZoneButton.cpp \
   
ifneq ($(FORCE_CASE_INSENSITIVE),)
	CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
	SOURCES += RSDKv4/fcaseopen.c
endif

ifeq ($(PROFILE),1)
	CXXFLAGS_ALL += -pg -g -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -fno-default-inline
endif

ifeq ($(VERBOSE),0)
	CC := @$(CC)
	CXX := @$(CXX)
endif


INCLUDES  += \
    -I./RSDKv4/ \
    -I./RSDKv4/NativeObjects/ \
    -I./dependencies/all/asio/asio/include/ \
    -I./dependencies/all/stb-image/ \
    -I./dependencies/all/tinyxml2/


INCLUDES += $(LIBS)

# Main Sources
SOURCES = \
    RSDKv4/Animation    \
    RSDKv4/Audio        \
    RSDKv4/Collision    \
    RSDKv4/Debug        \
    RSDKv4/Drawing      \
    RSDKv4/Ini          \
    RSDKv4/Input        \
    RSDKv4/Math         \
    RSDKv4/ModAPI       \
    RSDKv4/Networking   \
    RSDKv4/Object       \
    RSDKv4/Palette      \
    RSDKv4/Reader       \
    RSDKv4/Renderer     \
    RSDKv4/RetroEngine  \
    RSDKv4/Scene        \
    RSDKv4/Scene3D      \
    RSDKv4/Script       \
    RSDKv4/Sprite       \
    RSDKv4/String       \
    RSDKv4/Text         \
    RSDKv4/Userdata     \
    RSDKv4/main         \
    RSDKv4/NativeObjects/AboutScreen        \
    RSDKv4/NativeObjects/AchievementDisplay \
    RSDKv4/NativeObjects/AchievementsButton \
    RSDKv4/NativeObjects/AchievementsMenu   \
    RSDKv4/NativeObjects/All                \
    RSDKv4/NativeObjects/BackButton         \
    RSDKv4/NativeObjects/CWSplash           \
    RSDKv4/NativeObjects/CreditText         \
    RSDKv4/NativeObjects/DialogPanel        \
    RSDKv4/NativeObjects/FadeScreen         \
    RSDKv4/NativeObjects/InstructionsScreen \
    RSDKv4/NativeObjects/LeaderboardsButton \
    RSDKv4/NativeObjects/MenuBG             \
    RSDKv4/NativeObjects/MenuControl        \
    RSDKv4/NativeObjects/ModInfoButton      \
    RSDKv4/NativeObjects/ModsButton         \
    RSDKv4/NativeObjects/ModsMenu           \
    RSDKv4/NativeObjects/MultiplayerButton  \
    RSDKv4/NativeObjects/OptionsButton      \
    RSDKv4/NativeObjects/OptionsMenu        \
    RSDKv4/NativeObjects/PauseMenu          \
    RSDKv4/NativeObjects/PlayerSelectScreen \
    RSDKv4/NativeObjects/PushButton         \
    RSDKv4/NativeObjects/RecordsScreen      \
    RSDKv4/NativeObjects/RetroGameLoop      \
    RSDKv4/NativeObjects/SaveSelect         \
    RSDKv4/NativeObjects/SegaIDButton       \
    RSDKv4/NativeObjects/SegaSplash         \
    RSDKv4/NativeObjects/SettingsScreen     \
    RSDKv4/NativeObjects/StaffCredits       \
    RSDKv4/NativeObjects/StartGameButton    \
    RSDKv4/NativeObjects/SubMenuButton      \
    RSDKv4/NativeObjects/TextLabel          \
    RSDKv4/NativeObjects/TimeAttack         \
    RSDKv4/NativeObjects/TimeAttackButton   \
    RSDKv4/NativeObjects/TitleScreen        \
    RSDKv4/NativeObjects/VirtualDPad        \
    RSDKv4/NativeObjects/VirtualDPadM       \
    RSDKv4/NativeObjects/ZoneButton         \
    dependencies/all/tinyxml2/tinyxml2

PKGSUFFIX ?= $(SUFFIX)

BINPATH = $(OUTDIR)/$(NAME)$(SUFFIX)
PKGPATH = $(OUTDIR)/$(NAME)$(PKGSUFFIX)

OBJECTS += $(addprefix $(OBJDIR)/, $(addsuffix .o, $(SOURCES)))

$(shell mkdir -p $(OUTDIR))
$(shell mkdir -p $(OBJDIR))

$(OBJDIR)/%.o: %.c
	@mkdir -p $(@D)
	@echo -n Compiling $<...
	$(CXX) -c $(CXXFLAGS_ALL) $(INCLUDES) $(DEFINES) $< -o $@
	@echo " Done!"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo -n Compiling $<...
	$(CXX) -c $(CXXFLAGS_ALL) $(INCLUDES) $(DEFINES) $< -o $@
	@echo " Done!"

$(BINPATH): $(OBJDIR) $(OBJECTS)
	@echo -n Linking...
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $(OBJECTS) -o $@ $(LIBS_ALL)
	@echo " Done!"
	$(STRIP) $@

ifeq ($(BINPATH),$(PKGPATH))
all: $(BINPATH)
else
all: $(PKGPATH)
endif

clean:
	rm -rf $(OBJDIR) && rm -rf $(BINPATH)