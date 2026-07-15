#pragma once

#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"

class Comms {
    inline static HeapOrder* adj_commit_order{nullptr};
    inline static uint16_t adj_remote_id{};
    public:
    inline static bool adj_passed{false};
    static void start();
    static void check_adj_commit();
};
