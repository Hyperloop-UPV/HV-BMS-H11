#pragma once

#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;

class Contactor {
    DigitalOutputDomain::Instance* output;
    DigitalInputDomain::Instance* input;
    bool normally_opened;

public:
    Contactor() = default;
    Contactor(DigitalOutputDomain::Instance* output, DigitalInputDomain::Instance* input, bool normally_opened);
    void open();
    bool is_open();

    void close();
    bool is_closed();
};
