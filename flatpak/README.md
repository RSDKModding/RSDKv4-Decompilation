# Flatpak

You will need the FreeDesktop.org 21.08 SDK installed, if you don't have it,
install [Flathub](https://flathub.org/) which provides it.

# Sonic the Hedgehog

To build and install the Sonic the Hedgehog flatpak, you first need the Android
APK for the game. To buy one, visit https://www.sega.com/games/sonic-hedgehog.

Once you have the APK file, rename it to `sonic1.apk` and put it in this
directory.

To build and install the flatpak, run:
**System-wide:**
```
$ sudo flatpak-builder --install --force-clean sonic1 com.sega.Sonic1.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean sonic1 com.sega.Sonic1.json
```

# Sonic the Hedgehog 2

To build and install the Sonic the Hedgehog 2 flatpak, you first need the Android
APK for the game. To buy one, visit https://www.sega.com/games/sonic-hedgehog2.

Once you have the APK file, rename it to `sonic2.apk` and put it in this
directory.

To build and install the flatpak, run:
**System-wide:**
```
$ sudo flatpak-builder --install --force-clean sonic2 com.sega.Sonic2.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean sonic2 com.sega.Sonic2.json
```

# Sonic the Hedgehog (Origins)

**This method will not include music or sound effects, as the RSDK
file from Origins does not contain them.**

First, you need to install Sonic Origins.
To get it, visit https://store.steampowered.com/app/1794960 for the Steam version or https://store.epicgames.com/en-US/p/sonic-origins for the Epic Games version.

Once you have the game, you need to navigate to the game's files.
If you have the Steam version, go to your Steam library, right click on Sonic Origins and click `Manage` > `Browse local files`.

Once you're in the game's files, navigate to `image/x64/raw/retro`. Copy the `Sonic1u.rsdk` file into this directory.

To build and install the flatpak, run:

**System-wide:**
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.Sonic1Origins.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean soniccd com.sega.Sonic1Origins.json
```

# Sonic the Hedgehog 2 (Origins)

**This method will not include music or sound effects, as the RSDK
file from Origins does not contain them.**

First, you need to install Sonic Origins.
To get it, visit https://store.steampowered.com/app/1794960 for the Steam version or https://store.epicgames.com/en-US/p/sonic-origins for the Epic Games version.

Once you have the game, you need to navigate to the game's files.
If you have the Steam version, go to your Steam library, right click on Sonic Origins and click `Manage` > `Browse local files`.

Once you're in the game's files, navigate to `image/x64/raw/retro`. Copy the `Sonic2u.rsdk` file into this directory.

To build and install the flatpak, run:

**System-wide:**
```
$ sudo flatpak-builder --install --force-clean soniccd com.sega.Sonic2Origins.json
```
**User:**
```
$ flatpak-builder --user --install --force-clean soniccd com.sega.Sonic2Origins.json
```