/*
 * RF_CHNL
 .c
 *
 * Created: 19.11.2016 10:01:09
 *  Author: LV
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include <util/delay.h>
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"

#include "RF_CHNL_BCN.h"
#include "uart.h"
#include "LED.h"

void setup_timer(void);
//nRF24L01 *setup_rf(void);
//nRF24L01 *rf_init(uint8_t rf_channel);

volatile bool rf_interrupt = false;
volatile bool send_message = false;

nRF24L01 *rf;

int main(void) {

    _delay_ms(250);
	LED_init();
//	LED_On();

//	init_UART(9600);

	nRF24L01 __rf = {
		{SS_PORT, SS_PIN},
		{CE_PORT, CE_PIN},
		{SCK_PORT, SCK_PIN},
		{MOSI_PORT, MOSI_PIN},
		{MISO_PORT, MISO_PIN},
		Primary_RX,
		0,
		RF_CHANNEL_NO,
		{AddrP0_B0, AddrP0_B1, AddrP0_B2, AddrP0_B3, AddrP0_B4},
		{AddrP1_B0, AddrP1_B1, AddrP1_B2, AddrP1_B3, AddrP1_B4},
		AddrP2_B0,
		AddrP3_B0,
		AddrP4_B0,
		AddrP5_B0
	};

	rf = &__rf;

	nRF24L01_init(rf);

    nRF24L01_activate_pipe(rf, 0);     // pipe = 0

    sei();

    rf_interrupt = false;
    uint8_t to_address[5] = { 0xA0, 0xA1, 0xA2, 0xA3, 0xA4 };
    bool on = false;

    setup_timer();

//STOP
//_delay_ms(250);

    while (true) {
        if (rf_interrupt) {
            rf_interrupt = false;
            int tx_result = nRF24L01_transmit_success(rf);
            if (tx_result != 0) {
                nRF24L01_flush_transmit_message(rf);
                _delay_ms(250);
				LEDS_Blink_Once_Short();
				LEDS_Blink_Once_Short();
//                LED_Blink_Hex(tx_result);
//                STOP
            }
            else
				LED_Blink_n(1);
        }

        if (send_message) {
            send_message = false;
LEDS_Blink_Once_Short();
            on = !on;
            nRF24L01Message msg;
            if (on) {
                memcpy(msg.data, "ON", 3);
            }
            else memcpy(msg.data, "OFF", 4);
            msg.length = strlen((char *)msg.data) + 1;
            nRF24L01_transmit(rf, to_address, &msg);
        }
    }

    return 0;
}

// setup timer to trigger interrupt every second when at 1MHz
void setup_timer(void) {
    TCCR1B |= _BV(WGM12);   // Compare match (CTC) mode
    TIMSK1 |= _BV(OCIE1A);  // Timer/Counter1, Output Compare A Match Interrupt Enable
    OCR1A = 15624;
//    TCCR1B |= _BV(CS10) | _BV(CS11);    //clk/64
    TCCR1B |= _BV(CS10) | _BV(CS12);    //clk/1024
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
   send_message = true;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
    rf_interrupt = true;
}
