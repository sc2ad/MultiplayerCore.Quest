#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "Hooks/SessionManagerAndExtendedPlayerHooks.hpp"
#include "GlobalFields.hpp"
#include "Hooks/EnvironmentAndAvatarHooks.hpp"
#include "Players/MpPlayerData.hpp"
#include "UI/CenterScreenLoading.hpp"
#include "Utils/SemVerChecker.hpp"

#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"

#include "Networking/MpPacketSerializer.hpp"

#include "GlobalNamespace/IPlatformUserModel.hpp"
#include "GlobalNamespace/LocalNetworkPlayerModel.hpp"
#include "UnityEngine/Resources.hpp"
#include "GlobalNamespace/UserInfo.hpp"
#include "GlobalNamespace/PreviewDifficultyBeatmap.hpp"
#include "GlobalNamespace/MultiplayerSessionManager_SessionType.hpp"

#include "CodegenExtensions/ColorUtility.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
using namespace MultiQuestensions;
using namespace MultiplayerCore;
using namespace GlobalNamespace;

// Handles a PreviewBeatmapPacket used to transmit data about a custom song.
static void HandleMpexBeatmapPacket(Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player) {
    getLogger().debug("Player '%s' selected song '%s'", static_cast<std::string>(player->get_userId()).c_str(), static_cast<std::string>(packet->levelHash).c_str());
    BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
    Beatmaps::Abstractions::MpBeatmapLevel* preview = Beatmaps::NetworkBeatmapLevel::New_ctor(packet);
    lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), GlobalNamespace::PreviewDifficultyBeatmap::New_ctor(reinterpret_cast<IPreviewBeatmapLevel*>(preview), characteristic, packet->difficulty));
   // IPreviewBeatmapLevel* localPreview = lobbyPlayersDataModel->dyn__beatmapLevelsModel()->GetLevelPreviewForLevelId(packet->levelId);
   // MultiQuestensions::Beatmaps::PreviewBeatmapStub* preview;
   // try {
   //     if (localPreview == nullptr) {
   //         Il2CppString* nullString = nullptr;
   //         IPreviewBeatmapLevel* nullLvl = nullptr;
   //         preview = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(nullString, nullLvl, packet));
			//getLogger().info("Try getting CoverImage from BeatSaver");
			//std::string levelid = to_utf8(csstrtostr(preview->get_levelID()));
			//BeatSaver::API::GetBeatmapByHashAsync(GetHash(levelid),
   //             [preview, player, packet](std::optional<BeatSaver::Beatmap> beatmap) {
   //                 if (beatmap.has_value()) {
   //                     BeatSaver::API::GetCoverImageAsync(*beatmap, [preview, player, packet](std::vector<uint8_t> bytes) {
   //                         QuestUI::MainThreadScheduler::Schedule([packet, player, preview, bytes] {
   //                             if (packet && player && preview && lobbyPlayersDataModel) {
   //                                 preview->coverImage = QuestUI::BeatSaberUI::VectorToSprite(bytes);
   //                                 BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
   //                                 lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
   //                             }
   //                             else {
   //                                 getLogger().error("Error nullptr: packet='%p', player='%p', preview='%p', lobbyPlayersDataModel='%p'", packet, player, preview, lobbyPlayersDataModel);
   //                             }
   //                             }
   //                         );
   //                         }
   //                     );
   //                 }
   //                 else {
   //                     QuestUI::MainThreadScheduler::Schedule([packet, player, preview] {
   //                         if (packet && player && preview) {
   //                             BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
   //                             lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
   //                         }
   //                         else {
   //                             getLogger().error("Error nullptr: packet='%p', player='%p', preview='%p', lobbyPlayersDataModel='%p'", packet, player, preview, lobbyPlayersDataModel);
   //                         }
   //                         }
   //                     );
   //                 }
   //             }
   //             );
   //         return;
   //     }
   //     else {
   //         MultiQuestensions::Beatmaps::PreviewBeatmapPacket* nullpacket = nullptr;
   //         preview = THROW_UNLESS(il2cpp_utils::New<MultiQuestensions::Beatmaps::PreviewBeatmapStub*>(packet->levelHash, localPreview, nullpacket));
   //     }
   //     BeatmapCharacteristicSO* characteristic = lobbyPlayersDataModel->dyn__beatmapCharacteristicCollection()->GetBeatmapCharacteristicBySerializedName(packet->characteristic);
   //     //getLogger().debug("Check difficulty as unsigned int: %u", packet->difficulty);
   //     lobbyPlayersDataModel->SetPlayerBeatmapLevel(player->get_userId(), reinterpret_cast<IPreviewBeatmapLevel*>(preview), packet->difficulty, characteristic);
   // }
   // catch (const std::runtime_error& e) {
   //     getLogger().error("REPORT TO ENDER: %s", e.what());
   // }
   // catch (...) {
   //     getLogger().debug("REPORT TO ENDER: Unknown exception in HandleMpexBeatmapPacket");
   // }
}


