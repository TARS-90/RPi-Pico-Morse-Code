#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"

#define ON   1
#define OFF  0
#define UART_ID uart0
#define BAUD_RATE 115200

#define LED_PIN	    22  //
#define UART_TX_PIN 0  // pins
#define	UART_RX_PIN 1 //

const uint16_t LONG_SIGNAL = 700;   // in miliseconds
const uint16_t SHORT_SIGNAL = 350; //
const uint8_t MORSE_CODE[] = {	
		0x42, 0xC1, 0xC5, 0x81, 0x00, 0xC4,
		0x83, 0xC0, 0x40, 0xCE, 0x85, 0xC2, 
		0x43, 0x41, 0x87, 0xC6, 0xCB, 0x82, 
		0x80, 0x01, 0x84, 0xC8, 0x86, 0xC9, 
		0xCD, 0xC3 
	};

/*
 * Ignoring letters cases in computing index for Morse Code array
*/
uint8_t compute_index(const char character) {
	return character >= 'a' ? character - 'a' : character - 'A';
}

/*
 * Morse code of a character:
 *	- first two bits (MSB) are information of how many signals are
 *	needed to display character, where:
 *		- 01 -> two signals
 *		- 10 -> three signals
 *		- 11 -> four signals
 *		
 *	- last four bits (LSB) are the signals, where 1 is long signal
 *	and 0 is short signal, but in reversed direction
 *
 *	!!!bits that are between are ignored!!!
 * 
 *
 * Example: 
 *	character ->		A
 *	in real morse code ->	.- 
 *	in my morse code ->	01xxxx10 (x - ignored bits)
*/
void ASCII_to_morse(const char character) {
	uint8_t byte_code = MORSE_CODE[compute_index(character)];
	uint8_t signal_number = (byte_code >> 6) + 1; // shifting six bits to right

	while (signal_number--) {
		// checking if last bit in morse code byte equals 1
		if (byte_code % 2) {
			gpio_put(LED_PIN, ON);
			sleep_ms(LONG_SIGNAL);
		}
		else {
			gpio_put(LED_PIN, ON);
			sleep_ms(SHORT_SIGNAL);
		}

		gpio_put(LED_PIN, OFF);
		sleep_ms(200);
		byte_code >>= 1; // shifting byte code to right by one bit	
	}
}


int main() {
	stdio_init_all();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
    
	char string[] = "SOS";
	for (int i = 0; i < 3; i++) {
		ASCII_to_morse(string[i]);
	}

	sleep_ms(1000);

	return 0;
}
