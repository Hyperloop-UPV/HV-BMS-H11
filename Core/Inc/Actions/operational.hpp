class Operational{
    private:

    public:
        // On Entry
        static void start_precharge();
        static void start_sensors();

        // Cyclic
        static void check_precharge_status();
        static void read_current();
        static void read_voltage();
        static void monitor_cell_voltage();
        static void read_sdc();
};


