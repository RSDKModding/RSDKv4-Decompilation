![](header.png?raw=true)

A complete decompilation of Retro Engine v4 and the menus from Sonic 1 and 2 (2013).

# **SUPPORT THE OFFICIAL RELEASE OF SONIC 1 & 2**
+ Without assets from the official releases, this decompilation will not run.

+ You can get official releases of Sonic 1 & Sonic 2 from:
  * Windows
    * Via Steam, from [Sonic Origins](https://store.steampowered.com/app/1794960)
    * Via the Epic Games Store, from [Sonic Origins](https://store.epicgames.com/en-US/p/sonic-origins)
  * iOS
    * [Sonic 1, Via the App Store](https://apps.apple.com/au/app/sonic-the-hedgehog-classic/id316050001)
    * [Sonic 2, Via the App Store](https://apps.apple.com/au/app/sonic-the-hedgehog-2-classic/id347415188)
    * A tutorial for finding the game assets from the iOS versions can be found [here](https://gamebanana.com/tuts/14491).
  * Android
    * [Sonic 1, Via Google Play](https://play.google.com/store/apps/details?id=com.sega.sonic1px)
    * [Sonic 2, Via Google Play](https://play.google.com/store/apps/details?id=com.sega.sonic2.runner)
    * [Sonic 1, Via Amazon](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00D74DVKM)
    * [Sonic 2, Via Amazon](https://www.amazon.com.au/Sega-of-America-Sonic-Hedgehog/dp/B00HAPRVWS)
    * A tutorial for finding the game assets from the Android versions can be found [here](https://gamebanana.com/tuts/14492).

Even if your platform isn't supported by the official releases, you **must** buy or officially download it for the assets (you don't need to run the official release, you just need the game assets).

If you want to transfer your save(s) from the official mobile version(s), the **Android pre-forever** file path is `Android/data/com.sega.sonic1 or 2/SGame.bin` (other versions may have different file paths). Copy that file into the decompilation's folder with the name `SData.bin`.

# Additional Tweaks
* Added the built in script compiler from RSDKv5U.
* Added a built in mod loader and API, allowing to easily create and play mods with features such as save file redirection, custom achievements and XML GameConfig data.
* Custom menu and networking system for Sonic 2 multiplayer, allowing anyone to host and join servers and play 2P VS.
  * Servers may be unreliable; this feature is more or less a proof of concept.
* Egg Gauntlet Zone is playable in the Time Attack menu in Sonic 2, if you're using a version of the game that includes it.
* There is now a `settings.ini` file that the game uses to load all settings, similar to Sonic Mania.
* The dev menu can now be accessed from anywhere by pressing the `ESC` key if enabled in the config.
* The `F12` pause, `F11` step over & fast forward debug features from Sonic Mania have all been ported and are enabled if `devMenu` is enabled in the config.
* A number of additional dev menu debug features have been added:
  * `F1` will load the first scene in the Presentation stage list (usually the title screen).
  * `F2` and `F3` will load the previous and next scene in the current stage list.
  * `F5` will reload the current scene, as well as all assets and scripts.
  * `F8` and `F9` will visualize touch screen and object hitboxes.
  * `F10` will activate a palette overlay that shows the game's 8 internal palettes in real time.
* Added the idle screen dimming feature from Sonic Mania Plus, as well as allowing the user to disable it or set how long it takes for the screen to dim.

# How to build
## Windows
* Clone the repo, then follow the instructions in the [dependencies readme for Windows](./dependencies/windows/dependencies.txt) to setup dependencies, then build via the visual studio solution.
* Alternatively, you can grab a prebuilt executable from the releases section.

## Windows via MSYS2 (64-bit Only)
### Decompilation
* Download the newest version of the MSYS2 installer from [here](https://www.msys2.org/) and install it.
* Run the MINGW64 prompt (from the windows Start Menu/MSYS2 64-bit/MSYS2 MinGW 64-bit), when the program starts enter `pacman -Syuu` in the prompt and hit Enter.
* Press `Y` when it asks if you want to update packages. If it asks you to close the prompt, do so, then restart it and run the same command again. This updates the packages to their latest versions.
* Install the dependencies with the following command: `pacman -S pkg-config make git mingw-w64-i686-gcc mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-glew`
* Clone the repo with the following command: `git clone --recursive https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-1-2-2013-Decompilation`.
* Run `make -f Makefile.msys2 CXX=x86_64-w64-mingw32-g++ CXXFLAGS=-static -j4`.
  * -j switch is optional, but will make building faster by running it parallel on multiple cores (8 cores would be -j9).

## Windows UWP (Phone, Xbox, etc.)
* Clone the repo, then follow the instructions in the [dependencies readme for Windows](./dependencies/windows/dependencies.txt) and [dependencies readme for UWP](./dependencies/windows-uwp/dependencies.txt) to setup dependencies.
* Copy your `Data.rsdk` file into `Sonic1Decomp.UWP` or `Sonic2Decomp.UWP` depending on the game, then build and deploy via `RSDKv4.UWP.sln`.
* You may also need to generate visual assets. To do so, open the Package.appxmanifest file in the designer. Under the Visual Assets tab, select an image of your choice, and click generate.

## Linux
### Decompilation
* To setup your build enviroment and library dependecies, run the following commands:
  * Ubuntu (Mint, Pop!_OS, etc...): `sudo apt install build-essential git libsdl2-dev libvorbis-dev libogg-dev libglew-dev libdecor-0-dev`
    * If you're using Debian, add `libgbm-dev` and `libdrm-dev`.
  * Fedora Linux: `sudo dnf install g++ SDL2-devel libvorbis-devel libogg-devel glew-devel`
  * Arch Linux: `sudo pacman -S base-devel git sdl2 libvorbis libogg glew`
* Clone the repo and its other dependencies with the following command: `git clone --recursive https://github.com/Rubberduckycooly/Sonic-1-2-2013-Decompilation.git`
* Go into the repo you just cloned with `cd Sonic-1-2-2013-Decompilation`.
* Run `make -j5`.
    * If your distro is using gcc 8.x.x, then add the argument `LIBS=-lstdc++fs`.
    * -j switch is optional, but will make building faster by running it parallel on multiple cores (8 cores would be -j9).

## Mac
* Clone the repo, follow the instructions in the [dependencies readme for Mac](./dependencies/mac/dependencies.txt) to setup dependencies, then build via the Xcode project.
* Alternatively, a Mac build of v1.3.1 by [Sappharad](https://github.com/Sappharad) can be found [here](https://github.com/Sappharad/Sonic-1-2-2013-Decompilation/releases/tag/1.3.1-mac).

## Android
* Clone the repo, then follow the instructions in the [dependencies readme for Android](./dependencies/android/dependencies.txt).
* Ensure the symbolic links in `android/app/jni` are correct. If not, fix them with the following on Windows:
  * `mklink /D src ..\..\..`
  * `mklink /D SDL ..\..\..\dependencies\android\SDL`
* Open `android/` in Android Studio, install the NDK and everything else that it asks for, and build.

## Unofficial Branches
Follow the installation instructions in the readme of each branch.
* For the **PlayStation Vita**, go to [Xeeynamo's fork](https://github.com/xeeynamo/Sonic-1-2-2013-Decompilation).
* For the **Nintendo Switch**, go to [heyjoeway's fork](https://github.com/heyjoeway/Sonic-1-2-2013-Decompilation).
* For the **Nintendo 3DS**, go to [JeffRuLz's fork](https://github.com/JeffRuLz/Sonic-1-2-2013-Decompilation).
  * A New Nintendo 3DS is required for the games to run smoothly.
* To play it on the web using **Wasm**, go to [mattConn's fork](https://github.com/mattConn/Sonic-Decompilation-WASM).

Because these branches are unofficial, we can't provide support for them and they may not be up-to-date.

## Other Platforms
Currently the only supported platforms are the ones listed above, however the backend uses libogg, libvorbis & SDL2 to power it (as well as tinyxml2 for the mod API and asio for networking), so the codebase is very multiplatform.
If you're able to, you can clone this repo and port it to a platform not on the list.

# Server
The multiplayer server requires Python 3.8 or later. You can download Python [here](https://www.python.org/downloads/).
To use the server, open Command Prompt in the folder [Server.py](./Server/Server.py) is located in, then run the command `py -3 Server.py [local IPv4 address] [port] debug`. You can find your local IPv4 address using the command `ipconfig`.
Note that the C++ server found in the `Server` folder has been deprecated and no longer works. It has been kept in the repo for reference purposes.

# FAQ
You can find the FAQ [here](./FAQ.md).

# Special Thanks
* [st×tic](https://github.com/stxticOVFL) for helping me fix bugs, tweaking up my sometimes sloppy code and generally being really helpful and fun to work with on this project.
* [The Weigman](https://github.com/TheWeigman) for creating the header you see up here along with similar assets.
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine) for being supportive of me and for giving me a place to show off these things that I've found.

# Contact:
Join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it.
