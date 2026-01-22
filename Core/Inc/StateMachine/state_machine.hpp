#include <array>
#include <cstdint>
#include <cstddef>

typedef void (*state_t)(void);

// State machine basada en funciones
template<size_t N>
class StateMachine{
    private:
        std::array<state_t, N> states;
        state_t current_state;
    public: 
        StateMachine(const std::array<state_t, N>& s);
        void update();
};

void update_state_machines();