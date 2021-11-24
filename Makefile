.DEFAULT_GOAL := all

NAME		=  s122013
SUFFIX		= 
PKGCONFIG	=  pkg-config
DEBUG		?= 0
STATIC		?= 1
VERBOSE		?= 0
PROFILE		?= 0
STRIP		?= strip

# -fsigned-char required to prevent hang in LoadStageCollisions
CFLAGS		?= -fsigned-char -std=c++20

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
	CFLAGS += -g
	STRIP = :
else
	CFLAGS += -O3
endif

ifeq ($(PROFILE),1)
	CFLAGS += -pg -g -fno-inline-functions -fno-inline-functions-called-once -fno-optimize-sibling-calls -fno-default-inline
endif

ifeq ($(VERBOSE),0)
	CC := @$(CC)
	CXX := @$(CXX)
endif

# =============================================================================

CFLAGS += `$(PKGCONFIG) --cflags sdl2 ogg vorbis theora vorbisfile theoradec`
LIBS   += `$(PKGCONFIG) --libs-only-l --libs-only-L sdl2 ogg vorbis theora vorbisfile theoradec`

#CFLAGS += -Wno-strict-aliasing -Wno-narrowing -Wno-write-strings

ifeq ($(STATIC),1)
	CFLAGS += -static
endif

INCLUDES  += \
    -I./dependencies/all/stb-image  \
    -I./dependencies/all/tinyxml2  \
    -I./RSDKv4  \
    -I./RSDKv4/NativeObjects  \

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
	$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@
	@echo " Done!"

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	@echo -n Compiling $<...
	$(CXX) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@
	@echo " Done!"

$(BINPATH): $(OBJDIR) $(OBJECTS)
	@echo -n Linking...
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@ $(LIBS)
	@echo " Done!"
	$(STRIP) $@

ifeq ($(BINPATH),$(PKGPATH))
all: $(BINPATH)
else
all: $(PKGPATH)
endif

clean:
	rm -rf $(OBJDIR)