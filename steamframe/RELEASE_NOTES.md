Native ARM64 Linux build of [Starship](https://github.com/HarbourMasters/Starship) (Star Fox 64 PC port) for the Steam Frame, built from upstream `main`.

**No ROM included.** Put your own Star Fox 64 ROM (US 1.0 or 1.1, `.z64`) in `~/.local/share/starship/` or next to `run.sh`; the first launch turns it into `sf64.o2r` with no prompts.

Download `starship-steam-frame-arm64.zip`, unzip it into `~/devkit-game/<name>/` and point Steam at `run.sh` (no compatibility tool, or Steam Linux Runtime 4.0 arm64).

On the Frame (detected at launch, `STARSHIP_STEAM_FRAME=0/1` overrides):
- View opens the menu with the controller, including controllers that connect after launch
- fullscreen 1920x1080, VSync on, single viewport, menu drawn at 150%
- ROMs in the data and install folders are processed automatically; with none, a message says where to put it

Built natively on GitHub's `ubuntu-22.04-arm` runner for ARMv8.2 (Snapdragon 8 Gen 3 tuned).
