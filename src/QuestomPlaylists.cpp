#include "QuestomPlaylists.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

#include "System/BitConverter.hpp"
#include "System/Security/Cryptography/SHA1.hpp"

// Shameless copypasta from SongLoader.
std::string QuestomPlaylists::Utils::GetHash(std::vector<char> chars) {
    ::Array<uint8_t>* bytes = ::Array<uint8_t>::NewLength(chars.size());

    for (int i = 0; i < bytes->Length(); i++) {
        bytes->values[i] = chars[i];
    }

    std::string hash = to_utf8(csstrtostr(System::BitConverter::ToString(System::Security::Cryptography::SHA1::Create()->ComputeHash(bytes))));

    hash.erase(std::remove(hash.begin(), hash.end(), '-'), hash.end());

    return hash;
}

std::vector<char> QuestomPlaylists::Utils::ReadBytes(std::string path) {
    return ::readbytes(path);
}

std::string QuestomPlaylists::Utils::ReadFile(std::string path) {
    return ::readfile(path);
}

int QuestomPlaylists::Utils::CountFiles(std::filesystem::directory_iterator dir) {
    return std::distance(dir, std::filesystem::directory_iterator{});
}

void QuestomPlaylists::Hooks::Install() {
    LevelFilteringNavigationController();
    TableView();
}