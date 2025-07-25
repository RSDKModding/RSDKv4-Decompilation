add_library(RetroEngine SHARED ${RETRO_FILES})

set(DEP_PATH android)
set(RETRO_SDL_VERSION 2)
set(RETRO_USE_HW_RENDER ON)
set(RETRO_OUTPUT_NAME "RetroEngine")

set(COMPILE_OGG TRUE)
set(COMPILE_VORBIS TRUE)

set(OGG_FLAGS -ffast-math -fsigned-char -O2 -fPIC -DPIC -DBYTE_ORDER=LITTLE_ENDIAN -D_ARM_ASSEM_ -w)
set(VORBIS_FLAGS -ffast-math -fsigned-char -O2 -fPIC -DPIC -DBYTE_ORDER=LITTLE_ENDIAN -D_ARM_ASSEM_ -w)

target_link_libraries(RetroEngine
    android
    EGL
    GLESv1_CM
    log
    z
    jnigraphics
    OpenSLES
    SDL2
)

target_link_options(RetroEngine PRIVATE -u GameActivity_onCreate)