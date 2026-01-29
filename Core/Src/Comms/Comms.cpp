#include "Comms/Comms.hpp"
#include "BMSStateMachine.hpp"

namespace HVBMS {
ServerSocket* Comms::order_endpoint{nullptr};
DatagramSocket* Comms::control_station_endpoint{nullptr};
std::unordered_map<Comms::Target, std::vector<HeapPacket*>> Comms::packets{};

void Comms::start() {
    order_endpoint = new ServerSocket(IPV4(HVBMS_IP), ORDER_PORT);
    control_station_endpoint =
        new DatagramSocket(IPV4(HVBMS_IP), CONTROL_STATION_PORT,
                           IPV4(CONTROL_SATION_IP), CONTROL_STATION_PORT);
}

bool Comms::tcp_connected() {
    tcp_state = order_endpoint->is_connected();
    return order_endpoint->is_connected();
}

void Comms::add_packet(Target target, HeapPacket* packet) {
    packets[target].push_back(packet);
}

void Comms::send_packets() {
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
    auto paquete_prueba = new HeapPacket(static_cast<uint16_t>(HVBMS::Comms::IDPacket::CONSTANT), &state_machine.current_state);
    HVBMS::Comms::add_packet(HVBMS::Comms::Target::CONTROL_STATION, paquete_prueba);
}

}  // namespace HVBMS