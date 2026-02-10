#pragma once

#include "Communications/Packets/DataPackets.hpp"
#include "Communications/Packets/OrderPackets.hpp"
#include "BMSStateMachine.hpp"
#include "Sensors/Sensors.hpp"
#include "Sensors/BatteryPack.hpp"
#include "Sensors/SDC.hpp"

namespace HVBMS {
class Comms {
    public:
        static void start();
        static void update();
};
}  // namespace HVBMS


