#include "SteamFrame.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

#include <libultraship/libultraship.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <SDL2/SDL.h>

namespace fs = std::filesystem;

namespace SteamFrame {

namespace {

#if defined(__linux__) && defined(__aarch64__)
std::string OsReleaseValue(const std::string& key) {
    // Inside Steam Linux Runtime (pressure-vessel) /etc/os-release describes the runtime; the host's
    // copy is mounted at /run/host/os-release.
    std::ifstream osRelease("/run/host/os-release");
    if (!osRelease.is_open()) {
        osRelease.open("/etc/os-release");
    }
    std::string line;
    while (std::getline(osRelease, line)) {
        if (line.rfind(key + "=", 0) != 0) {
            continue;
        }
        std::string value = line.substr(key.size() + 1);
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        return value;
    }
    return "";
}
#endif

bool Detect() {
    if (const char* forced = std::getenv("STARSHIP_STEAM_FRAME")) {
        return std::strcmp(forced, "0") != 0;
    }
#if defined(__linux__) && defined(__aarch64__)
    // The Frame reports ID=steamos, VARIANT_ID=vr. It is the only aarch64 device SteamOS ships on;
    // its Arch Linux ARM base (Holo) reports ID=holo.
    const std::string id = OsReleaseValue("ID");
    return id == "steamos" || id == "holo" || OsReleaseValue("VARIANT_ID") == "vr";
#else
    return false;
#endif
}

// SDL event watch: hands controller add/remove events to ImGui, which only uses them to rescan its
// controller list, before libultraship takes them off the queue.
int ForwardControllerDeviceEventsToImGui(void* userdata, SDL_Event* event) {
    const bool imguiBackendUp =
        ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().BackendPlatformUserData != nullptr;
    if (imguiBackendUp && (event->type == SDL_CONTROLLERDEVICEADDED || event->type == SDL_CONTROLLERDEVICEREMOVED)) {
        ImGui_ImplSDL2_ProcessEvent(event);
    }
    return 0;
}

} // namespace

bool IsSteamFrame() {
    static const bool sIsSteamFrame = Detect();
    return sIsSteamFrame;
}

std::vector<std::string> FindRoms() {
    std::vector<std::string> roms;
    for (const std::string& dir : { Ship::Context::GetAppDirectoryPath(), Ship::Context::GetAppBundlePath() }) {
        std::error_code ec;
        std::vector<std::string> found;
        for (const auto& entry : fs::directory_iterator(dir, ec)) {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (entry.is_regular_file(ec) && (ext == ".z64" || ext == ".n64" || ext == ".v64")) {
                found.push_back(fs::weakly_canonical(entry.path(), ec).string());
            }
        }
        std::sort(found.begin(), found.end());
        for (const auto& rom : found) {
            // The install and data folders are the same in a portable install.
            if (std::find(roms.begin(), roms.end(), rom) == roms.end()) {
                roms.push_back(rom);
            }
        }
    }
    return roms;
}

void ApplyDefaults() {
    if (!IsSteamFrame()) {
        return;
    }

    SPDLOG_INFO("Steam Frame detected, applying Steam Frame defaults");

    // The Frame has no keyboard or mouse in the headset: let the controller's View button open
    // the menu and drive it.
    CVarRegisterInteger(CVAR_IMGUI_CONTROLLER_NAV, 1);
    // The game is presented as a single virtual screen, so keep popout windows inside it.
    CVarRegisterInteger(CVAR_ENABLE_MULTI_VIEWPORTS, 0);
    CVarRegisterInteger(CVAR_VSYNC_ENABLED, 1);
    CVarSave();

    // libultraship only goes fullscreen by itself on VARIANT_ID=steamdeck; otherwise it opens a
    // 640x480 window that gamescope stretches, which makes the menu enormous.
    // Config::Contains() can't be used here: for a missing nested key it finds the nearest parent.
    auto config = Ship::Context::GetInstance()->GetConfig();
    const bool fullscreenSet =
        config->GetBool("Window.Fullscreen.Enabled", false) == config->GetBool("Window.Fullscreen.Enabled", true);
    if (!fullscreenSet) {
        config->SetBool("Window.Fullscreen.Enabled", true);
    }
    // Backbuffer size of the flat window; gamescope scales it onto the virtual screen.
    if (config->GetInt("Window.Fullscreen.Width", -1) == -1 && config->GetInt("Window.Fullscreen.Height", -1) == -1) {
        config->SetInt("Window.Fullscreen.Width", 1920);
        config->SetInt("Window.Fullscreen.Height", 1080);
    }
    config->Save();
}

void SetupGui() {
    if (!IsSteamFrame()) {
        return;
    }

    // This libultraship consumes controller add/remove events before ImGui sees them, so ImGui keeps
    // the controller list from launch and View can't open the menu with a controller that connects
    // later, as the Frame's do. Have ImGui use every controller and forward those events to it.
    const Ship::WindowBackend backend = Ship::Context::GetInstance()->GetWindow()->GetWindowBackend();
    if (backend == Ship::WindowBackend::FAST3D_SDL_OPENGL || backend == Ship::WindowBackend::FAST3D_SDL_METAL) {
        ImGui_ImplSDL2_SetGamepadMode(ImGui_ImplSDL2_GamepadMode_AutoAll, nullptr, 0);
        SDL_AddEventWatch(ForwardControllerDeviceEventsToImGui, nullptr);
    }

    // Menu text is read from a virtual screen a few metres away; draw it at 150%.
    ImGui::GetStyle().ScaleAllSizes(1.5f);
    ImGui::GetIO().FontGlobalScale = 1.5f;
}

} // namespace SteamFrame
