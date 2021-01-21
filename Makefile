CXXFLAGS_ALL = $(shell pkg-config --cflags sdl2 vorbisfile vorbis) $(CXXFLAGS) \
               -DBASE_PATH='"$(BASE_PATH)"'

LDFLAGS_ALL = $(LDFLAGS)
LIBS_ALL = $(shell pkg-config --libs sdl2 vorbisfile vorbis) -pthread $(LIBS)

SOURCES = Sonic12Decomp/Animation.cpp     \
          Sonic12Decomp/Audio.cpp         \
          Sonic12Decomp/Collision.cpp     \
          Sonic12Decomp/Debug.cpp         \
          Sonic12Decomp/Drawing.cpp       \
          Sonic12Decomp/Ini.cpp           \
          Sonic12Decomp/Input.cpp         \
          Sonic12Decomp/main.cpp          \
          Sonic12Decomp/Math.cpp          \
          Sonic12Decomp/Network.cpp       \
          Sonic12Decomp/Object.cpp        \
          Sonic12Decomp/Palette.cpp       \
          Sonic12Decomp/PauseMenu.cpp     \
          Sonic12Decomp/Reader.cpp        \
          Sonic12Decomp/RetroEngine.cpp   \
          Sonic12Decomp/RetroGameLoop.cpp \
          Sonic12Decomp/Scene.cpp         \
          Sonic12Decomp/Scene3D.cpp       \
          Sonic12Decomp/Script.cpp        \
          Sonic12Decomp/Sprite.cpp        \
          Sonic12Decomp/String.cpp        \
          Sonic12Decomp/Text.cpp          \
          Sonic12Decomp/Userdata.cpp      \

objects/%.o: %
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $^ -o $@ -c

bin/sonic2013: $(SOURCES:%=objects/%.o)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS_ALL) $(LDFLAGS_ALL) $^ -o $@ $(LIBS_ALL)

install: bin/sonic2013
	install -Dp -m755 bin/sonic2013 $(prefix)/bin/sonic2013
