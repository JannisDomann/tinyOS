#ifndef RNLIB_H
#define RNLIB_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

namespace RNLIB {

	/* global */ 
	#define INPUT 0x00
	#define OUTPUT 0xFF
	#define HIGH 0xFF
	#define LOW 0x00
	#define CRLF "\r\n"

	/* ADC */
	#define VDDREF 27.1/5.1
	#define AREFVOLT 5.00
	#define IREFVOLT 2.56
	#define AREF 0x00
	#define AVCC 0x01
	#define IREF 0x03
	#define FREERUN 0x00
	#define ANALOGCOMP 0x01
	#define EXTINT 0x02
	#define C0COMPMTCH 0x03
	#define C0OVRFLW 0x04
	#define C1COMPMTCHB 0x05
	#define C1OVRFLW 0x06
	#define C1CPTEVT 0x07

	/* USART */
	#define BAUDRATE 115200
	#define PARNONE 0x00
	#define PAREVEN 0x02
	#define PARODD 0x03
	#define CHRBUFLEN 0x40

	/* TIMER */
	#define NORMAL 0x00
	#define PWM 0x01
	#define CTC0 0x02
	#define CTC1 0x04
	#define FPWM 0x03
	#define COMPTGL 0x01
	#define COMPCLR 0x02
	#define COMPSET 0x03
	#define TIMERDEACT 0x00
	#define PRESCOFF 0x00
	#define PRESCNO 0x01
	#define PRESC08 0x02
	#define PRESC064 0x03
	#define PRESC0256 0x04
	#define PRESC01024 0x05
	#define PRESC18 0x02
	#define PRESC164 0x03
	#define PRESC1256 0x04
	#define PRESC11024 0x05
	#define PRESC28 0x02
	#define PRESC232 0x03
	#define PRESC264 0x04
	#define PRESC2128 0x05
	#define PRESC2256 0x06
	#define PRESC21024 0x07
	#define EXTCLKFAL 0x06
	#define EXTCLKRIS 0x07

	/* macros */
	#define SETBIT(REG,BIT) REG |= (0x01<<BIT)
	#define CLEARBIT(REG,BIT) REG &= ~(0x01<<BIT)
	#define TOGGLEBIT(REG,BIT) (REG ^= (0x01<<BIT))
	#define TOGGLEPORT(REG) (REG ^= 0xFF)
	#define CHECKBIT(REG,BIT) (REG & (0x01<<BIT))
	#define MIN(A,B) (A<B?A:B)
	#define MAX(A,B) (A>B?A:B)
	

	/* delay */
	void delay(const uint16_t& s);
	void delay_ms(const uint16_t& ms);
	void delay_us(uint16_t us);

	/* pin/port configurations */
	void pinMode(volatile uint8_t& port, const uint8_t& mode, const uint8_t& pin = 0x08);
	void portWrite(volatile uint8_t& port, const uint8_t& value);
	uint8_t portRead(volatile uint8_t& port);
	void pinWrite(volatile uint8_t& port, const uint8_t& pin, const uint8_t& value);
	uint8_t pinRead(volatile uint8_t& port, const uint8_t& pin);

	/* USART */
	#undef FDEV_SETUP_STREAM
	#define FDEV_SETUP_STREAM(p, g, f) \
    {                              \
        .buf = NULL,               \
        .unget = 0,                \
        .flags = f,                \
        .size = 0,                 \
        .len = 0,                  \
        .put = p,                  \
        .get = g,                  \
        .udata = 0                 \
    }
	extern FILE usartStream;
	void initUSART(uint32_t baudrate, const uint8_t& characters, const uint8_t& stoppbits, const uint8_t& parity);
	int usartPutC(char c, FILE* file);
	int usartGetC(FILE* file);

	/* I2C */
	// void initI2C();
	// void sendI2C(const uint8_t& data);

	/* ADC */
	void initADC(const uint8_t& refVoltage, const bool& interrupt=false, const uint8_t& mode=0x00);
	uint16_t adcGetSingle(const uint8_t& channel, const uint8_t& countMeas);

	/* TIMER */
	void initTimer0(const uint8_t prescaler=PRESCNO, const uint8_t mode=NORMAL, const uint8_t comp=NORMAL, const uint8_t value=0x00);
	void stopTimer0();
	void resetTimer0();
	void initTimer1A(const uint8_t prescaler=PRESCNO, const uint8_t mode=NORMAL, const uint8_t comp=NORMAL, const uint16_t value=0x00);
	void stopTimer1A();
	void resetTimer1A();

	/* EEPROM */
	void eepromWrite(const uint16_t& address, const uint8_t data);
	void eepromWrite(const uint16_t& address, uint8_t* buffer, const uint8_t& size);
	uint8_t eepromRead(const uint16_t& address);
	uint16_t eepromRead(const uint16_t& address, uint8_t* buffer);

	/* buttons */
	uint8_t getButton();

	/* vdd */
	double getVdd();

	/* led */
	void setLed(const uint8_t& led);
	void clearLed(const uint8_t& led);
	void toggleLed(const uint8_t& led);

	/* speaker */
	void tone(const double& frequency=440.0, const uint8_t& length=0x60);
	void beep();
	void doubleBeep();

	/* Display */
	void print7Seg(volatile uint8_t& port, const char& num);

	/* Numpad */
	void initPad(volatile uint8_t& ddr, volatile uint8_t& port);
	uint8_t readPad(volatile uint8_t& port, volatile uint8_t& pin);

	/* time */
	void printTime(const uint32_t& time);
}

#endif // !RNLIB_H
