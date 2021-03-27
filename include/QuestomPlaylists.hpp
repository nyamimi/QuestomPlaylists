#pragma once

#include "main.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace QuestomPlaylists {
    namespace Utils {
        std::string GetHash(std::vector<char> chars);

        std::vector<char> ReadBytes(std::string path);

        std::string ReadFile(std::string path);

        int CountFiles(std::filesystem::directory_iterator dir);
    }

    namespace Hooks {
        void LevelFilteringNavigationController();
        void TableView();
        
        void Install();
    }
}