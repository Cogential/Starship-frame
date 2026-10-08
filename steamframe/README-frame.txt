Starship (Star Fox 64 PC port) for the Steam Frame
==================================================

This is a native ARM64 build of Starship (https://github.com/HarbourMasters/Starship) for
Valve's Steam Frame. It does not include the game: you need your own Star Fox 64 ROM.

Install
-------
1. Copy this folder to the Frame, e.g. ~/devkit-game/Star_Fox_64_Starship/.
2. Add run.sh as a non-Steam game. Use no compatibility tool, or Steam Linux Runtime 4.0 (arm64).
3. Put your ROM (US 1.0 or 1.1, .z64) in ~/.local/share/starship/ or next to run.sh.

The first launch turns the ROM into sf64.o2r in ~/.local/share/starship/. This takes a minute or
two, with a black screen while it runs. Later launches start straight away. JP and EU ROMs put in
the same folder are processed too, for the voice-replacement options.

Controls
--------
- View (Back) opens the Starship menu; move with the D-pad or stick, A selects, B goes back.
- Saves, settings and mods live in ~/.local/share/starship/ and survive reinstalls.
- STARSHIP_STEAM_FRAME=0 in the launch options turns off the Frame defaults.
