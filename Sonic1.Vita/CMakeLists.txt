## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)

## This includes the Vita toolchain, must go before project definition
# It is a convenience so you do not have to type
# -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
# highly recommended that you include this block for all projects.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

## Define project parameters here
# Name of the project
project(Sonic1)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "Sonic 1")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "RSDK00002")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.00")

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O3 -fsigned-char -fno-lto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fsigned-char -fno-lto -fno-rtti -fno-exceptions")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

add_definitions(-DBASE_PATH="ux0:data/Sonic1/")

# Add any additional include paths here
include_directories(
  /home/user/vitasdk/arm-vita-eabi/include
)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)

## Build and link
# Add all the files needed to compile here
add_executable(${PROJECT_NAME}
  ../Sonic12Decomp/Animation.cpp
  ../Sonic12Decomp/Audio.cpp
  ../Sonic12Decomp/Collision.cpp
  ../Sonic12Decomp/Debug.cpp
  ../Sonic12Decomp/Drawing.cpp
  ../Sonic12Decomp/Ini.cpp
  ../Sonic12Decomp/Input.cpp
  ../Sonic12Decomp/main.cpp
  ../Sonic12Decomp/Math.cpp
  ../Sonic12Decomp/Network.cpp
  ../Sonic12Decomp/Object.cpp
  ../Sonic12Decomp/Palette.cpp
  ../Sonic12Decomp/PauseMenu.cpp
  ../Sonic12Decomp/Reader.cpp
  ../Sonic12Decomp/RetroEngine.cpp
  ../Sonic12Decomp/RetroGameLoop.cpp
  ../Sonic12Decomp/Scene.cpp
  ../Sonic12Decomp/Scene3D.cpp
  ../Sonic12Decomp/Script.cpp
  ../Sonic12Decomp/Sprite.cpp
  ../Sonic12Decomp/String.cpp
  ../Sonic12Decomp/Text.cpp
  ../Sonic12Decomp/Userdata.cpp
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
target_link_libraries(${PROJECT_NAME}
  SDL2
  vita2d
  vorbisfile
  vorbis
  ogg
  pthread
  SceDisplay_stub
  SceAudio_stub
  SceCtrl_stub
  SceSysmodule_stub
  SceGxm_stub
  SceCommonDialog_stub
  SceAppMgr_stub
  SceTouch_stub
  SceHid_stub
  SceAudio_stub
  SceMotion_stub
  m
)

## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  # FILE sce_sys/icon0.png sce_sys/icon0.png
  # FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  # FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)
