#pragma once
#include "main.hpp"
#include "Beatmaps/Abstractions/MpBeatmapLevel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

// TODO: Still work in progress CT
DECLARE_CLASS_CUSTOM_DLL(MultiplayerCore::Beatmaps, BeatSaverBeatmapLevel, Abstractions::MpBeatmapLevel, "MultiplayerCore.Beatmaps",

	DECLARE_CTOR(New, StringW hash, GlobalNamespace::IPreviewBeatmapLevel* preview);

	DECLARE_INSTANCE_FIELD(StringW, levelHash);

	// Getter overrides
	// Note that not a single one of these overrides needs to use MpBeatmapLevel, they can just use the interface.
	// They will still override the interface methods, basically, the functionality is identical but you remove a bunch of garbage.
	DECLARE_OVERRIDE_METHOD(StringW, get_levelHash, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_levelID, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelID>::get());

	DECLARE_OVERRIDE_METHOD(StringW, get_songName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songSubName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_songAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(StringW, get_levelAuthorName, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_beatsPerMinute, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(float, get_songDuration, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_levelHash>::get());
	DECLARE_OVERRIDE_METHOD(::System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::PreviewDifficultyBeatmapSet*>*, get_previewDifficultyBeatmapSets, il2cpp_utils::il2cpp_type_check::MetadataGetter<&Abstractions::MpBeatmapLevel::get_previewDifficultyBeatmapSets>::get());


	public:
		template <class TPreview>
		static LocalBeatmapLevel* CS_Ctor(StringW hash, TPreview* preview) {
			// This should realistically be a TPreview, not a TPreview*, and then it should ask if the type is convertible to IPreviewBeatmapLevel*,
			// which also allows for user defined conversion operators
			static_assert(std::is_convertible_v<std::remove_pointer_t<TPreview>, GlobalNamespace::IPreviewBeatmapLevel>, "Make sure your Type Implements and is Convertible to IPreviewBeatmapLevel*");
			// If you have checked to see that your type IS convertible, you should use static_cast here instead.
			return BeatSaverBeatmapLevel::New_ctor(hash, reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(preview));
		}

		// Conversion operators to BaseType MpBeatmapLevel
		operator Abstractions::MpBeatmapLevel* () noexcept {
			return reinterpret_cast<Abstractions::MpBeatmapLevel*>(this);
		}

		// Conversion operators to BaseType MpBeatmapLevel
		operator GlobalNamespace::IPreviewBeatmapLevel* () noexcept {
			return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this);
		}

		// Converter
		GlobalNamespace::IPreviewBeatmapLevel* get_preview() {
			return reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(this);
		}
)