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
CFLAGS		?= -fsigned-char

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

INCLUDES  += 	-I./Sonic12Decomp

INCLUDES += $(LIBS)

# Main Sources
SOURCES	+=	Sonic12Decomp/Animation \
			Sonic12Decomp/Audio \
			Sonic12Decomp/Collision \
			Sonic12Decomp/Debug \
			Sonic12Decomp/Drawing \
			Sonic12Decomp/Ini \
			Sonic12Decomp/Input \
			Sonic12Decomp/main \
			Sonic12Decomp/Math \
			Sonic12Decomp/Network \
			Sonic12Decomp/Object \
			Sonic12Decomp/Palette \
			Sonic12Decomp/PauseMenu \
			Sonic12Decomp/Reader \
			Sonic12Decomp/RetroEngine \
			Sonic12Decomp/RetroGameLoop \
			Sonic12Decomp/Scene \
			Sonic12Decomp/Scene3D \
			Sonic12Decomp/Script \
			Sonic12Decomp/Sprite \
			Sonic12Decomp/String \
			Sonic12Decomp/Text \
			Sonic12Decomp/Userdata

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
