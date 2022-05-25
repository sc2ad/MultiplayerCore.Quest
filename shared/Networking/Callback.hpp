#pragma once
#include "main.hpp"

#include "GlobalNamespace/ThreadStaticPacketPool_1.hpp"
#include "GlobalNamespace/PacketPool_1.hpp"
#include "LiteNetLib/Utils/NetDataReader.hpp"

template <class T>
using PacketCallback = function_ptr_t<void, T, GlobalNamespace::IConnectedPlayer*>;

namespace MultiplayerCore {
	class CallbackBase {
	public:
		virtual ~CallbackBase() {};
		virtual void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) = 0;
	};

	template <class TPacket, ::il2cpp_utils::CreationType creationType = ::il2cpp_utils::CreationType::Temporary> class CallbackWrapper : public CallbackBase {
	private:
		PacketCallback<TPacket> action = nullptr;

	public:
		CallbackWrapper(PacketCallback<TPacket> callback) {
			action = callback;
		}

		void Invoke(LiteNetLib::Utils::NetDataReader* reader, int size, GlobalNamespace::IConnectedPlayer* player) {
			getLogger().debug("Running Invoke creating packet");
			TPacket packet;
			// Why not: packet = THROW_UNLESS(il2cpp_utils::New<TPacket, creationType>())?
			// Also, consider using `il2cpp_utils::NewSpecific` instead of `New`, should be a nice optimization.
			if constexpr (creationType == ::il2cpp_utils::CreationType::Temporary)
				packet = THROW_UNLESS(il2cpp_utils::New<TPacket>());
			else
				packet = THROW_UNLESS(il2cpp_utils::New<TPacket, il2cpp_utils::CreationType::Manual>());

			//getLogger().debug("Assigning from ThreadStaticPacketPool");
			//packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			// TPacket packet = GlobalNamespace::ThreadStaticPacketPool_1<TPacket>::get_pool()->Obtain();
			// If packet is null at this point, THROW_UNLESS would have already thrown. This is a dead condition.
			if (packet == nullptr) {
				reader->SkipBytes(size);
			}
			else {
				try {
					packet->Deserialize(reader);
					// If this throws a conventional RunMethodException or a StackTraceException, .what() will not log the stack trace.
					// It might be worth it to wrap this in an IL2CPP_CATCH_HANDLER() instead, or catch RunMethodException explicitly.
				}
				catch (const std::exception& e) {
					getLogger().debug("Exception Deserializing Packet");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
			if (action != nullptr) {
				try {
					action(packet, player);
					// Ditto, though in this case, this is far more likely to be a C++ call, so perhaps less valuable.
				}
				catch (const std::exception& e) {
					getLogger().error("Exception running action");
					getLogger().error("REPORT TO ENDER: %s", e.what());
				}
				catch (...) {
					getLogger().debug("REPORT TO ENDER: Unknown exception Deserializing Packet");
				}
			}
			// At the end of this scope, if packet was created manually, you will leak memory! Make sure you deallocate it!
			// Use: gc_free_specific(packet) assuming it is a pointer AND it was manually allocated.
		}
	};
}