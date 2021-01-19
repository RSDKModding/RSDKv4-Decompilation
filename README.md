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

## Mac:
* Clone the repo, then follow the instructions in the [depencencies readme for mac](./dependencies/mac/dependencies.txt) to setup dependencies, then build via the xcode project
* note: a proper mac release is being worked on, and hopefully will be released soon

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

# Background:
in 2018 I started researching Christan Whitehead's 'Retro Engine' as a side project since I was bored, I started with Sonic CD (RSDKv3) since it was the most well known version that hadn't had much support, since at that time Sonic Mania's (RSDKv5) modding scene was already thriving, and eventually I expanded my range to Retro-Sonic (Retro-Sonic Engine), Sonic Nexus (RSDKv1) & Sonic 1/2 (RSDKv4), since then I have worked during spare moments to document and reverse all that I can of all versions of RSDK as it was just interesting to see how things worked under the hood or how features evolved and changed over time. Fast forward to 2020 and [Sappharad](https://github.com/Sappharad) shows me his decompilation of Sonic CD based on the windows phone 7 port since they'd seen my other github repositories relating to RSDK reversing. After seeing their decompilation I had the idea to start my own Sonic CD decompilation based on the PC port, with improvements and tweaks android port, though I didn't have much time to get around to it, so the project was shelved until I had more time to work on it. in mid-december 2020, I remembered the sonic CD decompilation that I started and finally had the time to work on it more, so after around 2 weeks of on/off working the decompilation was finally in a solid working state, though I continued tweaking it for another few weeks just to iron out all the glitches and bugs that I found. in early january 2021 it was released to almost universal praise, and since I enjoyed working on that I decided I would try to make a decompilation of RSDKv4 (Sonic 1/Sonic 2), since the engine was similar enough to v3's that I could translate much of the core over from the v3 decompilation, which I was sucessfully able to do. a few days into working on the v4 decompilation I asked [RMGRich](https://github.com/MGRich) if they wanted to help work on this with me since there's a lot about v4 that hasn't been very documented that I'd have to figure out, they accepted and over the first half of january 2021 we got to work in finishing the decompilation

# Contact:
you can join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it
