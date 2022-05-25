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

// I'm honestly not sure why you need this to be CallbackBase*, especially if they are seemingly always? populated with CallbackWrapper instances.
// You could make this CallbackWrapper non-pointers or you could make this a C++ smart pointer so you don't have to ensure you are deleting correctly (see: std::unique_ptr or std::shared_ptr)
// That would seemingly work much better than your current implementation.
// Also, as always is the case, consider std::unordered_map if you don't need an order :)
using CallbackDictionary = std::map<std::string, MultiplayerCore::CallbackBase*>;

DECLARE_CLASS_CODEGEN_INTERFACES(MultiplayerCore::Networking, MpPacketSerializer, Il2CppObject,
    classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*),
    DECLARE_OVERRIDE_METHOD(void, Serialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Serialize", 2), LiteNetLib::Utils::NetDataWriter* writer, LiteNetLib::Utils::INetSerializable* packet);
    DECLARE_OVERRIDE_METHOD(void, Deserialize, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "Deserialize", 3), LiteNetLib::Utils::NetDataReader* reader, int length, GlobalNamespace::IConnectedPlayer* data);
    DECLARE_OVERRIDE_METHOD(bool, HandlesType, il2cpp_utils::FindMethodUnsafe(classof(GlobalNamespace::INetworkPacketSubSerializer_1<GlobalNamespace::IConnectedPlayer*>*), "HandlesType", 1), Il2CppReflectionType* type);

    DECLARE_INSTANCE_FIELD(GlobalNamespace::MultiplayerSessionManager*, _sessionManager);

    DECLARE_CTOR(Construct, GlobalNamespace::MultiplayerSessionManager* sessionManager);
    DECLARE_DTOR(Deconstruct);

    private:
        static constexpr uint8_t Packet_ID = 100u;

        CallbackDictionary packetHandlers;

    public:
        template <class TPacket>
        void RegisterCallback(CallbackWrapper<TPacket>* callback) {
            // You should instead check for conversions WITH the pointer, since an applicable conversion without the pointer will be applicable with the pointer.
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            packetHandlers[static_cast<std::string>(csTypeOf(TPacket)->get_Name())] = callback;
        }

        template <class TPacket>
        void UnregisterCallback() {
            // Ditto
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            getLogger().debug("UnregisterCallback called");

            auto itr = packetHandlers.find(static_cast<std::string>(csTypeOf(TPacket)->get_Name()));
            if (itr != packetHandlers.end()) {
                // If you were using smart pointers here, you wouldn't need to delete here.
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
        void Send(SafePtr<TPacket> message) {
            // This method is not useful, you should call the other one instead.
            // You can do so via: Send(*mySafePtr) and it would work just fine, for example.
            // If you REALLY want this method to still exist, at least have it call the other method so you only have the logic in one place:
            // Send(static_cast<TPacket*>(message));
            // Should be the entire content of this method.
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            _sessionManager->Send(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(static_cast<TPacket*>(message)));
        }

        template <class TPacket>
        void SendUnreliable(TPacket message) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            _sessionManager->SendUnreliable(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(message));
        }

        template <class TPacket>
        void SendUnreliable(SafePtr<TPacket> message) {
            // Ditto
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            _sessionManager->SendUnreliable(reinterpret_cast<LiteNetLib::Utils::INetSerializable*>(static_cast<TPacket*>(message)));
        }

        /*
        Registers a Packet Callback, if you specify CreationType::Manual you'll be responsible for cleanup yourself
        The above comment ^ is not very specific, because the CallbackWrapper type itself need not be manually allocated.
        Also, it is specifically the TPacket instance that is created that is controlled by the creationType NTTP, NOT the actual instance or anything else.
        // That packet is only passed in as a parameter to your various methods.
        */
        template <class TPacket, ::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary>
        void RegisterCallback(PacketCallback<TPacket> callback) {
            static_assert(std::is_convertible_v<std::remove_pointer_t<TPacket>, LiteNetLib::Utils::INetSerializable> || std::is_convertible_v<std::remove_pointer_t<TPacket>, MultiplayerCore::Networking::Abstractions::MpPacket>, "Make sure your Type Implements and is Convertible to LiteNetLib::Utils::INetSerializable*");
            CallbackWrapper<TPacket, creationType>* newCallback = new CallbackWrapper<TPacket>(callback);
            this->RegisterCallback(newCallback);
        }
)
namespace MultiplayerCore {
    extern MultiplayerCore::Networking::MpPacketSerializer* mpPacketSerializer;
}
