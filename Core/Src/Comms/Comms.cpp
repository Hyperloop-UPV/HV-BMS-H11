#include "Comms/Comms.hpp"

namespace HVBMS {
DatagramSocket* Comms::control_station_endpoint{nullptr};
std::unordered_map<Comms::Target, std::vector<HeapPacket*>> Comms::packets{};

void Comms::start() {
    control_station_endpoint =
        new DatagramSocket(IPV4(HVBMS_IP), CONTROL_STATION_PORT,
                           IPV4(CONTROL_SATION_IP), CONTROL_STATION_PORT);
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
}  // namespace HVBMS