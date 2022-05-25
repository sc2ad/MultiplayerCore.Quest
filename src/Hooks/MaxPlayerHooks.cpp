#include "main.hpp"
#include "Hooks/Hooks.hpp"
#include "GlobalNamespace/MultiplayerOutroAnimationController.hpp"
#include "GlobalNamespace/CreateServerFormController.hpp"
#include "GlobalNamespace/FormattedFloatListSettingsController.hpp"

//Nothing here causes blackscreen crash


using namespace GlobalNamespace;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace UnityEngine;
using namespace UnityEngine::Playables;
using namespace UnityEngine::Timeline;

namespace MultiplayerCore {

    int targetIterations = 0;

    MAKE_HOOK_MATCH(MultiplayerResultsPyramidPatch, &MultiplayerResultsPyramidView::SetupResults, void, MultiplayerResultsPyramidView* self, IReadOnlyList_1<MultiplayerPlayerResultsData*>* resultsData, UnityEngine::Transform* badgeStartTransform, UnityEngine::Transform* badgeMidTransform) {
        try {
            static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
            static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Take_Generic, { classof(MultiplayerPlayerResultsData*) }));
            // You shouldn't call RunMethodThrow yourself unless you are POSITIVE you will never see an exception.
            // I'd suggest RunMethodRethrow instead.
            auto* takeResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5);


            static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
            static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(MultiplayerPlayerResultsData*) }));
            List<MultiplayerPlayerResultsData*>* newResultsData = il2cpp_utils::RunMethodThrow<List_1<MultiplayerPlayerResultsData*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, takeResult);

            //List<MultiplayerPlayerResultsData*>* newResultsData = Enumerable::ToList<MultiplayerPlayerResultsData*>(Enumerable::Take<MultiplayerPlayerResultsData*>(reinterpret_cast<IEnumerable_1<MultiplayerPlayerResultsData*>*>(resultsData), 5));
            MultiplayerResultsPyramidPatch(self, (IReadOnlyList_1<MultiplayerPlayerResultsData*>*)newResultsData, badgeStartTransform, badgeMidTransform);
        }
        // This can (and should) be a RunMethodException, especially after you swap from RunMethodThrow to RunMethodRethrow.
        // Otherwise, this literally won't catch anything, unless it's something bs-hook throws from the calls that aren't RunMethodThrow.
        catch (const std::runtime_error& e) {
            getLogger().critical("REPORT TO ENDER: Hook MultiplayerResultsPyramidPatch File " __FILE__ " at Line %d: %s", __LINE__, e.what());
            getLogger().debug("returning usual results pyramid");
            MultiplayerResultsPyramidPatch(self, resultsData, badgeStartTransform, badgeMidTransform);
        }
    }

    MAKE_HOOK_MATCH(IntroAnimationPatch, &MultiplayerIntroAnimationController::PlayIntroAnimation, void, MultiplayerIntroAnimationController* self, float maxDesiredIntroAnimationDuration, Action* onCompleted) {
        PlayableDirector* realDirector = self->dyn__introPlayableDirector();
        if (targetIterations == 0)
        {
            targetIterations = floor((reinterpret_cast<IReadOnlyCollection_1<GlobalNamespace::IConnectedPlayer*>*>(self->dyn__multiplayerPlayersManager()->dyn__allActiveAtGameStartPlayers())->get_Count() - 1) / 4) + 1;
        }
        try {
            // Run animation one time for each set of 4 players
            if (targetIterations != 1) {
                getLogger().debug("starter animaton");
                // Create duplicated animations
                GameObject* newPlayableGameObject = GameObject::New_ctor();
                self->dyn__introPlayableDirector() = newPlayableGameObject->AddComponent<PlayableDirector*>();

                using SetPlayableAsset = function_ptr_t<void, Il2CppObject*, PlayableAsset*>;
                // See il2cpp_utils::resolve_icall :)
                static SetPlayableAsset setPlayableAsset = reinterpret_cast<SetPlayableAsset>(il2cpp_functions::resolve_icall("UnityEngine.Playables.PlayableDirector::SetPlayableAsset"));
                setPlayableAsset(self->dyn__introPlayableDirector(), realDirector->get_playableAsset());

                // Mute duplicated animations except one (otherwise audio is very loud)
                TimelineAsset* mutedTimeline = reinterpret_cast<TimelineAsset*>(self->dyn__introPlayableDirector()->get_playableAsset());

                // Try not to cache the same methods in multiple places, it increases size of your mod and decreases performance for no real gain.
                // Note that generic sharing will take place, so Enumerable_ToList being a MakeGenericMethod for even an Il2CppObject* will work for all reference types
                // including TrackAsset* and MultiplayerResultsData*, for example.
                // You should consider moving these Enumerable methods to another type/header file imo.
                static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
                static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(TrackAsset*) }));

                //List<TrackAsset*>* outputTracks = Enumerable::ToList<TrackAsset*>(animationTimeline->GetOutputTracks());
                // No RunMethodThrow
                List<TrackAsset*>* outputTracks = il2cpp_utils::RunMethodThrow<List_1<TrackAsset*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_ToList, mutedTimeline->GetOutputTracks());

                for (int i = 0; i < outputTracks->get_Count(); i++) {
                    TrackAsset* currentTrack = outputTracks->get_Item(i);
                    bool isAudio = il2cpp_utils::AssignableFrom<AudioTrack*>(reinterpret_cast<Il2CppObject*>(currentTrack)->klass);
                    currentTrack->set_muted(isAudio);
                }
            }

            self->dyn__bindingFinished() = false;
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
            // Reset director to real director
            self->dyn__introPlayableDirector() = realDirector;
            //targetIterations--;
            if (targetIterations-1 != 0)
                self->PlayIntroAnimation(maxDesiredIntroAnimationDuration, onCompleted);
        }
        // Same issue with exceptions as mentioned above, except in this case there are more potential ways this exception could be thrown.
        // You probably want to catch everything generically anyways, so it might be worth just letting it bubble up to the IL2CPP_CATCH_HANDLER that wraps this hook
        // or writing your own catch handler.
        catch (const std::runtime_error& e) {
            // Reset director to real director
            self->dyn__introPlayableDirector() = realDirector;
            getLogger().critical("REPORT TO ENDER: Hook IntroAnimationPatch Exception: %s", e.what());
            IntroAnimationPatch(self, maxDesiredIntroAnimationDuration, onCompleted);
        }
    }

    enum CalculatePlayerIndexSequencePatchType {
        None, BindTimeline, BindOutroTimeline
    };

    CalculatePlayerIndexSequencePatchType cpispt = None;

    MAKE_HOOK_MATCH(MultiplayerIntroAnimationController_BindTimeline, &MultiplayerIntroAnimationController::BindTimeline, void, MultiplayerIntroAnimationController* self) {
        getLogger().debug("Start: MultiplayerIntroAnimationController_BindTimeline");
        cpispt = BindTimeline;
        MultiplayerIntroAnimationController_BindTimeline(self);
    }

    MAKE_HOOK_MATCH(MultiplayerOutroAnimationController_BindOutroTimeline, &MultiplayerOutroAnimationController::BindOutroTimeline, void, MultiplayerOutroAnimationController* self) {
        getLogger().debug("Start: MultiplayerOutroAnimationController_BindOutroTimeline");
        cpispt = BindOutroTimeline;
        MultiplayerOutroAnimationController_BindOutroTimeline(self);
    }

    MAKE_HOOK_MATCH(MultiplayerPlayersManager_get_allActiveAtGameStartPlayers, &MultiplayerPlayersManager::get_allActiveAtGameStartPlayers, IReadOnlyList_1<GlobalNamespace::IConnectedPlayer*>*, MultiplayerPlayersManager* self) {
        getLogger().debug("Start: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers");
        if (targetIterations == 0)
        {
            // Probably not a debug log, then. Also should probably crash the game noticably or something or just be removed.
            getLogger().debug("THIS SHOULD NEVER RUN");
            targetIterations = floor((reinterpret_cast<IReadOnlyCollection_1<GlobalNamespace::IConnectedPlayer*>*>(self->dyn__allActiveAtGameStartPlayers())->get_Count() - 1) / 4) + 1;
        }
        // Same argument for these.
        static auto* Enumerable_ToList_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToList", 1));
        static auto* Enumerable_ToList = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToList_Generic, { classof(IConnectedPlayer*) }));

        static auto* Enumerable_Take_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Take", 2));
        static auto* Enumerable_Take = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Take_Generic, { classof(IConnectedPlayer*) }));

        if (cpispt == BindTimeline) {
            cpispt = None;
            try {
                getLogger().debug("Getting intro active players (skipping (target iterations-1)*4, Taking 4 then adding player)");
                List_1<IConnectedPlayer*>* listActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_ToList, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(self->dyn__allActiveAtGameStartPlayers()));
                //List<IConnectedPlayer*>* listActivePlayers = Enumerable::ToList<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(allActivePlayer));
                IConnectedPlayer* localPlayer = nullptr;

                // Check if active players contains local player and remove local player
                for (int i = 0; i < listActivePlayers->get_Count(); i++) {
                    IConnectedPlayer* currentPlayer = listActivePlayers->get_Item(i);
                    if (currentPlayer->get_isMe()) {
                        listActivePlayers->RemoveAt(i);
                        localPlayer = currentPlayer;
                    }
                }

                // Skip x amount of players and then take 4
                static auto* Enumerable_Skip_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "Skip", 2));
                static auto* Enumerable_Skip = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_Skip_Generic, { classof(IConnectedPlayer*) }));
                // Same argument for RunMethodThrow
                auto* skipResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_Skip, reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4);

                auto* takeResult = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_Take, skipResult, 4);

                List_1<IConnectedPlayer*>* selectedActivePlayers = il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                    Enumerable_ToList, takeResult);

                //List_1<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(Enumerable::Skip<IConnectedPlayer*>(reinterpret_cast<IEnumerable_1<IConnectedPlayer*>*>(listActivePlayers), (targetIterations - 1) * 4), 4));
                //List_1<IConnectedPlayer*>* selectedActivePlayers = Enumerable::ToList<IConnectedPlayer*>(Enumerable::Take<IConnectedPlayer*>(skipResult, 4));

                // Add back local player if not null
                if (targetIterations == 1 && localPlayer != nullptr) {
                    selectedActivePlayers->Add(localPlayer);
                }

                getLogger().debug("Finish: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers(intro)");

                //return new list of players
                return reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(selectedActivePlayers);
            }
            catch (const std::runtime_error& e) {
                // Same argument for this.
                getLogger().critical("REPORT TO ENDER: Hook MultiplayerPlayersManager_get_allActiveAtGameStartPlayers Exception: %s", e.what());
                return self->dyn__allActiveAtGameStartPlayers();
            }
        }
        else if (cpispt == BindOutroTimeline) {
            cpispt = None;
            getLogger().debug("Getting outro active players(first 4 in list)");
            // Please just ctrl-shift-f for RunMethodThrow and borderline swap all of them to be RunMethodRethrow.
            // They should have the same syntax as far as you use them.
            auto* result = il2cpp_utils::RunMethodThrow<IEnumerable_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_Take, reinterpret_cast<List_1<IConnectedPlayer*>*>(self->dyn__allActiveAtGameStartPlayers()), 4);
            
            getLogger().debug("Finish: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers(outro)");
            getLogger().debug("RESETTING TARGET ITERATIONS");
            targetIterations = 0;
            return reinterpret_cast<IReadOnlyList_1<IConnectedPlayer*>*>(il2cpp_utils::RunMethodThrow<List_1<IConnectedPlayer*>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToList, result));
        }
        cpispt = None;
        getLogger().debug("Getting all active players");
        getLogger().debug("Finish: MultiplayerPlayersManager_get_allActiveAtGameStartPlayers(ALL)");
        return self->dyn__allActiveAtGameStartPlayers();
    }

    MAKE_HOOK_MATCH(CreateServerFormController_get_formData, &CreateServerFormController::get_formData, CreateServerFormData, CreateServerFormController* self) {
        CreateServerFormData result = CreateServerFormController_get_formData(self);
        result.maxPlayers = std::clamp<int>(self->dyn__maxPlayersList()->get_value(), 2, std::clamp(getConfig().config["MaxPlayers"].GetInt(), 2, 120));
        return result;
    }

    MAKE_HOOK_MATCH(CreateServerFormController_Setup, &CreateServerFormController::Setup, void, CreateServerFormController* self, int selectedNumberOfPlayers, bool netDiscoverable) {
        try {
            std::vector<int> rangeVec;
            static auto* Enumerable_ToArray_Generic = THROW_UNLESS(il2cpp_utils::FindMethodUnsafe(classof(Enumerable*), "ToArray", 1));
            static auto* Enumerable_ToArray = THROW_UNLESS(il2cpp_utils::MakeGenericMethod(Enumerable_ToArray_Generic, { classof(int) }));
            il2cpp_utils::RunMethodThrow<::Array<int>*, false>(static_cast<Il2CppClass*>(nullptr),
                Enumerable_ToArray, Enumerable::Range(2, std::clamp(getConfig().config["MaxPlayers"].GetInt(), 2, 120) - 1))->copy_to(rangeVec);
            //Enumerable::ToArray(Enumerable::Range(2, 9))->copy_to(rangeVec);
            std::vector<float> resultVec(rangeVec.begin(), rangeVec.end());
            self->dyn__maxPlayersList()->dyn__values() = il2cpp_utils::vectorToArray(resultVec);
        } catch (const std::runtime_error& e) {
            // Yada yada same exception stuff
            getLogger().critical("REPORT TO ENDER: Hook CreateServerFormController_Setup Exception: %s", e.what());
        }
        CreateServerFormController_Setup(self, selectedNumberOfPlayers, netDiscoverable);
    }

    void Hooks::MaxPlayerHooks() {
        INSTALL_HOOK(getLogger(), MultiplayerResultsPyramidPatch);
        INSTALL_HOOK(getLogger(), IntroAnimationPatch);
        INSTALL_HOOK(getLogger(), MultiplayerIntroAnimationController_BindTimeline);
        INSTALL_HOOK(getLogger(), MultiplayerOutroAnimationController_BindOutroTimeline);

        INSTALL_HOOK_ORIG(getLogger(), MultiplayerPlayersManager_get_allActiveAtGameStartPlayers);
        INSTALL_HOOK(getLogger(), CreateServerFormController_get_formData);
        INSTALL_HOOK(getLogger(), CreateServerFormController_Setup);

    }
}