//std::map<std::string, SafePtr<Extensions::ExtendedPlayer>> _extendedPlayers;
//SafePtr<Extensions::ExtendedPlayer> localExtendedPlayer;
SafePtr<MultiplayerCore::Players::MpPlayerData> localPlayer;
std::map<std::string, SafePtr<MultiplayerCore::Players::MpPlayerData>> _players;
IPlatformUserModel* platformUserModel;

static void HandleExtendedPlayerPacket(MultiplayerCore::Players::MpPlayerData* playerData, IConnectedPlayer* player) {
    const std::string userId = static_cast<std::string>(player->get_userId());
    //if (extendedPlayers->ContainsKey(player->get_userId())) {
    if (_players.contains(userId)) {
        _players.at(userId) = playerData;
        //Extensions::ExtendedPlayer* extendedPlayer = extendedPlayers->get_Item(player->get_userId());
        //extendedPlayer->_connectedPlayer = player;
        //extendedPlayer->platformID = packet->platformID;
        //extendedPlayer->platform = packet->platform;
        //extendedPlayer->playerColor = packet->playerColor;
        //extendedPlayer->mpexVersion = packet->mpexVersion;

        // Updates Color and NameTag
        //SetPlayerPlaceColor(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()), extendedPlayer->get_playerColor(), true);
        //CreateOrUpdateNameTag(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()));
    }
    else {
        getLogger().info("Received 'MpPlayerData' from '%s' with platformID: '%s' platform: '%d'",
            to_utf8(csstrtostr(player->get_userId())).c_str(),
            to_utf8(csstrtostr(playerData->platformId)).c_str(),
            (int)playerData->platform
        );
        //Extensions::ExtendedPlayer* extendedPlayer;
        //try {
        //    extendedPlayer = Extensions::ExtendedPlayer::CS_ctor(player, packet->platformID, packet->platform, packet->mpexVersion, packet->playerColor);
        //    if (to_utf8(csstrtostr(extendedPlayer->mpexVersion)) != MPEX_PROTOCOL)
        //    {
        //        getLogger().warning(
        //            "###################################################################\r\n"
        //            "Different MultiplayerExtensions protocol detected!\r\n"
        //            "The player '%s' is using MpEx protocol version %s while you are using MpEx protocol " MPEX_PROTOCOL "\r\n"
        //            "For best compatibility all players should use a compatible version of MultiplayerExtensions/MultiQuestensions.\r\n"
        //            "###################################################################",
        //            to_utf8(csstrtostr(player->get_userName())).c_str(),
        //            to_utf8(csstrtostr(extendedPlayer->mpexVersion)).c_str()
        //        );
        //    }
        //}
        //catch (const std::runtime_error& e) {
        //    getLogger().error("REPORT TO ENDER: Exception while trying to create ExtendedPlayer: %s", e.what());
        //}
        //if (extendedPlayer) {
            _players.emplace(userId, playerData);

            //SetPlayerPlaceColor(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()), extendedPlayer->get_playerColor(), true);
            //// This packet is usually received before the avatar is actually created
            //CreateOrUpdateNameTag(reinterpret_cast<IConnectedPlayer*>(extendedPlayer->get_self()));
            getLogger().debug("MpPlayerData done");
    }
}

void HandlePlayerConnected(IConnectedPlayer* player) {
    try {
        getLogger().debug("HandlePlayerConnected");
        if (player) {
            getLogger().info("Player '%s' joined", static_cast<std::string>(player->get_userId()).c_str());
            getLogger().debug("Sending MpPlayerData");
            if (localPlayer->platformId)
            {
                //Extensions::ExtendedPlayerPacket* localPlayerPacket = Extensions::ExtendedPlayerPacket::Init(localExtendedPlayer->get_platformID(), localExtendedPlayer->get_platform(), localExtendedPlayer->get_playerColor());
                //getLogger().debug("LocalPlayer Color is, R: %f G: %f B: %f", localPlayerPacket->playerColor.r, localPlayerPacket->playerColor.g, localPlayerPacket->playerColor.b);
                mpPacketSerializer->Send(static_cast<Players::MpPlayerData*>(localPlayer));
            }
            getLogger().debug("MpPlayerData sent");

            //SetPlayerPlaceColor(player, Extensions::ExtendedPlayer::DefaultColor, false);
        }
    }
    catch (const std::runtime_error& e) {
        getLogger().error("REPORT TO ENDER: %s", e.what());
    }
}

