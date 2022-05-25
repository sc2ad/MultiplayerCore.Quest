#include "main.hpp"
#include "CS_DataStore.hpp"

DEFINE_TYPE(MultiplayerCore, DataStore);

namespace MultiplayerCore {
    void DataStore::New(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
        loadingGameplaySetupData = gameplaySetupData;
        loadingDifficultyBeatmap = difficultyBeatmap;
    }

    DataStore* DataStore::instance;

    DataStore* DataStore::get_Instance() {
        return instance;
    }

    DataStore* DataStore::CS_Ctor(GlobalNamespace::ILevelGameplaySetupData* gameplaySetupData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap) {
        //instance = THROW_UNLESS(il2cpp_utils::New<MultiplayerCore::DataStore*, il2cpp_utils::CreationType::Manual>(gameplaySetupData, difficultyBeatmap));
        // You should seriously consider not making any of these if you already have an instance.
        // ex: if (!instance) { line below }
        // This will ensure that you don't keep redundantly allocating new CS_Ctor instances because of how you call this function elsewhere.
        instance = MultiplayerCore::DataStore::New_ctor<il2cpp_utils::CreationType::Manual>(gameplaySetupData, difficultyBeatmap);
        return instance;
    }

    void DataStore::Clear() {
        if (instance) {
            instance->loadingGameplaySetupData = nullptr;
            instance->loadingDifficultyBeatmap = nullptr;
        }
    }
}