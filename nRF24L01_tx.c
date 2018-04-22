#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <string.h>
#include "nrf24l01.h"

#include "RF_CHNL_BCN.h"
#include "LED.h"

void setup_timer(void);
//nRF24L01 *setup_rf(void);

//volatile bool rf_interrupt = false;
//volatile bool send_message = false;

int nRF24L01_beacon(void) {
    uint8_t to_address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };
    uint8_t rf_channel = 73;
    bool on = false;
    sei();
    nRF24L01 *rf = rf_init(rf_channel);
    setup_timer();

    while (true) {
        if (rf_interrupt) {
            rf_interrupt = false;
            int success = nRF24L01_transmit_success(rf);
            if (success != 0)
                nRF24L01_flush_transmit_message(rf);
LEDS_Blink_Once_Short();
        }

        if (send_message) {
            send_message = false;
            on = !on;
            nRF24L01Message msg;
            if (on) memcpy(msg.data, "ON", 3);
            else memcpy(msg.data, "OFF", 4);
            msg.length = strlen((char *)msg.data) + 1;
            nRF24L01_transmit(rf, to_address, &msg);
        }
    }

    return 0;
}

//
// setup timer to trigger interrupt every second when at 1MHz
void setup_timer(void) {
    TCCR1B |= _BV(WGM12);               //Clear Timer on Compare match (CTC) mode
    TIMSK1 |= _BV(OCIE1A);              //Output Compare A Match Interrupt Enable
    OCR1A = 15624;
//    TCCR1B |= _BV(CS10) | _BV(CS11);    //CLK/64
    TCCR1B |= _BV(CS10) | _BV(CS12);      //CLK/1024
}

// each one second interrupt
ISR(TIMER1_COMPA_vect) {
 //           LEDS_Blink_Once();
   send_message = true;
}

// nRF24L01 interrupt
ISR(INT0_vect) {
    rf_interrupt = true;
//    LEDS_Blink_Once_Short();
//    LED_HexBlink(0x00);
}
