# RSDKv4 WINDOWS CMAKE
# TODO: maybe have this do linux too? who knows

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(RSDKv4)

#proj config
set(CXX_STANDARD 12)
set(CXX_STANDARD_REQUIRED on)

find_package(SDL2 CONFIG REQUIRED)
find_package(Ogg CONFIG REQUIRED)
find_package(Vorbis CONFIG REQUIRED)

file(GLOB SOURCE_CODE CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/Sonic12Decomp/*.cpp"
    "${CMAKE_SOURCE_DIR}/Sonic12Decomp/*.hpp"
)	

add_executable(RSDKv4 ${SOURCE_CODE})
target_compile_features(RSDKv4 PRIVATE)

target_include_directories(Solstice PUBLIC 
    ${CMAKE_SOURCE_DIR}/Sonic12Decomp
    ${CMAKE_SOURCE_DIR}/dependencies/windows/SDL2/include
    ${CMAKE_SOURCE_DIR}/dependencies/windows/libogg/include
    ${CMAKE_SOURCE_DIR}/dependencies/windows/livorbis/include
)

target_link_libraries(RSDKv4 
    SDL2::SDL2
    SDL2::SDL2main
    Ogg::ogg
    Vorbis::vorbis
    Vorbis::vorbisfile
    shell
)