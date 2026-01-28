#include "vox/common/server/server_logic.hpp"

template<typename ...T>
void server_log(std::format_string<T...> fmt, T &&...values) {
	std::print("[SERVER] ");
	std::println(fmt, std::forward<T>(values)...);
}

ServerLogic::ServerLogic(std::shared_ptr<IServerDriver> p_network) : mp_network(p_network) {
	m_world.create_initial_chunks();
}

void ServerLogic::run(std::stop_token stop_token) {
	server_log("Started");

	m_tick_loop.start();
	while(!stop_token.stop_requested()) {
		m_tick_loop.accumulate();
		while(m_tick_loop.consume_tick()) {
			tick();
		}

		C2S_Packet packet;
		i32 sender_id;
		while(mp_network->PollPacket(packet, sender_id)) {
			handle_packet(packet, sender_id);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void ServerLogic::tick() {
}

template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

void ServerLogic::handle_packet(const C2S_Packet &packet, i32 sender_id) {
	std::visit(Overloaded {
		[&](const C2S_ChatMessagePacket &p)  {
			// TODO: Handle commands
			server_log("[CHAT] {}: {}", sender_id, p.message);
		},

		[&](const C2S_PlayerUpdatePacket &p)  {
		},
	}, packet);
}
