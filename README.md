# Adaptive Street Light Control Using Time and Environmental Illumination

## 1. Project Description & Objectives
### Description
Conventional street lighting relies either on manual switching—leading to human error and daytime energy wastage—or standalone LDR circuits that falsely trigger during passing shadows, heavy cloud cover, or vehicle headlights. 

This project implements a hybrid **Adaptive Street Light Control System** on an **NXP LPC2148 / LPC2138 ARM7TDMI-S** microcontroller. By combining **internal Real-Time Clock (RTC)** time-window scheduling with **ambient illumination (Lux) sensing** via an LDR and 10-bit ADC (`ADC0`), street lights operate strictly when both scheduled night hours and low ambient light conditions (`< 50 Lux`) are met. An external interrupt (`EINT0`) and a `4x4` matrix keypad allow authorized technicians to modify the RTC time or ON/OFF schedule directly on-site.

### Objectives
* Prevent daytime electricity wastage through dual-condition (Time + Lux) switching.
* Eliminate false switching caused by temporary shadows or bright vehicle headlights.
* Display real-time clock (`HH:MM:SS`), calibrated environmental Lux (`0–930 Lux`), and street light status on a `16x2` alphanumeric LCD.
* Provide interrupt-driven (`EINT0`) field configuration via a `4x4` matrix keypad without reflashing firmware.

---

