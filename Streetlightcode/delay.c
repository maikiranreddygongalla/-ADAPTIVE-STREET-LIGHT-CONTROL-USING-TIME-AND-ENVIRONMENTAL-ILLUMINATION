#include "delay.h"
// Generates an approximate delay in microseconds (calibrated for 12MHz CCLK)
void delay_us(u32 dlyUS)
{
	for(dlyUS*=12;dlyUS>0;dlyUS--); // Multiply input by 12 and decrement in an empty loop
}

// Generates an approximate delay in milliseconds
void delay_ms(u32 dlyMS)
{
	for(dlyMS*=12000;dlyMS>0;dlyMS--); // Multiply input by 12,000 and decrement in an empty loop
}

// Generates an approximate delay in seconds
void delay_s(u32 dlyS)
{
	for(dlyS*=12000000;dlyS>0;dlyS--); // Multiply input by 12,000,000 and decrement in an empty loop
}
