RETRO_FORCE_CASE_INSENSITIVE ?= 1
RETRO_MOD_LOADER             ?= 1
RETRO_NETWORKING             ?= 1
RETRO_USE_HW_RENDER          ?= 1
RETRO_DISABLE_PLUS           ?= 0

CXXFLAGS_ALL = $(shell pkg-config --cflags --static sdl2 vorbisfile vorbis glew) $(CXXFLAGS) \
        -DBASE_PATH='"$(BASE_PATH)"' \
        -IRSDKv4/               \
        -IRSDKv4/NativeObjects/ \
        -Idependencies/all/asio/asio/include/ \
        -Idependencies/all/stb-image/ \
        -Idependencies/all/tinyxml2/

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
        RSDKv4/Networking.cpp	 \
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
        RSDKv4/NativeObjects/AboutScreen.cpp        \
        RSDKv4/NativeObjects/AchievementDisplay.cpp \
        RSDKv4/NativeObjects/AchievementsButton.cpp \
        RSDKv4/NativeObjects/AchievementsMenu.cpp   \
        RSDKv4/NativeObjects/BackButton.cpp         \
        RSDKv4/NativeObjects/CWSplash.cpp           \
        RSDKv4/NativeObjects/CreditText.cpp         \
        RSDKv4/NativeObjects/DialogPanel.cpp        \
        RSDKv4/NativeObjects/FadeScreen.cpp         \
        RSDKv4/NativeObjects/InstructionsScreen.cpp \
        RSDKv4/NativeObjects/LeaderboardsButton.cpp \
        RSDKv4/NativeObjects/MenuBG.cpp             \
        RSDKv4/NativeObjects/MenuControl.cpp        \
        RSDKv4/NativeObjects/ModInfoButton.cpp      \
        RSDKv4/NativeObjects/ModsButton.cpp         \
        RSDKv4/NativeObjects/ModsMenu.cpp           \
        RSDKv4/NativeObjects/MultiplayerButton.cpp  \
        RSDKv4/NativeObjects/MultiplayerHandler.cpp \
        RSDKv4/NativeObjects/MultiplayerScreen.cpp  \
        RSDKv4/NativeObjects/OptionsButton.cpp      \
        RSDKv4/NativeObjects/OptionsMenu.cpp        \
        RSDKv4/NativeObjects/PauseMenu.cpp          \
        RSDKv4/NativeObjects/PlayerSelectScreen.cpp \
        RSDKv4/NativeObjects/PushButton.cpp         \
        RSDKv4/NativeObjects/RecordsScreen.cpp      \
        RSDKv4/NativeObjects/RetroGameLoop.cpp      \
        RSDKv4/NativeObjects/SaveSelect.cpp         \
        RSDKv4/NativeObjects/SegaIDButton.cpp       \
        RSDKv4/NativeObjects/SegaSplash.cpp         \
        RSDKv4/NativeObjects/SettingsScreen.cpp     \
        RSDKv4/NativeObjects/StaffCredits.cpp       \
        RSDKv4/NativeObjects/StartGameButton.cpp    \
        RSDKv4/NativeObjects/SubMenuButton.cpp      \
        RSDKv4/NativeObjects/TextLabel.cpp          \
        RSDKv4/NativeObjects/TimeAttack.cpp         \
        RSDKv4/NativeObjects/TimeAttackButton.cpp   \
        RSDKv4/NativeObjects/TitleScreen.cpp        \
        RSDKv4/NativeObjects/VirtualDPad.cpp        \
        RSDKv4/NativeObjects/VirtualDPadM.cpp       \
        RSDKv4/NativeObjects/ZoneButton.cpp         \
   
ifeq ($(RETRO_FORCE_CASE_INSENSITIVE),1)
    CXXFLAGS_ALL += -DFORCE_CASE_INSENSITIVE
	SOURCES += RSDKv4/fcaseopen.c
endif

ifeq ($(RSDK_MOD_LOADER), 0)
	CXXFLAGS_ALL += -DRETRO_USE_MOD_LOADER=$(RSDK_MOD_LOADER)
endif
ifeq ($(RSDK_USE_NETWORKING), 0)
	CXXFLAGS_ALL += -DRETRO_USE_MOD_LOADER=$(RSDK_USE_NETWORKING)
endif

ifeq ($(RETRO_USE_HW_RENDER), 0)
	CXXFLAGS_ALL += -RETRO_USING_OPENGL=$(RETRO_USE_HW_RENDER)
endif

ifeq ($(RETRO_DISABLE_PLUS), 1)
	CXXFLAGS_ALL += -DRSDK_AUTOBUILD
endif

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) -std=c++17 $^ -o $@ -c

bin/RSDKv4: $(SOURCES:%=objects/%.o)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/RSDKv4
	install -Dp -m755 bin/RSDKv4 $(prefix)/bin/RSDKv4

clean:
	rm -r -f bin && rm -r -f objects
