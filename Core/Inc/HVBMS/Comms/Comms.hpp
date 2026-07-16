#pragma once

#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"

extern "C" const char ADJ_COMMIT_HASH[16];

class Comms {
    inline static HeapOrder* adj_commit_order{nullptr};
    inline static uint64_t adj_remote_id{};
    public:
    inline static bool adj_passed{false};
    static void start();
    static void check_adj_commit();
};
