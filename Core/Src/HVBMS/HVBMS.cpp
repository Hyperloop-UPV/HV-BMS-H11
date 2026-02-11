#include "HVBMS/HVBMS.hpp"

#define set_protection_name(protection, name)                 \
    {                                                         \
        protection->set_name((char*)malloc(name.size() + 1)); \
        sprintf(protection->get_name(), "%s", name.c_str());  \
    }

void HVBMS::update() {
    state_machine.check_transitions();

    if (current_gsm_state == States_HVBMS::FAULT) {
        return;
    }

    current_gsm_state = state_machine.get_current_state();
}

void HVBMS::check_bms_status() {
    for (auto& p : protections) {
        if (p->check_state() == Protections::FaultType::FAULT) {
            current_BMS_state = States_BMS::FAULT;
            return;
        }
    }
    current_BMS_state = States_BMS::OK;
}

void HVBMS::add_protections() {
    ProtectionManager::link_state_machine(state_machine,
                                          static_cast<ProtectionManager::state_id>(States_HVBMS::FAULT));

    // DC bus voltage
    Protection* protection = &ProtectionManager::_add_protection(&Sensors::voltage_sensor.reading,
                                                                 Boundary<float, ABOVE>{430});
    std::string name = "DC bus voltage";
    set_protection_name(protection, name);
    protections.push_back(protection);

    // Batteries current
    protection = &ProtectionManager::_add_protection(&Sensors::current_sensor.reading,
                                                     Boundary<float, OUT_OF_RANGE>{-15, 85});
    name = "Battery pack current";
    set_protection_name(protection, name);
    protections.push_back(protection);

    // SoCs
    auto id{1};
    for (auto& [_, soc] : Sensors::batteries.SoCs) {
        protection = &ProtectionManager::_add_protection(&soc, Boundary<float, BELOW>(0.24));
        name = "SoC battery " + std::to_string(id);
        set_protection_name(protection, name);
        protections.push_back(protection);
        ++id;
    }

    // Batteries conversion rate
    id = 1;
    for (auto& battery : Sensors::batteries.batteries) {
        protection =
            &ProtectionManager::_add_protection(&battery.conv_rate, Boundary<float, BELOW>(0.5));
        name = "Conversion rate battery " + std::to_string(id);
        set_protection_name(protection, name);
        protections.push_back(protection);
        ++id;
    }

    // Batteries temperature
    id = 1;
    for (auto& temp : Sensors::batteries.batteries_temp) {
        protection = &ProtectionManager::_add_protection(&temp[0], Boundary<float, ABOVE>(50.0));
        name = "Temperature 1 battery " + std::to_string(id);
        set_protection_name(protection, name);
        protections.push_back(protection);

        protection = &ProtectionManager::_add_protection(&temp[1], Boundary<float, ABOVE>(50.0));
        name = "Temperature 2 battery " + std::to_string(id);
        set_protection_name(protection, name);
        protections.push_back(protection);

        ++id;
    }

    ProtectionManager::initialize();
}
