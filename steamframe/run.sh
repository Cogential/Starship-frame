#!/bin/sh
# Starts Starship (Star Fox 64) on the Steam Frame. This is the program to point Steam or FrameDrop at.
DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$DIR" || exit 1

# FrameDrop's Windows unzip drops executable bits.
chmod +x "$DIR/Starship" 2>/dev/null

# Saves, settings and the sf64.o2r built from the ROM live outside the install folder, so they
# survive reinstalls.
export SHIP_HOME="${SHIP_HOME:-$HOME/.local/share/starship}"
mkdir -p "$SHIP_HOME"

# This launcher only ships in the Steam Frame build, so turn on the Frame behaviour even if
# detection can't see the host OS. STARSHIP_STEAM_FRAME=0 in the launch options turns it off.
export STARSHIP_STEAM_FRAME="${STARSHIP_STEAM_FRAME:-1}"

# Hand any ROM shipped next to the game to the data folder until Starship has built its archive.
if [ ! -f "$SHIP_HOME/sf64.o2r" ]; then
    for rom in "$DIR"/*.z64 "$DIR"/*.n64 "$DIR"/*.v64; do
        [ -f "$rom" ] && [ ! -e "$SHIP_HOME/$(basename "$rom")" ] && cp "$rom" "$SHIP_HOME/"
    done
fi

# Steam describes its virtual Xbox pad as "Steam Frame Controllers" (28de:11e0), which SDL has
# no mapping for, so only D-pad up, A and B would work. Without the description SDL sees an
# ordinary Xbox 360 pad.
unset SteamVirtualGamepadInfo

exec "$DIR/Starship" "$@"
