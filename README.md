# Sonic 1/2 2013 Decompilation
A Full Decompilation of Sonic 1 & 2 (2013)

# **SUPPORT THE OFFICIAL RELEASE OF SONIC 1 & SONIC 2**
+ Without assets from the official releases this decompilation will not run.
+ Video tutorial on how to find your legally obtained data.rsdk file: https://www.youtube.com/watch?v=gzIfRW91IxE

+ You can get the official release of sonic 1 & sonic 2 from:
  * [Sonic 1 (iOS, Via the App Store)](https://apps.apple.com/au/app/sonic-the-hedgehog-classic/id316050001)
  * [Sonic 2 (iOS, Via the App Store)](https://apps.apple.com/au/app/sonic-the-hedgehog-2-classic/id347415188)
  * [Sonic 1 (Android, Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.sonic1px&hl=en_AU&gl=US)
  * [Sonic 2 (Android, Via Google Play)](https://play.google.com/store/apps/details?id=com.sega.sonic2.runner&hl=en_AU&gl=US)
  * [Sonic 1 (Android, Via Amazon)](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00D74DVKM)
  * [Sonic 2 (Android, Via Amazon)](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00HAPRVWS)

Even if your platform isn't supported by the official releases, buy it for the assets (you don't need to run the official release, you just need the game assets)

If you want to transfer your save from the **Android pre-forever versions,** you can go to `Android/data/com.sega.sonic1 or 2/SGame.bin` and copy it to the `SData.bin` in the EXE folder.

# Additional Tweaks
* added a built in script compiler, similar to CD, but tweaked up to match the new syntax for the scripts used in RSDKv4
* There is now a settings.ini file that the game uses to load all settings, similar to Sonic Mania
* Dev menu can now be accessed from anywhere by pressing the `ESC` key if enabled in the config
* The `f12` pause, `f11` step over & fast forward debug features from sonic mania have all be ported and are enabled if devMenu is enabled in the config
* If `devMenu` is enabled in the config, pressing `f10` will activate a palette overlay that shows the game's 8 internal palettes in real time

# TODOs:
* the "native object" system has been implimented, but the objects (aside from RetroGameLoop and a temporary pause menu) and the proper HW rendering system have yet to be added
* probably some more bug fixes, because there always are a few stragglers
* create a `cmakelists.txt` file for windows compiling so builds can be added automatically via git actions
* S2 networking code, we attempted to write code to handle the 2PVS mode in S2 but we couldn't finish for many reasons, we did leave our WIP code in the game, so if you think you could do it by all means give it a shot!

# How to build:
## Windows:
* Clone the repo, then follow the instructions in the [depencencies readme for windows](./dependencies/windows/dependencies.txt) to setup dependencies, then build via the visual studio solution
* or grab a prebuilt executable from the releases section

## Windows via MSYS2 (64-bit Only):

* Download the newest version of the MSYS2 installer from [here](https://www.msys2.org/) and install it.
* Run the MINGW64 prompt (from the windows Start Menu/MSYS2 64-bit/MSYS2 MinGW 64-bit), when the program starts enter `pacman -Syuu` in the prompt and hit Enter. Press `Y` when it asks if you want to update packages. If it asks you to close the prompt, do so, then restart it and run the same command again. This updates the packages to their latest versions.
* Now install the dependencies with the following command: `pacman -S make git mingw-w64-i686-gcc mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis`
* Clone the repo with the following command: `git clone https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-CD-11-Decompilation`
* Then run `make CXX=x86_64-w64-mingw32-g++ CXXFLAGS=-static -j4` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)

## Windows UWP (Phone, Xbox, etc.):
* Clone the repo, then follow the instructions in the [depencencies readme for Windows](./dependencies/windows/dependencies.txt) and [depencencies readme for UWP](./dependencies/windows-uwp/dependencies.txt) to setup dependencies, copy your `Data.rsdk` folder into `Sonic1Decomp.UWP` or `Sonic2Decomp.UWP` depending on the game, then build and deploy via `Sonic12Decomp.UWP.sln`
* You may also need to generate visual assets, to do so, open the Package.appxmanifest file in the designer, under the Visual Assets tab, select an image of your choice and click generate.

## Linux:
* To setup your build enviroment and library dependecies run the following commands:
* Ubuntu (Mint, Pop!_OS, etc...): `sudo apt install build-essential git libsdl2-dev libvorbis-dev libogg-dev`
* Arch Linux: `sudo pacman -S base-devel git sdl2 libvorbis libogg`
* Clone the repo with the following command: `git clone https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-1-2-2013-Decompilation`
* Then run `make -j4` (-j switch is optional but will make building faster, it's based on the number of cores you have +1 so 8 cores wold be -j9)

## Mac:
* Clone the repo, then follow the instructions in the [depencencies readme for mac](./dependencies/mac/dependencies.txt) to setup dependencies, then build via the xcode project
* a mac build of v1.0.0 by sappharad can be found [here](https://github.com/Sappharad/Sonic-1-2-2013-Decompilation/releases/tag/1.0.0mac)

## Switch:
* head on over to [heyjoeway's fork](https://github.com/heyjoeway/Sonic-1-2-2013-Decompilation) and follow the installation instructions in the readme

## Other platforms:
Currently the only supported platforms are the ones listed above, however the backend uses libogg, libvorbis & SDL2 to power it, so the codebase is very multiplatform.
if you've cloned this repo and ported it to a platform not on the list or made some changes you'd like to see added to this repo, submit a pull request and it'll most likely be added

# FAQ
### Q: The screen is tearing, how do I fix it?
A: Try turning on vsync, that worked for me (tested on mac)

### Q: I found a bug/I have a feature request!
A: Submit an issue in the issues tab and I'll fix/add (if possible) it as soon as I can

### Q: Will you do a decompilation for Sonic CD (2011)?
A: I already have! you can find it [here](https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation)!

### Q: Will you do a decompilation for Sonic Mania?
A: No. Sonic Mania is a ton bigger and requires that I'd decompile not only how the (far more complex) RSDKv5 works, but also all _600_+ objects work

# Special Thanks
* [RMGRich](https://github.com/MGRich): for helping me fix bugs, tweaking up my sometimes sloppy code and generally being really helpful and fun to work with on this project
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine): for being supportive of me and for giving me a place to show off these things that I've found

# Contact:
you can join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it
