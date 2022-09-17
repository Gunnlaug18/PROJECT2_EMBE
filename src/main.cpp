/*
 * main.c
 *
 *  Created on: 31. jan. 2022
 *      Author: Bjarki
 */

// #include "delay.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <digital_out.h>
#include <digital_in.h>
#include <encoder.h>


Digital_in encoder_c1(DDD2);
Digital_in encoder_c2(DDD3);
Digital_out motor_m1(DDD4);
Digital_out motor_m2(DDD5);
Encoder encoder(1000);
int counter = 0;

char TxBuffer[32];
int indx, len;

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
	if (b > 0){
		encoder.increment();
		encoder.speed_increment();
	}else{
		encoder.decrement();
		encoder.speed_decrement();
	}
	// counter +=1;
}


ISR(TIMER1_COMPA_vect)
{
	encoder.set_speed();
}


int main(){
	// Init_Uart();
	Init_Uart();
	encoder.timer_msec(1000);
	encoder_c1.init();
	encoder_c2.init();
	motor_m1.init();
	motor_m2.init();
	
	motor_m1.set_hi();
	motor_m2.set_lo();
	// // PORTD |= 1<<DDD4;
	// PORTD &= ~(1<<DDD5);
	while (1){
		// if (counter % 2 == 0){
		// 	motor_m1.set_hi();
		// 	motor_m2.set_lo();
		// }else{
		// 	motor_m1.set_lo();
		// 	motor_m2.set_lo();
		// }


		// _delay_ms(1); // delay 1 ms
		// counter +=1;

		// // UART_load_charVal_in_TxBuffer(encoder.speed()); // load encoder value to transmit
		// // UART_transmit_TxBuffer(); // transmit encoder value over UART
		// // reset_TxBuffer(); // reset transmit buffer




		// if (encoder.speed() >= 1300*0.63){
		// 	UART_load_charVal_in_TxBuffer(counter); // load encoder value to transmit
		// 	UART_transmit_TxBuffer(); // transmit encoder value over UART
		// 	reset_TxBuffer(); // reset transmit buffer
			
		// 	motor_m1.set_lo();
		// 	motor_m2.set_lo();
		// 	_delay_ms(1000);
		// 	counter = 0;

		// }else if (counter ==0 || counter ==1)
		// {
		// 	motor_m1.set_hi();
		// 	motor_m2.set_lo();
		// }



		
		_delay_ms(1); // delay 1 ms
		UART_load_charVal_in_TxBuffer(encoder.speed()); // load encoder value to transmit
		UART_transmit_TxBuffer(); // transmit encoder value over UART
		reset_TxBuffer(); // reset transmit buffer

	}

	return 0;
}
