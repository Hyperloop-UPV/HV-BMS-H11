# HV-BMS-H11 Firmware Documentation

## Overview

The HV-BMS-H11 is the High Voltage Battery Management System firmware for the HyperloopUPV pod. It runs on an STM32H723ZGT6 (Cortex-M7 @ 550 MHz) and manages a battery pack of **8 x MC33771C** cell-monitoring ICs over TPL (transformer-isolated SPI), monitoring up to **96 cells** (12 per IC) with **16 temperature channels** (2 NTC thermistors per IC via GPIO).

The firmware handles:

- Battery cell voltage and temperature monitoring via NXP MC33771C(BCC) driver chain
- Precharge / contactor sequencing (discharge, precharge, low-side, high-side)
- Insulation monitoring (Bender IR155 IMD)
- Shutdown circuit (SDC) monitoring
- DC bus voltage and current sensing via ADC
- Ethernet telemetry to control station (UDP data + TCP orders)
- Fault protection with hard-fault analysis

---

## Architecture

```
Core/Inc/HVBMS/
├── HVBMS.hpp              Top-level state machine (CONNECTING → OPERATIONAL)
├── Comms/Comms.hpp        Packet init + network startup
├── Sensors/
│   ├── BatteryH11.hpp    MC33771C (BCC) driver interface — 8 devices, 12 cells each
│   ├── BatteryPack.hpp    Legacy LTC6810 driver (unused on H11)
│   ├── Sensors.hpp        Sensor orchestrator (voltage, current, IMD, SDC, battery)
│   ├── SDC.hpp            Shutdown circuit with debouncing
│   └── IMD.hpp            Bender IR155 insulation monitor
├── Actuators/
│   ├── Actuators.hpp      Contactor management (open/close HV, precharge)
│   └── Contactor.hpp      Single contactor with feedback
├── Data/Data.hpp          Pin mappings, SPI configs, ADC, timer, global instances
└── BCC/bcc_stlib.hpp      MC33771C register init values + platform glue (SPI/TPL, timers)

Core/Src/HVBMS/
├── HVBMS.cpp              State machine transitions, order handling
├── Comms/Comms.cpp         DataPackets + OrderPackets binding to battery data
├── Sensors/
│   ├── Sensors.cpp         Init and periodic update
│   ├── SDC.cpp              EXTI callback for shutdown circuit
│   └── IMD.cpp              EXTI callback + IMD frequency decode
└── Actuators/
    ├── Actuators.cpp        Contactor sequencing
    └── Contactor.cpp        Single contactor open/close with auxiliary feedback
```

---

## State Machine

| State | Entry Action | Cyclic Actions |
|-------|-------------|----------------|
| CONNECTING | `Comms::start()`, `Sensors::battery_h11.start()` (BCC conversion) | Toggle operational LED every 300ms, update sensors every 10ms |
| OPERATIONAL | Turn on operational LED | Update sensors every 1ms |

Transitions:
- CONNECTING → OPERATIONAL: when TCP connection to control station is established
- Any state → FAULT: on SDC disengagement, IMD fault, or `FAULT` order

On fault entry: opens all HV contactors, turns off SDC forward, turns on fault LED.

---

## Battery Interface (BatteryH11.hpp — `Batteries` struct)

### Configuration

| Constant | Value | Description |
|----------|-------|-------------|
| `H11_N_BATTERIES` | 8 | Number of MC33771C ICs in TPL chain |
| `H11_N_CELLS` | 12 | Cells per IC |
| `H11_N_GPIO` | 4 | Analog GPIO inputs per IC |
| `H11_N_TEMPS` | 2 | Temperature sensors (NTC) per IC |

### Public Interface

