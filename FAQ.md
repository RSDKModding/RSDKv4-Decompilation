# General
### Q: How do I set this up?
A: You can find a tutorial for setting up both the game and mods [here](https://gamebanana.com/tuts/14066). Alternatively, opening the decompilation without any game assets present will generate a TXT file containing a brief guide to setting them up.

### Q: The screen is tearing, how do I fix it?
A: Try turning on VSync in settings.ini.

### Q: Why is my game is running faster than it's supposed to?
A: The Retro Engine has all of its engine and game logic tied to the framerate, meaning the faster your refresh rate is, the faster the game runs. You can fix this by turning off VSync in settings.ini or by changing your device's refresh rate.

### Q: I'm on Windows and experiencing issues such as crashes or mods not appearing in the mod menu, what's wrong?
A: A likely reason for this is that you put the decomp in a directory in your user folder (Desktop, Downloads, Documents, etc). These directories are known to cause issues, so move your decomp installation elsewhere, such as the root of the C drive or another drive. If it's still not working, try redownloading and reinstalling the decomp.

### Q: Why does my character softlock or die at the end of some acts in Sonic 1/2?
A: This happens when the screen is too wide. The games weren't fully optimized for ultrawide, so script issues like these will occur. Using the [decompiled scripts](https://github.com/RSDKModding/RSDKv4-Script-Decompilation) should fix this.

### Q: I found a bug!
A: Submit an issue in the Issues tab and we might look into it. Keep in mind that this is a decompilation, so bugs that exist in official releases will most likely not be fixed here.


# Using Mobile RSDK Files
### Q: Why does pressing B pause the game during gameplay?
A: This is a known script issue with most Sega Forever versions of the games. Using the [decompiled scripts](https://github.com/RSDKModding/RSDKv4-Script-Decompilation) should fix it.

### Q: Why is the default life count 1 instead of 3?
A: This is due to a change made in recent Sega Forever versions of the games. Using the [decompiled scripts](https://github.com/RSDKModding/RSDKv4-Script-Decompilation) should fix it in most instances.


# Using Origins RSDK Files
You can find a guide for fixing most of these issues [here](https://gamebanana.com/tuts/16686).

### Q: Why doesn't using the datafile work?
A: The RSDK files from Sonic Origins are encrypted as RSDKv5 datapacks, not RSDKv4 ones. Using Data Folder Mode or repacking the files in the correct format will fix the issue.

### Q: Why is there no audio?
A: Sonic Origins doesn't have any music or sound effects contained in the games' data files, instead storing and handling all in-game audio itself through Hedgehog Engine 2. You can fix this by simply inserting the audio files from the mobile versions of the games. Sound effects new to Origins will have to be inserted manually.

### Q: The game crashes when trying to load the main menu or pause screen, what's wrong?
A: Similar to above, Origins removes some of the assets for the original mobile versions' menus. Also similar to above, the fix is to insert those files from the mobile versions.

### Q: Why is the Drop Dash disabled by default? How do I turn it on?
A: By default, the game mode is set to Classic Mode, which disables the Drop Dash. The only way to change this is through a mod.

### Q: How do I play as Amy?
A: Sonic Team implemented Amy in a way where she isn't playable on the decomp out of the box. This can be fixed via mods. There are also checks implemented in the engine to prevent playing as Amy on release builds and autobuilds.

### Q: Why is the DLC disabled in release builds and autobuilds?
A: Long story short, it's to minimize piracy and ensure an extra layer of legal protection for Sonic Origins Plus. Giving players paid content for free is not the goal of this project.

### Q: Why doesn't local 2P VS work in Sonic 2?
A: In the version of the Retro Engine that Origins uses (RSDKv5U), certain features that local multiplayer uses, such as splitscreen and multiple controller support, rely on tech found in RSDKv5. This is a decompilation of standalone RSDKv4, and recreating this tech is outside of the scope of the project.


# Miscellaneous
### Q: Will you do a decompilation for Sonic CD (2011)?
A: I already have! You can find it [here](https://github.com/RSDKModding/RSDKv3).

### Q: Will you do a decompilation for RSDKv5/Sonic Mania?
A: I already have! You can find Sonic Mania [here](https://github.com/RSDKModding/Sonic-Mania-Decompilation) and RSDKv5 [here](https://github.com/RSDKModding/RSDKv5-Decompilation).
