# Retro Engine (RSDK) v4 Decompilation
A Full Decompilation of RSDKv4/Sonic 1 & 2 (2013)

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
* fix the HW Renderer, the game wasn't built around it/ it doesn't properly support palettes so all palette effects are broken, and it doesn't play nice with the SDL2 renderer, everything else works though
* (maybe) S2 networking code, we attempted to write code to handle the 2PVS mode in S2 but we couldn't finish for many reasons, we did leave our WIP code in the game, so if you think you could do it by all means give it a shot!

# How to build:
unless you wanna mod smth thats hardcoded in the game, just use the release

# Special Thanks
* [RubberDuckyCooly](https://github.com/Rubberduckycooly): For making the original decompilations of Sonic 1 & 2
* [RMGRich](https://github.com/MGRich): for helping RDC fix bugs, tweaking up their sometimes sloppy code and for working on the original decomps
* Everyone in the [Retro Engine Modding Server](https://dc.railgun.works/retroengine): for being supportive of RDC and for giving them a place to show off these things that RDC found

# Contact:
you can join the [Retro Engine Modding Discord Server](https://dc.railgun.works/retroengine) for any extra questions you may need to know about the decompilation or modding it