| Method | Description |
|--------|-------------|
| `init()` | Initialize BCC driver config, write 59 init registers, clear fault registers |
| `start()` | Start global ADC conversion on all MC33771C ICs |
| `read()` | Trigger conversion (AVG_8), read cells + analog + temperatures + update SOC |
| `read_cells()` | Read all 96 cell voltages via `BCC_Meas_GetCellVoltages`, compute min/max/total |
| `read_analog()` | Read GPIO analog voltages for NTC thermistors, convert to temperatures |
| `read_current()` | Read ISENSE voltage from IC #1 |
| `update_SOC()` | Compute SOC from OCV polynomial |
| `get_min_cell()` | Return minimum cell voltage across all batteries |
| `get_max_cell()` | Return maximum cell voltage across all batteries |
| `get_total_voltage()` | Return sum of all battery voltages |
| `get_min_temperature()` | Return minimum temperature across all NTCs |
| `get_max_temperature()` | Return maximum temperature across all NTCs |
| `get_SOC()` | Return current SOC estimate |

### Data Layout

- `battery[i].cells[0..11]` — cell voltages in V for IC `i` (index 0–7)
- `battery[i].total_voltage` — sum of all 12 cells for IC `i`
- `battery[i].conv_rate` — conversion rate placeholder
- `temperature[i*2..i*2+1]` — NTC temperatures in °C for IC `i`
- `SOC` — pack-level SOC estimate (0–100%)
- `min_cell`, `max_cell` — global min/max cell voltage
- `total_voltage` — total pack voltage
- `min_temperature`, `max_temperature` — global min/max temperatures

### NTC Thermistor Formula

```
R = V_analog * 1000 / (3.0 - V_analog)   (divider with 1kΩ reference)
T = (R - 100) / (0.00385 * 100)           (beta equation approximation, R0=100Ω, TCR=0.00385)
```

### SOC OCV Polynomial

Applied to the average battery voltage:
```
SOC = -62.5 + 14.9*V + 21.9*V² - 4.18*V³   (clamped to [0, 100])
```

---

## Communications (Comms::start)

### Data Packets (UDP → control station)

| Packet ID | Name | Variables |
|-----------|------|-----------|
| 910–925 | battery_1..16 | SOC, 6 cells, total_voltage, conv_rate per packet |
| 926 | battery_17 | (reserved, bound to battery[7].cells[0..5]) |
| 927 | battery_18 | (reserved, bound to battery[7].cells[6..11]) |
| 928 | batteries_voltage | Total pack voltage |
| 930 | voltage_sensor | DC bus voltage (ADC) |
| 931 | current_sensor | DC bus current (ADC) |
| 940 | general_state_machine | GSM state |
| 942 | driver_diagnosis | BCC conversion period |
| 943 | imd | IMD status, resistance, is_ok, duty, freq |
| 944 | sdc | SDC status |
| 945 | minimum_soc | Pack minimum SOC |
| 947 | batteries_data | min/max cell voltage, min/max temperature |
| 956 | contactor_status | discharge, precharge, low, high |

### Cell Mapping

Battery packets 1–16 are mapped to the 8 MC33771C ICs, each IC split across two packets (6 cells per packet):

| Packet | IC | Cells |
|--------|-----|-------|
| battery_1 (910) | 0 | cells[0..5] |
| battery_2 (911) | 0 | cells[6..11] |
| battery_3 (912) | 1 | cells[0..5] |
| battery_4 (913) | 1 | cells[6..11] |
| ... | ... | ... |
| battery_15 (924) | 7 | cells[0..5] |
| battery_16 (925) | 7 | cells[6..11] |

SOC in all battery packets is bound to the pack-level `battery_h11.SOC`.

### Order Packets (TCP from control station)

| Order ID | Name | Action |
|----------|------|--------|
| 0 | FAULT | Triggers FAULT state |
| 901 | open_contactors | Opens all HV contactors |
| 903 | start_precharge | Starts precharge sequence |
| 904 | bypass_imd | Reserved (no IMD bypass hardware on H11) |

---

## Actuators (Contactors)

| Contactor | DO Pin | DI Pin | Type |
|-----------|--------|--------|------|
| Low-side | PD9 | PD13 | Normally Opened |
| High-side | PD8 | PD14 | Normally Opened |
| Precharge | PB14 | PG2 | Normally Opened |
| Discharge | PD10 | PD12 | Normally Closed |

