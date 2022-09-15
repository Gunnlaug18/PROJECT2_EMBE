/*
 * main.c
 *
 *  Created on: 31. jan. 2022
 *      Author: Bjarki
 */

#include <avr/io.h>
#include <avr/interrupt.h>
// #include "delay.h"
#include "util/delay.h"
#define DES_LOC 350

char TxBuffer[32];
int indx, len;
int encoder = 0;

void Init_Uart()
{
	// 57600 baudrate
	UBRR0H = 0;
	UBRR0L = 16;
	UCSR0B = (1<<RXEN0) |(1<<TXEN0);// |(1<<TXCIE0);
	UCSR0C = (1<<USBS0) |(3<<UCSZ00);
	indx = len = 0;
}

void reset_TxBuffer(){
	indx = len = 0;
}

void UART_transmit_TxBuffer(){
	while (indx < len){
		while(!(UCSR0A & (1<<UDRE0))){;}
		UDR0 = TxBuffer[indx];
		indx++;
	}
}


int strlen(char s[])
{
	int i=0;
	while ( s[i] != 0 )
	i++;
	return i;
}

void reverse(char s[])
{
	int i,j;
	char c;

	for (i=0, j=strlen(s)-1; i<j; i++, j--){
	c = s[i];
	s[i] = s[j];
	s[j] = c;
	}
}

void UART_itoa(int n, char s[])
{
	int i,sign;

	if ( (sign = n ) < 0 )
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ( ( n /= 10 ) > 0 );
	if (sign < 0 )
	s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void UART_load_string_in_TxBuffer(char s[])
{
	while(s[len] != 0)
	{
		TxBuffer[len] = s[len];
		len++;
	}

	TxBuffer[len] = '\r';
	TxBuffer[len+1] = '\n';
	len += 2;
}

void UART_load_charVal_in_TxBuffer(int data)
{
	char temp[32];
	UART_itoa(data,temp);
	UART_load_string_in_TxBuffer(temp);
}


ISR(INT1_vect)
{
	int b = PIND & (1 << PIND2);
	if (encoder >= 700 || encoder <= -700) encoder = 0;
	if (b > 0){
		encoder++;
	}else{
		encoder--;
	}
}


int main(){
	Init_Uart();

	DDRD &= (1<<DDD3); // set pin D3 as input
	DDRD &= (1<<DDD2); // set pin D2 as input

	PORTD |= (1<<DDD3); // set PD3 pullup enabled
	EIMSK |= (1<<INT1); // external interrupt mask register for INT1 interrupt vector
	EICRA |= (1<<ISC10) | (1<<ISC11); // falling + rising edge interrupt requests

	asm("sei"); // enable interrupts

	while (1){
		while ((encoder < DES_LOC  || encoder > DES_LOC ))
		{

			while (encoder < DES_LOC){ // forwards (encoder++)
				PORTD |= 1<<DDD4;
				PORTD &= ~(1<<DDD5);
			}
			PORTD &= ~(1<<DDD4);
			PORTD &= ~(1<<DDD5);

			while (encoder > DES_LOC){ // backwards (encoder--)
				PORTD &= ~(1<<DDD4);
				PORTD |= 1<<DDD5;
			}
			PORTD &= ~(1<<DDD4);
			PORTD &= ~(1<<DDD5);

		}
		_delay_ms(1); // delay 1 ms
		UART_load_charVal_in_TxBuffer(encoder); // load encoder value to transmit
		UART_transmit_TxBuffer(); // transmit encoder value over UART
		reset_TxBuffer(); // reset transmit buffer
	}

	return 0;
}
