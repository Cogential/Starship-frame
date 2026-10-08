#pragma once

#include <string>
#include <vector>

// Support for running Starship natively on the Steam Frame (Snapdragon 8 Gen 3, aarch64 SteamOS).
//
// The game is shown as a flat window on a virtual screen inside the headset, with no keyboard or
// mouse and no file dialogs, so the work here is about making it usable from a controller alone:
// controller-driven menus, a single fullscreen surface, readable text, and ROM processing that
// needs no prompts.
namespace SteamFrame {

// True when running on a Steam Frame. Set STARSHIP_STEAM_FRAME=1 (or 0) to force the answer, e.g. to
// test the Frame behaviour on a desktop or to opt out of it on the headset.
bool IsSteamFrame();

// Star Fox 64 ROMs (.z64/.n64/.v64) in the data folder and next to the game, data folder first.
std::vector<std::string> FindRoms();

// Fills in Frame-friendly defaults for any setting the player has not chosen themselves. Must run
// after the configuration and CVars are loaded and before the window is created.
void ApplyDefaults();

// Must run once the window and its menu exist: lets the menu see controllers that connect after
// launch and scales the menu up so it can be read on the virtual screen.
void SetupGui();

} // namespace SteamFrame