void HandlePlayerDisconnected(IConnectedPlayer* player) {
    getLogger().info("Player '%s' left", static_cast<std::string>(player->get_userId()).c_str());
    //getLogger().debug("Reseting platform lights");
    //SetPlayerPlaceColor(player, UnityEngine::Color::get_black(), true);
    _players.erase(static_cast<std::string>(player->get_userId()));
}

//void HandleDisconnect(DisconnectedReason* reason) {
//}

MAKE_HOOK_MATCH(SessionManagerStart, &MultiplayerSessionManager::Start, void, MultiplayerSessionManager* self) {

    sessionManager = self;
    SessionManagerStart(sessionManager);
    mpPacketSerializer = Networking::MpPacketSerializer::New_ctor(sessionManager);
    //packetManager = new PacketManager(sessionManager);


    mpPacketSerializer->RegisterCallback<Beatmaps::Packets::MpBeatmapPacket*>(HandleMpexBeatmapPacket);
    //mpPacketSerializer->RegisterCallback<Extensions::ExtendedPlayerPacket*>("MultiplayerExtensions.Extensions.ExtendedPlayerPacket", HandleExtendedPlayerPacket);
}

MAKE_HOOK_MATCH(SessionManager_StartSession, &MultiplayerSessionManager::StartSession, void, MultiplayerSessionManager* self, MultiplayerSessionManager_SessionType sessionType, ConnectedPlayerManager* connectedPlayerManager) {
    SessionManager_StartSession(self, sessionType, connectedPlayerManager);
    getLogger().debug("MultiplayerSessionManager.StartSession, creating localPlayer");
    //try {
        
        //localExtendedPlayer = Extensions::ExtendedPlayer::CS_ctor(self->get_localPlayer());
        //localExtendedPlayerSPTR = localExtendedPlayer;

        //if (!UnityEngine::ColorUtility::TryParseHtmlString(il2cpp_utils::newcsstr(getConfig().config["color"].GetString()), localExtendedPlayer->playerColor))
        //    localExtendedPlayer->playerColor = UnityEngine::Color(0.031f, 0.752f, 1.0f, 1.0f);

        static auto localNetworkPlayerModel = UnityEngine::Resources::FindObjectsOfTypeAll<LocalNetworkPlayerModel*>().get(0);
        static auto UserInfoTask = localNetworkPlayerModel->dyn__platformUserModel()->GetUserInfo();
        static auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void(System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>*)>)[&](System::Threading::Tasks::Task_1<GlobalNamespace::UserInfo*>* userInfoTask) {
            auto userInfo = userInfoTask->get_Result();
            if (userInfo) {
                if (!localPlayer) localPlayer = Players::MpPlayerData::Init(userInfo->dyn_platformUserId(), (Players::Platform)userInfo->dyn_platform().value);
                else {
                    localPlayer->platformId = userInfo->dyn_platformUserId();
                    localPlayer->platform = (Players::Platform)userInfo->dyn_platform().value;
                }
            }
            else getLogger().error("Failed to get local network player!");
            }
        );
        reinterpret_cast<System::Threading::Tasks::Task*>(UserInfoTask)->ContinueWith(action);

        using namespace MultiQuestensions::Utils;
        self->SetLocalPlayerState(getModdedStateStr(), true);
        self->SetLocalPlayerState(getMEStateStr(), MatchesVersion("MappingExtensions", "*"));
        self->SetLocalPlayerState(getNEStateStr(), MatchesVersion("NoodleExtensions", "*"));
        self->SetLocalPlayerState(getChromaStateStr(), MatchesVersion(ChromaID, ChromaVersionRange));
    //}
    //catch (const std::runtime_error& e) {
    //    getLogger().error("%s", e.what());
    //}
    self->add_playerConnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerConnected));
    self->add_playerDisconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<IConnectedPlayer*>*>(classof(System::Action_1<IConnectedPlayer*>*), static_cast<Il2CppObject*>(nullptr), HandlePlayerDisconnected));
    //self->add_disconnectedEvent(il2cpp_utils::MakeDelegate<System::Action_1<GlobalNamespace::DisconnectedReason>*>*>(classof(System::Action_1<GlobalNamespace::DisconnectedReason>*>*), static_cast<Il2CppObject*>(nullptr), HandleDisconnect));
}


void MultiplayerCore::Hooks::SessionManagerAndExtendedPlayerHooks() {
    INSTALL_HOOK(getLogger(), SessionManagerStart);
    INSTALL_HOOK(getLogger(), SessionManager_StartSession);
}