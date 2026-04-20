#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"
#include <string.h>

#define ON   1
#define OFF  0
#define UART_ID uart0
#define BAUD_RATE 115200

// pins
const uint8_t LED_PIN     = 22;
const uint8_t UART_TX_PIN = 0;
const uint8_t UART_RX_PIN = 1;

// in miliseconds
const uint16_t TIME_UNIT    = 150; 
const uint16_t SHORT_SIGNAL = TIME_UNIT; 
const uint16_t LONG_SIGNAL  = TIME_UNIT * 3;   
const uint16_t WORD_DELAY   = TIME_UNIT * 7;

// Morse Code tabe A-Z
const uint8_t MORSE_CODE[] = {	
		0x42, 0xC1, 0xC5, 0x81, 0x00, 0xC4,
		0x83, 0xC0, 0x40, 0xCE, 0x85, 0xC2, 
		0x43, 0x41, 0x87, 0xC6, 0xCB, 0x82, 
		0x80, 0x01, 0x84, 0xC8, 0x86, 0xC9, 
		0xCD, 0xC3 
	};


// Ignoring letters cases 
uint8_t compute_index(const uint8_t character) {
	return character >= 'a' ? character - 'a' : character - 'A';
}

/*
  Morse code of a character:
 	- first two bits (MSB) are information of how many signals are
 	needed to display character, where:
 		- 01 -> two signals
 		- 10 -> three signals
 		- 11 -> four signals
 		
 	- last four bits (LSB) are the signals, where 1 is long signal
 	and 0 is short signal, but in reversed direction
 
 	!!!bits that are between are ignored!!!
 
 
  Example: 
 	character ->		A
 	in real morse code ->	.- 
 	in my morse code ->	01xxxx10 (x - ignored bits)
*/
void ASCII_to_morse(const uint8_t character) {
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
		sleep_ms(TIME_UNIT);
		byte_code >>= 1; // shifting byte code to right by one bit	
	}
}


/*
	Creating a Queue (FIFO) for reciving words from UART
*/

typedef struct Node {
	struct Node* next;
	uint8_t*  word;
} Node;

typedef struct Queue {
	Node* head;	
	void  (*add)(uint8_t*);
	uint8_t* (*get)(void);
} Queue;

Queue queue = {0};

void add(uint8_t* word) {
	Node* node = malloc(sizeof(Node));
	if (!node) return;

	node->next = NULL;

	node->word = malloc(strlen(word) + 1);
	if (!node->word) {
		free(node);
		return;
	}
	strcpy(node->word, word);

	// if queue is not empty
	if (queue.head != NULL) {
		// find last node 
		Node* tmp = queue.head;
		while (tmp->next != NULL) 
			tmp = tmp->next;
		tmp->next = node; // and then make current node the last one
	}
	else queue.head = node;
}

uint8_t* get() {
	// if queue is not empty
	if (queue.head != NULL) {
		Node* tmp = queue.head;
		uint8_t* word = tmp->word;
		queue.head = tmp->next;
		free(tmp->word);
		free(tmp);

		return word;
	}
	// if queue is empty
	return NULL;
}

int main() {
	// Raspberry Pico initialization
	stdio_init_all();
	gpio_init(LED_PIN);
	gpio_set_dir(LED_PIN, GPIO_OUT);
	uart_init(UART_ID, BAUD_RATE);


  
	// Queue initialization
	queue.head = NULL;
	queue.add  = add;
	queue.get  = get;

	while (true) {

	}
	
	return 0;
}
