# Android

## Install dependencies
* SDL2: [Download the source code](https://github.com/libsdl-org/SDL/tree/SDL2) and go to the `Tags` section and find `2.28.2`, download the `Source Code (zip)` and unzip it in `dependencies/android/SDL`.
![image](https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation/assets/66157074/4aec8fe4-cd69-48da-a2d7-b00e7649e06a)
![image](https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation/assets/66157074/2881ff87-8af7-494a-9ed4-0af69c4ebb54)
![image](https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation/assets/66157074/906d27e4-aa52-4fc4-962e-70fca20abef1)

* GLEW: [Download the binaries](http://glew.sourceforge.net/) and unzip it in `dependencies/android/glew`.

* libogg: [Download](https://xiph.org/downloads/) and unzip it in `dependencies/android/libogg`. Then, copy "config_types.h" from here and paste it in "./libogg/include/ogg/".

* libvorbis: [Download](https://xiph.org/downloads/) and unzip it in `dependencies/android/libvorbis`.

* Ensure the symbolic links in `[root]/android/app/jni` are correct:
  * `SDL` -> Root of the Android SDL dependency
  * `src` -> Root of the RSDKv4 repository
  
    To add symbolic links, do the following:
      * Windows: `mklink /d "[name-of-symlink]" "[path]"`
      * Linux: `ln -s "[path]" "[name-of-symlink]"`
* Open `[root]/android/` in Android Studio, install the NDK and everything else that it asks for, and build.

## Common build issues (Windows)
### `make: *** INTERNAL: readdir: No such file or directory`
Delete `android/app/build` and try again. This occurs when the path is short enough to build, but too long for a rebuild. 
### `make: Interrupt/exception caught (code = 0xc0000005)`
Your paths are too long. Try renaming the symbolic links to something shorter, or make a symbolic link to RSDKv4 closer to the root of the drive and open the project through there (e.x. C:/RSDKv4/android). *I haven't had either issue since I did this.*
