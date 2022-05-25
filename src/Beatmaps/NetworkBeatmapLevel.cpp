#include "Beatmaps/NetworkBeatmapLevel.hpp"
#include "custom-types/shared/register.hpp"
#include "GlobalNamespace/VarIntExtensions.hpp"
#include "songloader/shared/API.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

DEFINE_TYPE(MultiplayerCore::Beatmaps, NetworkBeatmapLevel);

namespace MultiplayerCore::Beatmaps {
	void NetworkBeatmapLevel::New(Packets::MpBeatmapPacket* packet) {
		_packet = packet;
		this->levelID = StringW(RuntimeSongLoader::API::GetCustomLevelsPrefix().c_str()) + _packet->levelHash;
	}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* NetworkBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		if (!coverImageTask) {
			// Few things about this:
			// 1. The task is started when you construct it, you don't need to set the state accordingly.
			// 2. If, while you are going to the beatsaver API to get the beatmap, 'this' no longer exists for some reason, you will crash painfully.
			// Your coverImageTask should not be gc'd while your this instance is alive, so long as you create these instances in a conventional way
			// ex: New, NewSpecific, New_ctor, ..., but IF YOUR INSTANCE DOES DIE, especially while being used in the nested async calls, you could have some pain.
			coverImageTask = System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
			reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->dyn_m_stateFlags() = System::Threading::Tasks::Task::TASK_STATE_STARTED;

			BeatSaver::API::GetBeatmapByHashAsync(_packet->levelHash,
				[this](std::optional<BeatSaver::Beatmap> beatmap) {
					if (beatmap.has_value()) {
						BeatSaver::API::GetCoverImageAsync(*beatmap, [this](std::vector<uint8_t> bytes) {
							QuestUI::MainThreadScheduler::Schedule([this, bytes] {
								getLogger().debug("Got coverImage from BeatSaver");
								coverImageTask->TrySetResult(QuestUI::BeatSaberUI::VectorToSprite(bytes));
								// This is also not necessary, it does this in the TrySetResult call.
								reinterpret_cast<System::Threading::Tasks::Task*>(coverImageTask)->dyn_m_stateFlags() = System::Threading::Tasks::Task::TASK_STATE_RAN_TO_COMPLETION;
								}
							);
							}
						);
					}
				}
			);
		}
		if (coverImageTask)
			return coverImageTask;
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}


#pragma region All the pain in form of getter functions

	StringW NetworkBeatmapLevel::get_levelID() {
		return StringW(RuntimeSongLoader::API::GetCustomLevelsPrefix().c_str()) + _packet->levelHash;
	}

	StringW NetworkBeatmapLevel::get_levelHash() {
		return _packet->levelHash;
	}

	StringW NetworkBeatmapLevel::get_songName() {
		return _packet->songName;
	}

	StringW NetworkBeatmapLevel::get_songSubName() {
		return _packet->songSubName;
	}

	StringW NetworkBeatmapLevel::get_songAuthorName() {
		return _packet->songAuthorName;
	}

	StringW NetworkBeatmapLevel::get_levelAuthorName() {
		return _packet->levelAuthorName;
	}

	float NetworkBeatmapLevel::get_beatsPerMinute() {
		return _packet->beatsPerMinute;
	}

	float NetworkBeatmapLevel::get_songDuration() {
		return _packet->songDuration;
	}

	//float NetworkBeatmapLevel::get_songTimeOffset() {
	//	return 0;
	//}

	//float NetworkBeatmapLevel::get_previewDuration() {
	//	return 0;
	//}

	//float NetworkBeatmapLevel::get_previewStartTime() {
	//	return 0;
	//}

	//float NetworkBeatmapLevel::get_shuffle() {
	//	return 0;
	//}

	//float NetworkBeatmapLevel::get_shufflePeriod() {
	//	return 0;
	//}

	//GlobalNamespace::EnvironmentInfoSO* NetworkBeatmapLevel::get_allDirectionsEnvironmentInfo() {
	//	return nullptr;
	//}

	//GlobalNamespace::EnvironmentInfoSO* NetworkBeatmapLevel::get_environmentInfo() {
	//	return nullptr;
	//}

	//::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>* NetworkBeatmapLevel::get_previewDifficultyBeatmapSets() {
	//	return nullptr;
	//}

#pragma endregion
}
