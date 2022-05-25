#include "main.hpp"
#include "Beatmaps/Providers/MpBeatmapLevelProvider.hpp"
#include "Beatmaps/LocalBeatmapLevel.hpp"
#include "Beatmaps/NetworkBeatmapLevel.hpp"
//#include "Beatmaps/BeatSaverBeatmapLevel.hpp"
using namespace MultiplayerCore::Beatmaps;
using namespace GlobalNamespace;

#include "songloader/shared/API.hpp"
using namespace RuntimeSongLoader::API;

#include "songdownloader/shared/BeatSaverAPI.hpp"
using namespace BeatSaver::API;

namespace MultiplayerCore::Beatmaps::Providers {
        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash</returns>
        IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmap(StringW levelHash) {
            auto* beatmap = GetBeatmapFromLocalBeatmaps(levelHash);
            if (beatmap)
                return beatmap;
            else {
                return GetBeatmapFromBeatSaver(levelHash);
            }
        }

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash from local, already downloaded beatmaps.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash</returns>
        IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromLocalBeatmaps(StringW levelHash) {
            std::optional<GlobalNamespace::CustomPreviewBeatmapLevel*> previewOpt = GetLevelByHash(levelHash);
            if (!previewOpt) return nullptr;
            return LocalBeatmapLevel::CS_Ctor(levelHash, previewOpt.value())->get_preview();
        }

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> for the specified level hash from BeatSaver.
        /// </summary>
        /// <param name="levelHash">The hash of the level to get</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a matching level hash, or null if none was found.</returns>
        IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromBeatSaver(StringW levelHash) {
            // std::optional<BeatSaver::Beatmap> beatmap = GetBeatmapByHash(static_cast<std::string>(levelHash));
            // if (!beatmap) return nullptr;
            // return BeatSaverBeatmapLevel(levelHash, beatmap.value());
            return nullptr;
        }
        //{
        //    Beatmap ? beatmap = await _beatsaver.BeatmapByHash(levelHash);
        //    if (beatmap == null)
        //        return null;
        //    return new BeatSaverBeatmapLevel(levelHash, beatmap);
        //}

        /// <summary>
        /// Gets an <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> from the information in the provided packet.
        /// </summary>
        /// <param name="packet">The packet to get preview data from</param>
        /// <returns>An <see cref="GlobalNamespace::IPreviewBeatmapLevel*"/> with a cover from BeatSaver.</returns>
        IPreviewBeatmapLevel* MpBeatmapLevelProvider::GetBeatmapFromPacket(MultiplayerCore::Beatmaps::Packets::MpBeatmapPacket* packet) {
            // Doing this means that you will be returning and using a dangling IPreviewBeatmapLevel*.
            // It is POSSIBLE that your lack of references to the top-level type (the NetworkBeatmapLevel) will cause it to be gc'd
            // which may also result in your IPreviewBeatmapLevel being gc'd, which means you will be left with a pointer to garbage.
            // This might be a great place to return a SafePtr instead if you fear this may happen, or something similar.
            return NetworkBeatmapLevel::New_ctor(packet)->get_preview();
        }
        //= > new NetworkBeatmapLevel(packet, _beatsaver);
}