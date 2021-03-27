#include "QuestomPlaylists.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include "GlobalNamespace/BeatmapLevelCollection.hpp"
#include "GlobalNamespace/BeatmapLevelPack.hpp"
#include "GlobalNamespace/CustomBeatmapLevelCollection.hpp"
#include "GlobalNamespace/CustomBeatmapLevelPack.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

#include <filesystem>

namespace fs = std::filesystem;

using namespace GlobalNamespace;
using namespace QuestomPlaylists;

#define PATH "/sdcard/ModData/com.beatgames.beatsaber/Mods/QuestomPlaylists/Playlists"

// This code is genuinely embarrassing.
IPreviewBeatmapLevel* FindBeatmapById(::Array<IBeatmapLevelPack*>* levelPacks, std::string id) {
    // This is actually bad in practice, but we'll do it anyways... Fuck it.
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);

    for (int i = 0; i < levelPacks->Length(); i++) {
        ::Array<IPreviewBeatmapLevel*>* beatmapLevels = (reinterpret_cast<IAnnotatedBeatmapLevelCollection*>(levelPacks->values[i]))
                    ->get_beatmapLevelCollection()
                    ->get_beatmapLevels();

        for (int j = 0; j < beatmapLevels->Length(); j++) {
            std::string levelId = to_utf8(csstrtostr(beatmapLevels->values[j]->get_levelID()));

            // Again, this is bad in practice.
            std::transform(levelId.begin(), levelId.end(), levelId.begin(), ::tolower);

            // Bodge of the year award, goes to me.
            if (id.compare(levelId) == 0) {
                return beatmapLevels->values[j];
            }
        }
    }

    return nullptr;
}

MAKE_HOOK_OFFSETLESS(
    LevelFilteringNavigationController_UpdateSecondChildControllerContent,
    void,
    LevelFilteringNavigationController* self,
    SelectLevelCategoryViewController::LevelCategory levelCategory
) {
    switch (levelCategory) {
    case SelectLevelCategoryViewController::LevelCategory::CustomSongs:
        if (self->customLevelPacks) {
            ::Array<IBeatmapLevelPack*>* customLevelPacks = ::Array<IBeatmapLevelPack*>::NewLength(
                self->customLevelPacks->get_Length() + Utils::CountFiles(fs::directory_iterator(std::string(PATH)))
            );

            // Copy over the already existing level packs.
            for (int i = 0; i < self->customLevelPacks->Length(); i++) {
                customLevelPacks->values[i] = self->customLevelPacks->values[i];
            }

            int i = self->customLevelPacks->Length() - 1;
            
            // THIS IS BULLSHIT, I SHOULDN'T HAVE TO COPYPASTE DIRECTORY ITERATOR.
            for (const auto& file : fs::directory_iterator(std::string(PATH))) {
                std::string hash = Utils::GetHash(Utils::ReadBytes(file.path()));
                rapidjson::Document doc; doc.Parse(Utils::ReadFile(file.path()).c_str());

                // TODO: Check if already exists in customLevelPacks.

                getLogger().info("packHash: %s", hash.c_str());
                
                std::string playlistTitle = doc["playlistTitle"].GetString();
                std::string playlistAuthor = doc["playlistAuthor"].GetString();

                auto songs = doc["songs"].GetArray();

                ::Array<IPreviewBeatmapLevel*>* levels = ::Array<IPreviewBeatmapLevel*>::NewLength(1);

                int j = 0;

                for (const auto& song : doc["songs"].GetArray()) {
                    std::string levelId = song["levelid"].GetString();

                    levels->values[0] = FindBeatmapById(self->allBeatmapLevelPacks, levelId);
                }

                std::string image = doc["image"].GetString();

                image.erase(0, 7); // Remove Base64 header.
                
                BeatmapLevelPack* customBeatmapLevelPack = BeatmapLevelPack::New_ctor(il2cpp_utils::createcsstr("custom_levelPack_" + hash), il2cpp_utils::createcsstr(playlistTitle),
                    il2cpp_utils::createcsstr(playlistTitle), QuestUI::BeatSaberUI::Base64ToSprite(image), reinterpret_cast<IBeatmapLevelCollection*>(BeatmapLevelCollection::New_ctor(levels)));

                customLevelPacks->values[i++] = reinterpret_cast<IBeatmapLevelPack*>(customBeatmapLevelPack);
            }

            self->customLevelPacks = customLevelPacks;

            /*::Array<CustomPreviewBeatmapLevel*>* questomPreviewLevel = ::Array<CustomPreviewBeatmapLevel*>::NewLength(1);

            questomPreviewLevel->values[0] = reinterpret_cast<CustomPreviewBeatmapLevel*>((reinterpret_cast<IAnnotatedBeatmapLevelCollection*>(self->customLevelPacks->values[0]))->get_beatmapLevelCollection()->get_beatmapLevels()->values[0]);

            CustomBeatmapLevelPack* questomLevelPack = CustomBeatmapLevelPack::New_ctor(il2cpp_utils::createcsstr("custom_levelPack_QUESTOMPLAYLIST"), il2cpp_utils::createcsstr("My Questom Playlist!"),
                        il2cpp_utils::createcsstr("My Questom Playlist!"), QuestUI::BeatSaberUI::Base64ToSprite(poggers), CustomBeatmapLevelCollection::New_ctor(questomPreviewLevel));

            customLevelPacks->values[customLevelPacks->Length() - 1] = reinterpret_cast<IBeatmapLevelPack*>(questomLevelPack);

            self->customLevelPacks = customLevelPacks;*/
        }
        break;
    }

    LevelFilteringNavigationController_UpdateSecondChildControllerContent(self, levelCategory);
}

void QuestomPlaylists::Hooks::LevelFilteringNavigationController() {
    INSTALL_HOOK_OFFSETLESS(getLogger(), LevelFilteringNavigationController_UpdateSecondChildControllerContent, il2cpp_utils::FindMethodUnsafe("", "LevelFilteringNavigationController", "UpdateSecondChildControllerContent", 1));
}