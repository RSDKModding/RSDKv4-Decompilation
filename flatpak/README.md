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