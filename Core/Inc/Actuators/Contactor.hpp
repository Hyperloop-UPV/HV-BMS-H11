#pragma once

#include "ST-LIB.hpp"

using ST_LIB::DigitalOutputDomain;

class Contactor {
    enum class State { OPEN, CLOSED };

    DigitalOutputDomain::Instance *output;
    bool normally_opened;
    State state;

   public:
    Contactor(DigitalOutputDomain::Instance *output, bool normally_opened);
    void open();
    bool is_open();

    void close();
    bool is_closed();
};