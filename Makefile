.DEFAULT_GOAL := all

NAME		=  s122013
SUFFIX		= 
PKGCONFIG	=  pkg-config
DEBUG		?= 0
STATIC		?= 1
VERBOSE		?= 0
PROFILE		?= 0
STRIP		?= strip
DEFINES     =

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

ifeq ($(DEBUG),1)
	CXXFLAGS += -g
	STRIP = :
else
	CXXFLAGS += -O3
endif


ifeq ($(STATIC),1)
	CXXFLAGS += -static
endif

CXXFLAGS_ALL = `$(PKGCONFIG) --cflags --static sdl2 vorbisfile vorbis`
LIBS_ALL = `$(PKGCONFIG) --libs --static sdl2 vorbisfile vorbis`

CXXFLAGS_ALL += $(CXXFLAGS) \
               -DBASE_PATH='"$(BASE_PATH)"' \
               --std=c++17 \
               -fsigned-char

LDFLAGS_ALL = $(LDFLAGS)
LIBS_ALL += -pthread $(LIBS)

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
    RSDKv4/NativeObjects/All                \
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