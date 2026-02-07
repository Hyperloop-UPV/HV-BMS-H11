#include "Comms/Comms.hpp"
#include "BMSStateMachine.hpp"

namespace HVBMS {
ServerSocket* Comms::order_endpoint{nullptr};
DatagramSocket* Comms::control_station_endpoint{nullptr};
std::unordered_map<Comms::Target, std::vector<HeapPacket*>> Comms::packets{};
uint32_t Comms::state_id{0};

void Comms::start() {
    order_endpoint = new ServerSocket(IPV4(HVBMS_IP), ORDER_PORT);
    control_station_endpoint =
        new DatagramSocket(IPV4(HVBMS_IP), CONTROL_STATION_PORT,
                           IPV4(CONTROL_SATION_IP), CONTROL_STATION_PORT);
}

bool Comms::tcp_connected() {
    return order_endpoint->is_connected();
}

void Comms::add_packet(Target target, HeapPacket* packet) {
    packets[target].push_back(packet);
}

void Comms::send_packets() {
    // Actualizar estado de la gsm
    state_id = static_cast<uint32_t>(state_machine.get_current_state_id());
        
    for (auto& [target, packet] : packets) {
        for (auto& p : packet) {
            switch (target) {
                case Target::CONTROL_STATION:
                    control_station_endpoint->send_packet(*p);
                    break;
            }
        }
    } 
}

void Comms::create_packets(){
    auto paquete_gsm_state = new HeapPacket(static_cast<uint16_t>(IDPacket::GENERAL_STATE_MACHINE_STATUS),
                        &state_id);
    add_packet(Target::CONTROL_STATION, paquete_gsm_state);
}

}  // namespace HVBMS