#include "main.hpp"

#include "QuestomPlaylists.hpp"

static ModInfo modInfo;

Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;

    modInfo = info;
    
    getLogger().info("Completed QuestomPlaylists setup!");
}

extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing QuestomPlaylists hooks...");

    QuestomPlaylists::Hooks::Install();

    getLogger().info("Installed QuestomPlaylists hooks!");
}