## 2. Block Diagram & Workflow
### Block Diagram
*(Upload your block diagram image to `Adaptive Street Light Control Diagram (1).png`

### Operational Workflow
1. **Initialization:** Configure PLL (`60 MHz` CCLK from `12 MHz` crystal), GPIO directions, `16x2` LCD, `10-bit ADC0`, `4x4` matrix keypad, internal RTC, and `EINT0` via the Vectored Interrupt Controller (VIC).
2. **Data Acquisition:** Continuously read `HOUR`, `MIN`, and `SEC` from RTC registers and sample the LDR voltage divider on `ADC0.1` (`P0.28`), converting raw 10-bit counts into calibrated integer **Lux** values (`0–930 Lux`).
3. **Live Monitoring:** Update the `16x2` LCD dashboard with current time, ambient Lux reading, and street light load state (`ON` / `OFF`).
4. **Adaptive Control Decision:**
   * **Condition A (Time):** Current RTC time falls within the configured night schedule (e.g., `17:30` to `06:00`).
   * **Condition B (Illumination):** Measured ambient light is below `50 Lux`.
   * **Output:** Energize active-low street light LEDs (`P1.25–P1.27`) only when **both** Condition A and Condition B are true.
5. **Interrupt Configuration (`EINT0`):** Pressing the edit switch triggers `EINT0_ISR`, opening an interactive `4x4` keypad menu to update RTC time or ON/OFF schedule hours.

---

## 3. Hardware & Software Requirements
### Hardware Components
* **Microcontroller:** NXP LPC2148 / LPC2138 ARM7TDMI-S (32-bit, `12 MHz` crystal oscillator)
* **Light Sensor:** LDR with $10\text{ k}\Omega$ voltage-divider resistor connected to `P0.28` (`AD0.1`)
* **Display:** `16x2` Alphanumeric LCD (`D0–D7` on `P0.8–P0.15`, `RS`/`RW`/`EN` on `P0.16–P0.18`)
* **Input Interface:** `4x4` Matrix Keypad (`P1.16–P1.23`) & tactile interrupt push button on `EINT0` (`P0.1`)
* **Output Load:** Active-low Street Light LEDs on `P1.25`, `P1.26`, `P1.27` with $330\ \Omega$ current-limiting resistors
* **Power Supply:** Regulated `3.3 V` (MCU core/I/O) and `5.0 V` (LCD & peripherals)

### Software Tools & Modular Libraries
* **Keil $\mu\text{Vision}$ IDE & ArmCC Toolchain:** Firmware compilation and `.hex` file generation
* **Proteus ISIS Design Suite:** Circuit schematic design and real-time firmware simulation
* **Flash Magic:** UART0 In-System Programming (ISP) utility for flashing LPC2148 hardware
* **Custom Modular Drivers (`/Streetlightcode`):**
  * `types.h` & `defines.h` — Fixed-width integer typedefs and bit-manipulation macros (`SETBIT`, `CLRBIT`, `READBIT`, `WRITEBYTE`)
  * `delay.c` / `delay.h` — Microsecond, millisecond, and second delay routines calibrated for `12 MHz`
  * `adc.c` / `adc.h` — `ADC0` initialization and integer Lux conversion driver
  * `lcd.c` / `lcd.h` — `16x2` LCD command, character, string, and integer display drivers
  * `kpm2.c` / `kpm.h` — `4x4` matrix keypad scanning and debouncing routines
  * `rtc.c` / `rtc.h` & `main.c` — Internal RTC scheduling, `EINT0` ISR, and control loop

---

## 4. Circuit Diagram & Schematics
* **Hardware Kit Setup:** `Adaptive-Street-Light-Control-LPC2148-lcdscreen-picture-format.jpeg`
* * **Hardware Kit Setup:** `Adaptive-Street-Light-Control-LPC2148-vedio-format.mp4`

---

## 5. Complete Source Code
All modular `.c`, `.h`, and `_defines.h` source files are located in the [`/Streetlightcode`](./Streetlightcode) directory.

---

## 6. Steps to Build and Execute
1. Clone or download this repository and open the project folder in **Keil $\mu\text{Vision}$**.
2. Select target device **NXP LPC2148** (or **LPC2138**) with `Startup.s` included.
3. Add all `.c` files from `Streetlightcode/` into **Source Group 1**.
4. Open **Options for Target $\rightarrow$ Target**, set `XTAL = 12.0 MHz`, and enable **Create HEX File** under the **Output** tab.
5. Press **F7** to build the project and generate the `.hex` firmware file.
6. **Simulation:** Load the `.hex` file into the LPC2138/LPC2148 component in **Proteus ISIS** and run the simulation.
7. **Hardware Execution:** Connect the LPC2148 board via USB-UART, set the ISP jumper, and flash the `.hex` file using **Flash Magic**.

---

## 7. Testing Results & Output Screenshots
| Test Scenario | RTC Time | Ambient Light | Street Light State | Observed LCD Output |
| :--- | :--- | :--- | :--- | :--- |
| Bright Daytime | `11:30:00` | `850 Lux` | **OFF** | `11:30:00` / `LUX:850 SL:OFF` |
| Daytime Shadow / Cloud | `14:15:00` | `25 Lux` | **OFF** (Blocked by RTC) | `14:15:00` / `LUX:25 SL:OFF` |
| Nighttime Darkness | `19:45:00` | `18 Lux` | **ON** (Time + Lux met) | `19:45:00` / `LUX:18 SL:ON` |
| Nighttime Bright Glare | `22:10:00` | `450 Lux` | **OFF** (Lux > 50) | `22:10:00` / `LUX:450 SL:OFF` |

*(Upload output screenshots and demo video links into `Results_and_Media/`)*

---

## 8. Challenges Faced and Solutions
* **ADC Reading Stuck / Saturated on Hardware Kit:** Resolved by verifying `PINSEL1` channel selection for `AD0.1` (`P0.28`), matching the voltage divider resistor to the kit's LDR impedance, and scaling integer Lux output cleanly to `0–930 Lux`.
* **LCD Screen Flicker on Continuous Updates:** Replaced repeated `0x01` clear-display commands inside the `while(1)` loop with direct DDRAM cursor positioning (`0x80` and `0xC0`) and space padding.
* **Keypad Blocking Sensor Updates:** Moved configuration entry to external interrupt `EINT0` so normal RTC and Lux monitoring runs uninterrupted until the technician presses the Edit button.

---

## 9. Future Enhancements
* **PWM Traffic-Aware Dimming:** Dim street lights to `30%` duty cycle during late-night low-traffic hours and ramp to `100%` upon PIR/IR motion detection.
* **IoT Cloud Telemetry:** Interface an ESP8266/ESP32 over `UART1` to log energy savings and send automated fault alerts when a lamp fails.
* **Solar & Battery Management:** Add solar PV charging with battery voltage monitoring via `ADC0.2` for standalone off-grid deployment.,
