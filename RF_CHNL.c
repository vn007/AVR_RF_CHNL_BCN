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

#include "RF_CHNL.h"
#include "uart.h"

nRF24L01 *setup_rf(void);
void process_message(char *message);

volatile bool rf_interrupt = false;

int main(void) {

	init_UART(9600);
	_delay_ms(100);

uint8_t address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };
//sei();
cli();
nRF24L01 *rf = setup_rf();

//nRF24L01_listen(rf, 0, address);
uint8_t addr[5];
//nRF24L01_read_register(rf, CONFIG, addr, 1);
//send_Uart(nRF24L01_read_register(rf, STATUS, addr, 1));

for (int i=0; i<16; i++){
	nRF24L01_read_register(rf, i, addr, 1);
	send_Uart(addr[0]);
}

//send_Uart(addr[0]);

STOP

while (true) {
	if (rf_interrupt) {
		rf_interrupt = false;
		while (nRF24L01_data_received(rf)) {
			nRF24L01Message msg;
			nRF24L01_read_received_data(rf, &msg);
			process_message((char *)msg.data);
		}

		nRF24L01_listen(rf, 0, address);
	}
}

return 0;
}

nRF24L01 *setup_rf(void) {
    nRF24L01 *rf = nRF24L01_init();
    rf->ss.port = &PORTB;
    rf->ss.pin = PB2;
    rf->ce.port = &PORTB;
    rf->ce.pin = PB1;
    rf->sck.port = &PORTB;
    rf->sck.pin = PB5;
    rf->mosi.port = &PORTB;
    rf->mosi.pin = PB3;
    rf->miso.port = &PORTB;
    rf->miso.pin = PB4;
    // interrupt on falling edge of INT0 (PD2)
    EICRA |= _BV(ISC01);
    EIMSK |= _BV(INT0);
    nRF24L01_begin(rf);
    return rf;
}

void process_message(char *message) {
    if (strcmp(message, "ON") == 0)
	;
    else if (strcmp(message, "OFF") == 0)
	;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
    rf_interrupt = true;
}
