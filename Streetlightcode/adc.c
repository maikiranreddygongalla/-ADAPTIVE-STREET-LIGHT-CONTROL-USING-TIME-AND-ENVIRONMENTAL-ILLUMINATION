#include <LPC214x.h>         // Include LPC214x microcontroller register definitions
#include "lcd.h"             // Include LCD display function prototypes
#include "lcd_definesh.h"    // Include LCD command and hardware pin definitions
#include "kpm.h"             // Include keypad function prototypes
#include "delay.h"           // Include delay function prototypes
#include "adc.h"             // Include ADC function prototypes

#define VREF        3.3f      // LPC2148 ADC Reference Voltage (3.3V)
#define R_FIXED     10000.0f  // Fixed resistor value in Ohms (10k Ohm)
#define LUX_CONST   500000.0f // Calibration constant for standard LDR
#define MAX_LUX     930.0f    // Upper limit for Lux reading

// Select your hardware ADC channel:
// 0 -> P0.27 (AD0.0)
// 1 -> P0.28 (AD0.1)
// 2 -> P0.29 (AD0.2)
// 3 -> P0.30 (AD0.3)
#define LDR_CH      1

// Function to initialize ADC0 and configure the selected analog pin
void init_adc(u32 ch) {
    u32 pin_shift = ((27 + LDR_CH) - 16) * 2;
    PINSEL1 &= ~(3 << pin_shift);       // Clear PINSEL1 bits for selected pin
    PINSEL1 |=  (1 << pin_shift);       // Set pin as AD0.x function (01)

    AD0CR = (1 << LDR_CH) |             // Select ADC channel
            (13 << 8)     |             // CLKDIV = 13 -> ADC clock <= 4.5MHz
            (1 << 21);                  // PDN = 1 (Enable ADC)
}

// Function to read ADC, convert to float Lux (1.0 to 930.0), and display on LCD
f32 adc_lux(void) {
    u32 res = 0;
    f32 v, r_ldr, lux;

    // Ensure channel is selected and start ADC conversion immediately
    AD0CR &= ~0xFF;
    AD0CR |= (1 << LDR_CH) | (1 << 24);

    // Wait on channel-specific Data Register DONE bit (bit 31)
    switch (LDR_CH) {
        case 0: while (((AD0DR0 >> 31) & 1) == 0); res = (AD0DR0 >> 6) & 0x3FF; break;
        case 1: while (((AD0DR1 >> 31) & 1) == 0); res = (AD0DR1 >> 6) & 0x3FF; break;
        case 2: while (((AD0DR2 >> 31) & 1) == 0); res = (AD0DR2 >> 6) & 0x3FF; break;
        case 3: while (((AD0DR3 >> 31) & 1) == 0); res = (AD0DR3 >> 6) & 0x3FF; break;
    }
    AD0CR &= ~(1 << 24);                // Clear START bit

    // 1. Convert digital value (0-1023) to analog voltage
    v = (res * VREF) / 1023.0f;

    // Clamp voltage to prevent division by zero
    if (v <= 0.01f) v = 0.01f;
    if (v >= 3.29f) v = 3.29f;

    // 2. Calculate LDR Resistance
    // For standard 3-pin/4-pin LDR sensor modules (LDR connected to GND, 10k to 3.3V):
    r_ldr = R_FIXED * (v / (VREF - v));
    // Note: If Lux goes DOWN when shining a torch on LDR, use this line instead:
    // r_ldr = R_FIXED * ((VREF - v) / v);

    // 3. Convert Resistance to Lux in float
    lux = LUX_CONST / r_ldr;

    // 4. Enforce valid float range (1.0 to 930.0 Lux)
    if (lux < 1.0f) {
        lux = 1.0f;
    } else if (lux > MAX_LUX) {
        lux = MAX_LUX;
    }

    // 5. Display raw ADC count (A:xxx) and Lux on Line 2 of LCD
    CmdLCD(GOTO_LINE2_POS0 + 7);
    StrLCD("LDR");
   // U32LCD(res);
    //StrLCD(" ");
    //U32LCD((unsigned int)lux);
    //StrLCD("  ");
		F32LCD(lux,2);

    return lux;
}
