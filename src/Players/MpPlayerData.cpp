#include "main.hpp"
#include "Players/MpPlayerData.hpp"

DEFINE_TYPE(MultiplayerCore::Players, MpPlayerData)

namespace MultiplayerCore::Players {
#pragma region MpPlayerData
    void MpPlayerData::New() {
        getLogger().debug("Creating MpPlayerData");
    }

    MpPlayerData* MpPlayerData::Init(StringW platformID, Platform platform)
    {
        // Always worth noting that this is something that is a manually allocated instance and as such should be handled much more carefully.
        // Your usage of this function is correct, though I still think it's worth noting somewhere, potentially with [[nodiscard]] and a comment.
        auto mpPlayerData = THROW_UNLESS(il2cpp_utils::New<MpPlayerData*, il2cpp_utils::CreationType::Manual>());

        mpPlayerData->platformId = platformID;
        mpPlayerData->platform = platform;
        getLogger().debug("Creating MpPlayerData finished");
        return mpPlayerData;
    }


    void MpPlayerData::Serialize(LiteNetLib::Utils::NetDataWriter* writer) {
        getLogger().debug("MpPlayerData::Serialize");

        writer->Put(platformId);
        writer->Put((int)platform);
        getLogger().debug("Serialize MpPlayerData done");
    }

    void MpPlayerData::Deserialize(LiteNetLib::Utils::NetDataReader* reader) {
        getLogger().debug("MpPlayerData::Deserialize");

        this->platformId = reader->GetString();
        this->platform = (Platform)reader->GetInt();
        getLogger().debug("Deserialize MpPlayerData done");
    }
#pragma endregion
}