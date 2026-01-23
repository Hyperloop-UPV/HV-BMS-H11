#include "BMSStateMachine.hpp"
#include "ST-LIB.hpp"

// Implementación de las nuevas funciones "Guardián"

namespace guards {
    bool connection_finished() {
        // TODO: Devolver 'true' cuando la conexión TCP/UDP esté establecida
        return false;
    }

    bool fault_during_precharge() {
        // Esta función parece que ahora se comprueba durante el estado OPERATIONAL
        // TODO: Comprobar si hay un fallo durante la secuencia de precarga
        return false;
    }

    bool fault_during_operation() {
        // TODO: Comprobar todas las condiciones de fallo
        return false;
    }
} // namespace guards


// Implementación de las nuevas funciones de "Acción"

namespace actions {

    // --- Acciones de Entrada a FAULT ---
    void contactors_to_open() {
        // TODO: Abrir todos los contactores
        LOG("ACTION: Opening all contactors");
    }
    void fault_to_cs() {
        // TODO: Enviar un mensaje de FAULT a la Control Station
        LOG("ACTION: Propagating FAULT to Control Station");
    }
    void open_sdc() {
        // TODO: Abrir el SDC por software
        LOG("ACTION: Opening SDC through software");
    }

    // --- Acciones de Entrada a CONNECTING ---
    void connect_tcp() {
        // TODO: Iniciar la conexión TCP
        LOG("ACTION: Initializing TCP connection");
    }
    void connect_udp() {
        // TODO: Iniciar la conexión UDP
        LOG("ACTION: Initializing UDP connection");
    }

    // --- Acciones de Entrada a OPERATIONAL ---
    void start_precharge() {
        // ADVERTENCIA: Esta acción se está ejecutando al entrar en OPERATIONAL.
        // La lógica de precarga debería ocurrir ANTES de entrar en este estado.
        LOG("ACTION: Starting precharge sequence");
    }

    // --- Acciones Cíclicas ---

    void check_precharge_status() {
        // Se llama a 1kHz en estado OPERATIONAL
        // TODO: Leer corriente y voltaje del bus durante la precarga
    }

    void read_current() {
        // Se llama a 100Hz en estado OPERATIONAL
        // TODO: Leer corriente del bus
    }

    void read_voltage() {
        // Se llama a 100Hz en estado OPERATIONAL
        // TODO: Leer voltaje del bus
    }

    void monitor_cell_voltage() {
        // Se llama a 100Hz en estado OPERATIONAL
        // TODO: Leer voltajes de las celdas
    }

    void read_sdc() {
        // Se llama a 100Hz en estado OPERATIONAL
        // TODO: Leer el estado del SDC
    }
    
    void monitor_operational_status() {
        // Esta acción está declarada pero no se usa en la máquina de estados
    }

} // namespace actions