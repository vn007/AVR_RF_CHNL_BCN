#ifndef LED_H_INCLUDED
#define LED_H_INCLUDED

#define LED_PORT        PORTB
#define LED_DDR         DDRB
#define LED_PIN         0

#include "stdint.h"
#include "avr/io.h"


void LED_init();
void LED_Blink_Hex(uint8_t n);
void LED_Blink_n(uint8_t N);
void LEDS_Blink_Once(void);
void LEDS_Blink_Once_Short(void);
void LED_On(void);
void LED_Off(void);

#endif // LED_H_INCLUDED
