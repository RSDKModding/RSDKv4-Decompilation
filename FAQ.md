# General
### Q: How do I set this up?
A: You can find a tutorial for setting up both the game and mods [here](https://gamebanana.com/tuts/14066). Alternatively, opening the decompilation without any game assets present will generate a TXT file containing a brief guide to setting them up.

### Q: Why is the DLC disabled in release builds and autobuilds?
A: Long story short, it's to minimize piracy and ensure an extra layer of legal protection for Sonic Origins Plus. Giving players paid content for free is not the goal of this project.

### Q: The screen is tearing, how do I fix it?
A: Try turning on VSync in settings.ini.

### Q: I'm on Windows and experiencing issues such as mods not appearing in the mod menu, what's wrong?
A: A likely reason for this is that you put the decomp in a directory that's managed by OneDrive (Desktop, Downloads, etc). These directories are known to cause issues, so move your decomp installation elsewhere, such as the root of the C drive or another drive. If it's still not working, try redownloading and reinstalling the decomp.

### Q: I found a bug!
A: Submit an issue in the issues tab and we _might_ fix it in the main branch. Don't expect any major future releases, however.

# Using Mobile RSDK Files
### Q: Why does pressing B pause the game during gameplay?
A: This is a known script issue with most Sega Forever versions of the games. Using the [decompiled scripts](https://github.com/Rubberduckycooly/Sonic-1-Sonic-2-2013-Script-Decompilation) should fix it.

### Q: Why is the default life count 1 instead of 3?
A: This is due to a change made in recent Sega Forever versions of the games. Using the [decompiled scripts](https://github.com/Rubberduckycooly/Sonic-1-Sonic-2-2013-Script-Decompilation) should fix it in most instances.

# Using Origins RSDK Files
### Q: Why doesn't using the datafile work?
A: The RSDK files from Sonic Origins are encrypted in the RSDKv5 datapack format, not the RSDKv4 format. Repacking the files in the correct format or using Data Folder Mode will fix the issue.

### Q: Why is there no audio?
A: Sonic Origins doesn't have any music or sound effects contained in the games' data files, instead storing and handling all in-game audio itself through Hedgehog Engine 2. You can fix this by simply inserting the audio files from the mobile versions of the games. Sound effects added in Origins will have to be inserted manually.

### Q: The game crashes when trying to load the main menu or pause screen, what's wrong?
A: Similar to above, Origins removes some of the assets for the original mobile versions' menus. Also similar to above, the fix is to insert those files from the mobile versions.

### Q: Why is the Drop Dash disabled by default? How do I turn it on?
A: By default, the game mode is set to Classic Mode, which disables the Drop Dash. The only way to change this is through a mod, either by changing the default value of the `game.playMode` global variable in `GameConfig.bin` or by setting the variable to another value via scripts.

### Q: How do I play as Amy?
A: Sonic Team implemented Amy in a way where she isn't playable on the decomp out of the box. This can be fixed via mods. **Do not ask about this in an issue, as we will not be able to help you.**
There are also checks implemented in the engine to prevent playing as Amy on release builds and autobuilds.

### Q: Why doesn't local 2P VS work in Sonic 2?
A: In the version of the Retro Engine that Origins uses (RSDKv5U), certain features that local multiplayer uses, such as splitscreen and multiple controller support, rely on tech found in RSDKv5. This is a decompilation of standalone RSDKv4, and recreating this tech is outside of the scope of the project.

# Miscellaneous
### Q: Will you do a decompilation for Sonic CD (2011)?
A: I already have! You can find it [here](https://github.com/Rubberduckycooly/Sonic-CD-11-Decompilation).

### Q: Will you do a decompilation for Sonic Mania?
A: I already have! You can find the source code for Sonic Mania [here](https://github.com/Rubberduckycooly/Sonic-Mania-Decompilation) and RSDKv5 which is used to run it [here](https://github.com/Rubberduckycooly/RSDKv5-Decompilation).