Sequences:
- **Precharge**: discharge OPEN, low CLOSED, precharge CLOSED, high OPEN
- **Close HV**: discharge OPEN, low CLOSED, precharge OPEN, high CLOSED
- **Open HV**: discharge CLOSED, low OPEN, high OPEN, precharge OPEN

Precharge timeout: 4s. If voltage ratio < 0.95, opens HV and faults.

---

## Sensors

### ADC

| Channel | Pin | Sensor | Calibration |
|---------|-----|--------|-------------|
| Voltage | PA4 | DC bus voltage | slope=122.04, offset=-3.57 |
| Current | PA5 | DC bus current | slope=89.94, offset=-151.40 |

Protections: DC voltage > 410V, DC current > 120A.

### IMD (Bender IR155-32xx)

- Power enable: PE11
- OK signal: PE12 (EXTI both edges)
- PWM input capture: PA6 (TIM3 CH1)

Frequency decode:

| Freq (Hz) | Status |
|-----------|--------|
| 0 | SHORTCIRCUIT |
| 10 | NORMAL (resistance OK) |
| 20 | UNDERVOLTAGE |
| 30 | FAST_EVAL (startup) |
| 40 | EQUIPMENT_FAULT |
| 50 | GROUNDING_FAULT |

Resistance: `R = (90 * 1.2e6) / (duty - 5) - 1.2e6`

### SDC (Shutdown Circuit)

- SDC input: PB5 (EXTI both edges)
- SDC forward: PB4 (DO)
- Debounce: 10s on startup, then immediate FAULT on change

---

## SPI / TPL Bus (BCC Communication)

| Signal | Pin | Function |
|--------|-----|----------|
| SPI4 SCLK_TX | PE2 | BCC TPL clock |
| SPI4 DATA_TX | PE6 | BCC TPL data out |
| SPI5 SCLK_RX | PF7 | BCC TPL clock in |
| SPI5 DATA_RX | PF8 | BCC TPL data in |
| SPI5 CS_RX | PF6 | BCC TPL chip select (hardware) |
| CS_TX | PE4 | TPL transceiver enable (active low) |
| SPI_ENABLE | PE3 | MC33664 TPL transceiver enable |
| BATTERY_INTB | PE1 | MC33771C interrupt pin |

SPI configs: CPOL=HIGH, CPHA=2nd edge, MSB first, 8-bit, 2 MHz, DMA streams on DMA2.
- TX: Master mode (SPI4), software CS
- RX: Slave mode (SPI5), hardware CS

---

## Build & Flash

```bash
./hyper build main --preset board-debug-eth-ksz8041   # Build for custom PCB + KSZ8041 PHY
./hyper build main --preset nucleo-debug               # Build for Nucleo dev board
./hyper run main --uart                                 # Flash + serial monitor
```

Preset list in `CLAUDE.md` or `CMakePresets.json`.

---

## Pin Summary

| Pin | Function |
|-----|----------|
| PG9 | Operational LED |
| PG13 | Fault LED |
| PD8 | Contactor high (DO) |
| PD9 | Contactor low (DO) |
| PD10 | Contactor discharge (DO) |
| PB14 | Contactor precharge (DO) |
| PB4 | SDC forward fault (DO) |
| PE4 | SPI CS TX (DO) |
| PE3 | SPI enable (DO) |
| PE11 | IMD enable (DO) |
| PA4 | ADC voltage sensor |
| PA5 | ADC current sensor |
| PG2 | Aux contactor precharge (DI) |
| PD12 | Aux contactor discharge (DI) |
| PD13 | Aux contactor low (DI) |
| PD14 | Aux contactor high (DI) |
| PE1 | Battery INTB (DI) |
| PB5 | SDC interrupt (EXTI) |
| PE12 | IMD OK (EXTI) |
| PA6 | IMD PWM input capture (TIM3) |
| PE2 | SPI4 SCLK (BCC TX) |
| PE5 | SPI4 MISO (unused) |
| PE6 | SPI4 MOSI (BCC TX) |
| PF7 | SPI5 SCLK (BCC RX) |
| PF8 | SPI5 MISO (BCC RX) |
| PF9 | SPI5 MOSI (unused) |
| PF6 | SPI5 CS (BCC RX, hardware) |