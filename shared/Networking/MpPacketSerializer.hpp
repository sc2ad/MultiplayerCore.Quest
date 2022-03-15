#pragma once
#include "main.hpp"
#include "Callback.hpp"
#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"

#include <vector>
#include "System/Action_3.hpp"
#include "LiteNetLib/Utils/NetDataWriter.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/List_1.hpp"
#include "GlobalNamespace/INetworkPacketSubSerializer_1.hpp"
#include "GlobalNamespace/IConnectedPlayer.hpp"
#include "System/Type.hpp"

#include "GlobalNamespace/MultiplayerSessionManager_MessageType.hpp"

#include "Abstractions/MpPacket.hpp"


using CallbackDictionary = std::map<std::string, MultiplayerCore::CallbackBase*>;
//using TypeDictionary = std::map<Il2CppReflectionType*, std::string>;

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Networking, MpPacketSerializer, Il2CppObject,
    classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*),
    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Serialize", 2), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Deserialize", 3), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "HandlesType", 1), Il2CppReflectionType* type);

    //DECLARE_INSTANCE_METHOD(void, Serialize, LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    //DECLARE_INSTANCE_METHOD(void, Deserialize, LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    //DECLARE_INSTANCE_METHOD(bool, HandlesType, Il2CppReflectionType* type);

    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerSessionManager*, _sessionManager);

    DECLARE_CTOR(Construct, GlobalNamespace::MultiplayerSessionManager* sessionManager);
    DECLARE_DTOR(Deconstruct);

    private:
        const int Packet_ID = 100;

        CallbackDictionary packetHandlers;
        //TypeDictionary registeredTypes;

    public:
        template <class TPacket>
        void RegisterCallback(CallbackWrapper<TPacket>* callback) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            //registeredTypes[csTypeOf(TPacket)] = identifier;
            packetHandlers[static_cast<std::string>(csTypeOf(TPacket)->ToString())] = callback;
        }

        template <class TPacket>
        void UnregisterCallback() {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            getLogger().debug("UnregisterCallback called");

            //auto it = registeredTypes.find(csTypeOf(TPacket));
            //if (it != registeredTypes.end()) registeredTypes.erase(it);

            auto itr = packetHandlers.find(static_cast<std::string>(csTypeOf(TPacket)->ToString()));
            if (itr != packetHandlers.end()) {
                delete itr->second;
                packetHandlers.erase(itr);
            }
        }

        template <class TPacket>
        void Send(TPacket message) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            _sessionManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(message));
        }

        template <class TPacket>
        void SendUnreliable(TPacket message) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            _sessionManager->SendUnreliable(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(message));
        }

        template <class TPacket>
        void RegisterCallback(PacketCallback<TPacket> callback) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            CallbackWrapper<TPacket>* newCallback = new CallbackWrapper<TPacket>(callback);
            //std::string packetType = static_cast<std::string>(csTypeOf(TPacket)->ToString());
            //int pos = 0;
            //while ((pos = identifier.find("::")) != std::string::npos)
            //{
            //    identifier.replace(pos, 2, ".");
            //    pos++;
            //}
            this->RegisterCallback(newCallback);
        }

        //template <class TPacket>
        //void RegisterCallback(PacketCallback<TPacket> callback) {
        //    Il2CppReflectionType* packetType = csTypeOf(TPacket);
        //    if (packetType == nullptr) {
        //        getLogger().info("Packet Type null.");
        //        return;
        //    }

        //    std::string identifier = to_utf8(csstrtostr(packetType->ToString()));
        //    int pos = 0;
        //    while ((pos = identifier.find("::")) != std::string::npos)
        //    {
        //        identifier.replace(pos, 2, ".");
        //        pos++;
        //    }

        //    RegisterCallback<TPacket>(identifier, callback);
        //}
)
namespace MultiplayerCore {
    extern SafePtr<MultiplayerCore::Networking::MpPacketSerializer> mpPacketSerializer;
}
