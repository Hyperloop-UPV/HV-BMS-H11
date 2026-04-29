#include "HVBMS/Sensors/IMD.hpp"
#include "HVBMS/HVBMS.hpp"

void IMD::imd_fault(){
    HVBMS::state_machine.force_change_state((std::size_t)DataPackets::gsm_status::FAULT);
}