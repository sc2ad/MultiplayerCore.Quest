#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "GlobalNamespace/LoadingControl.hpp"
#include "GlobalNamespace/CenterStageScreenController.hpp"

DECLARE_CLASS_CODEGEN(MultiplayerCore::UI, CenterScreenLoading, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LoadingControl*, loadingControl);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::CenterStageScreenController*, screenController);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::LobbyGameStateController*, gameStateController);

    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnDisable);
    DECLARE_INSTANCE_METHOD(void, ShowLoading);
    // Note: downloadingProgress 1.0f = Full
    DECLARE_INSTANCE_METHOD(void, ShowDownloadingProgress, float downloadingProgress);
    //DECLARE_INSTANCE_METHOD(void, HideLoading);
    DECLARE_INSTANCE_METHOD(void, FixedUpdate);
    // How many of these methods need to be marked as C# methods? Are they all looked up via reflection? Perhaps ShowLoading and ShowDownloadingProgress can be fixed.

    bool isDownloading;

    static CenterScreenLoading* instance;
public:
    static int playersReady;
    // This should also be inline, but it shouldn't matter
    static CenterScreenLoading* get_Instance() { return instance; }
    static void HideLoading();
    //static void UpdatePlayersReady(int playerReady);
)
