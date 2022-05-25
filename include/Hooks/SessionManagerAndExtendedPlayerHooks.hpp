#pragma once
#include "Players/MpPlayerData.hpp"
#include "Beatmaps/Packets/MpBeatmapPacket.hpp"
#include "Networking/MpPacketSerializer.hpp"
#include "Networking/MpNetworkingEvents.hpp"

// Also, what's up with many things being in the MultiplayerCore namespace and many things not?
// Seems fairly odd to me... Pick one and stick with it so it's at least consistent so it's easier for others to understand please.
extern MultiplayerCore::Players::MpPlayerData* localPlayer;
extern std::unordered_map<std::string, MultiplayerCore::Players::MpPlayerData*> _playerData;


extern void HandlePlayerConnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandlePlayerDisconnected(GlobalNamespace::IConnectedPlayer* player);
extern void HandleDisconnect(GlobalNamespace::DisconnectedReason reason);

extern void HandleRegisterMpPackets(MultiplayerCore::Networking::MpPacketSerializer* _mpPacketSerializer);

static void HandleMpBeatmapPacket(MultiplayerCore::Beatmaps::Packets::MpBeatmapPacket* packet, GlobalNamespace::IConnectedPlayer* player);