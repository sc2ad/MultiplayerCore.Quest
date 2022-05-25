#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"

#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "songdownloader/shared/BeatSaverAPI.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "System/Threading/Tasks/Task_1.hpp"
#include "System/Threading/Tasks/TaskStatus.hpp"

#include "UnityEngine/ImageConversion.hpp"

#include "GlobalNamespace/MediaAsyncLoader.hpp"

#include <vector>

// Y'know, I'm really not sure why you even have this type...
// You could just as well have chosen a base type that _didn't_ implement the interface and thus would not need all of these odd no-op functions.
// Plus, it's unclear if you really even need this type to begin with, you could have just used the interface for some of your comparisons
// and because it's a pointer in your collections, be able to perfectly convert to whatever type you actually need.
// Seems redundant.
DEFINE_TYPE(MultiplayerCore::Beatmaps::Abstractions, MpBeatmapLevel);

namespace MultiplayerCore::Beatmaps::Abstractions {

	// Don't need a ctor for this type, don't bother declaring one.
	// The only potential oddities that could arise from removing this would involve calling base constructors and how you may be doing that.
	void MpBeatmapLevel::New() {}

	System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* MpBeatmapLevel::GetCoverImageAsync(System::Threading::CancellationToken cancellationToken) {
		return System::Threading::Tasks::Task_1<UnityEngine::Sprite*>::New_ctor(static_cast<UnityEngine::Sprite*>(nullptr));
	}

#pragma region All the pain in form of getter functions

	StringW MpBeatmapLevel::get_levelHash() {
		return StringW();
	}


	StringW MpBeatmapLevel::get_levelID() {
		// Why? get_levelHash() will always be the same call and isn't virtual or overridable so this won't actually do anything different per instance.
		// This function will just always crash/throw an exception because it will do:
		// - Make a StringW that wraps a "custom_level_"
		// - Add an empty (uninitialized) StringW to it (that holds the nullptr)
		// - Will resolve to calling string + null
		return StringW("custom_level_") + get_levelHash();
	}

	StringW MpBeatmapLevel::get_songName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_songSubName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_songAuthorName() {
		return StringW();
	}

	StringW MpBeatmapLevel::get_levelAuthorName() {
		return StringW();
	}

	float MpBeatmapLevel::get_beatsPerMinute() {
		return 0;
	}

	float MpBeatmapLevel::get_songDuration() {
		return 0;
	}

	float MpBeatmapLevel::get_songTimeOffset() {
		return 0;
	}

	float MpBeatmapLevel::get_previewDuration() {
		return 0;
	}

	float MpBeatmapLevel::get_previewStartTime() {
		return 0;
	}

	float MpBeatmapLevel::get_shuffle() {
		return 0;
	}

	float MpBeatmapLevel::get_shufflePeriod() {
		return 0;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_allDirectionsEnvironmentInfo() {
		return nullptr;
	}

	GlobalNamespace::EnvironmentInfoSO* MpBeatmapLevel::get_environmentInfo() {
		return nullptr;
	}

	ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*> MpBeatmapLevel::get_previewDifficultyBeatmapSets() {
		return ArrayW<GlobalNamespace::PreviewDifficultyBeatmapSet*>();
	}

#pragma endregion

}