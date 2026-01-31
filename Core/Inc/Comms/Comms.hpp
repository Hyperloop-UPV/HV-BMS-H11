#pragma once

#include "ST-LIB.hpp"

namespace HVBMS {
class Comms {
   public:
    enum class Target { CONTROL_STATION };
    static uint32_t state_id;
   private:
    static std::unordered_map<Target, std::vector<HeapPacket*>> packets;

    // IPs
    static constexpr std::string CONTROL_SATION_IP = "192.168.0.9";

    // Ports
    static const uint16_t ORDER_PORT{50500};
    static const uint16_t CONTROL_STATION_PORT{50400};

    // Socket
    static ServerSocket* order_endpoint;
    static DatagramSocket* control_station_endpoint;

   public:
    enum class IDPacket : uint16_t {
        BATTERY_1 = 910,
        BATTERY_2 = 911,
        BATTERY_3 = 912,
        BATTERY_4 = 913,
        BATTERY_5 = 914,
        BATTERY_6 = 915,
        BATTERY_7 = 916,
        BATTERY_8 = 917,
        BATTERY_9 = 918,
        BATTERY_10 = 919,
        //BATTERY_11 = 920,
        BATTERY_12 = 921,
        BATTERY_13 = 922,
        BATTERY_14 = 923,
        BATTERY_15 = 924,
        BATTERY_16 = 925,
        BATTERY_17 = 926,
        BATTERY_18 = 927,
        TOTAL_VOLTAGE = 928,
        VOLTAGE = 930,
        CURRENT = 931,
        GENERAL_STATE_MACHINE_STATUS = 940,
        PRUEBA = 920,
        SDC = 944,
        ESTADOS = 900,
    };

    enum class IDOrder : uint16_t {
        CLOSE_CONTACTORS_ID = 900,
        OPEN_CONTACTORS_ID = 901,
        SDC_OBCCU_ID = 902,
        IMD_BYPASS_ID = 903,
        INFO = 904
    };

    static constexpr std::string HVBMS_IP = "192.168.1.7";

    static void start();
    static bool tcp_connected();
    static void add_packet(Target target, HeapPacket* packet);
    static void send_packets();
    static void create_packets();
};
}  // namespace HVBMS


