#include "rnlib.h"

/* delay */
void RNLIB::delay(const uint16_t s) {
	RNLIB::initTimer0(PRESC01024, NORMAL, NORMAL, 0x7D);
	for (uint16_t i=0; i<s; i++) {
		for (uint8_t i=0; i<0x7D; i++) {
			while (TCNT0 != OCR0) {}
				TCNT0 = 0x00;
		}
	}
}

void RNLIB::delay_ms(const uint16_t ms) {
	RNLIB::initTimer0(PRESC064, NORMAL, NORMAL, 0xFA);
	for (uint16_t i=0; i<ms; i++) {
		while (TCNT0 != OCR0) {}
			TCNT0 = 0x00;
	}
}

void RNLIB::delay_us(uint16_t us) {
	for (uint16_t i=0; i<us; i++) {
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
	}
}

/* pin/port configurations */
void RNLIB::pinMode(volatile uint8_t& port, const uint8_t mode, const uint8_t pin) {
	if (pin >= 0x08)
		port = mode;
	else if (mode == INPUT)
		CLEARBIT(port, pin);
	else if (mode == OUTPUT)
		SETBIT(port, pin);
}

void RNLIB::portWrite(volatile uint8_t& port, const uint8_t value) {
	port = value;
}

uint8_t RNLIB::portRead(volatile uint8_t& port) {
	return port;
}

void RNLIB::pinWrite(volatile uint8_t& port, const uint8_t pin, const uint8_t value) {
	if (value > 0x00)
		SETBIT(port, pin);
	else
		CLEARBIT(port, pin);
}

uint8_t RNLIB::pinRead(volatile uint8_t& port, const uint8_t pin) {
	return CHECKBIT(port, pin);
}


/* USART */
FILE RNLIB::usartStream = FDEV_SETUP_STREAM(RNLIB::usartPutC, RNLIB::usartGetC, _FDEV_SETUP_RW) ;

void RNLIB::initUSART(uint32_t baudrate, const uint8_t characters, const uint8_t stoppbits, const uint8_t parity) {

	/* set baudrate
		*/
	baudrate = round((F_CPU/(baudrate*16.0))-1.0);

	UBRRH = (uint8_t)(baudrate>>0x08);
	UBRRL = (uint8_t)(baudrate);

	/* enable receiver and transmitter
		*/
	UCSRB = (UCSRB & 0b11100111) | (0x01<<RXEN) | (0x01<<TXEN);

	/* set frame format
		*/
	UCSRC = (UCSRC & 0b01000001) | (0x01<<URSEL) | (parity<<UPM0) | ((stoppbits-0x01)<<USBS) | ((MAX(characters, 0x08)-0x05)<<UCSZ0);

	/* set standard input/output
		*/
	stdin = &RNLIB::usartStream;
	stdout = &RNLIB::usartStream;
}

int RNLIB::usartPutC(char c, FILE* file) {
	if (file != &RNLIB::usartStream)
		return EOF;

	/* wait for empty transmit buffer */
	while (!CHECKBIT(UCSRA, UDRE)) {}
	UDR = c;
	return UDR;
}

int RNLIB::usartGetC(FILE* file) {
	if (file != &RNLIB::usartStream)
		return EOF;

	while (!CHECKBIT(UCSRA, RXC)) {}
	return UDR;
}


/* I2C */
// void initI2C() {}

// void sendI2C(const uint8_t data) {}


/* ADC */
void RNLIB::initADC(const uint8_t refVoltage, const bool& interrupt, const uint8_t mode) {
	/* set prescaler to 128 
		annot.: adc frequency should be between 50-200kHz,
		so at 16MHz the prescaler must be between 80-320,
		so it must be set to 128
		*/
	ADCSRA = (ADCSRA & 0b11111000) | (0x07<<ADPS0);

	/* set ref voltage
	   */
	ADMUX = (ADMUX & 0b00111111) | (refVoltage<<REFS0);

	/* check if adc should work in interrupt mode
		set global interrupts
		set mode
		set auto trigger
		set adc interrupt enable
		enable adc
		*/
	if (interrupt) {
		sei();
		SFIOR = (SFIOR & 0b00011111) | (mode<<ADTS0);
		SETBIT(ADCSRA, ADATE);
		SETBIT(ADCSRA, ADIE);
		SETBIT(ADCSRA, ADEN);
		if (mode == 0x00)
			SETBIT(ADCSRA, ADSC);
	}
	else {
		CLEARBIT(ADCSRA, ADEN);
		CLEARBIT(ADCSRA, ADIE);
		CLEARBIT(ADCSRA, ADATE);
	}
}

uint16_t RNLIB::adcGetSingle(const uint8_t channel, const uint8_t countMeas) {
	uint16_t value;

	/* set channel
		*/
	ADMUX = (ADMUX & 0b11100000) | (channel<<MUX0);

	/* activate ADC
		*/
	SETBIT(ADCSRA, ADEN);
	
	/* dummy -readout
		*/
	SETBIT(ADCSRA, ADSC);
	while (CHECKBIT(ADCSRA, ADSC)) {}
	value = ADCW;

	/* avg of countMeas measurements
		*/
	value = 0x00;
	for (uint8_t i=0x00; i<countMeas; i++) {
		SETBIT(ADCSRA, ADSC);
		while (CHECKBIT(ADCSRA, ADSC)) {}
		value += ADCW;
	}

	/* deactivate adc
		*/
	CLEARBIT(ADCSRA, ADEN);

	return (value /= countMeas);
}


/* TIMER */
void RNLIB::initTimer0(const uint8_t prescaler, const uint8_t mode, const uint8_t comp, const uint8_t value) {

	/* set compare value
		*/
	OCR0 = value;

	/* prescaler set
		*/
	TCCR0 |= (0b00000111) & (prescaler<<CS00);
	
	/* mode set
		*/
	TCCR0 |= (0b01001000) & (((mode>>0x01)<<WGM01) | (mode<<WGM00));
	
	/* compare set
		*/
	TCCR0 |= (0b00110000) & (comp<<COM00);

	/* enable interrupt
		*/
	sei();
	SETBIT(TIMSK, OCIE0);
}

void RNLIB::stopTimer0() {
	TCCR0 &= ~(0x07<<CS00);
}

void RNLIB::resetTimer0() {
	TCNT0 = 0x00;	
}

void RNLIB::initTimer1A(const uint8_t prescaler, const uint8_t mode, const uint8_t comp, const uint16_t value) {

	/* set compare value
		*/
	OCR1A = value;

	/* prescaler set
		*/
	TCCR1B |= (0b00000111) & (prescaler<<CS10);
	
	/* mode set
		*/
	TCCR1A |= (0b00000011) & (mode<<WGM10);
	TCCR1B |= (0b00011000) & ((mode>>0x02)<<WGM12);
	
	/* compare set
		*/
	TCCR1A |= (0b11000000) & (comp<<COM1A0);

	/* enable interrupt
		*/
	sei();
	SETBIT(TIMSK, OCIE1A);
}

void RNLIB::stopTimer1A() {
	TCCR1A &= ~(0x07<<CS00);
}

void RNLIB::resetTimer1A() {
	TCNT1 = 0x00;	
}


/* EEPROM */
void RNLIB::eepromWrite(const uint16_t address, const uint8_t data) {
	/* temp store status reg
		*/
	uint8_t stateSREG = SREG;
	
	/* dissable interrupts
		*/
	cli();

	{
		/* wait for completion of prev write and memory enable
			*/
		while (CHECKBIT(EECR, EEWE)) {}
		while (CHECKBIT(SPMCR, SPMEN)) {}

		/* set eeprom address reg
			*/
		EEAR = address;

		/* set eeprom data reg
			*/
		EEDR = data;

		/* enable eeprom write
			*/
		SETBIT(EECR, EEMWE);

		/* write data to eeprom
		*/
		SETBIT(EECR, EEWE);
	}

	/* restore status reg
		*/
	SREG = stateSREG;
}

uint8_t RNLIB::eepromRead(const uint16_t address) {
	/* temp store status reg
		*/
	uint8_t stateSREG = SREG;
	uint8_t data;
	
	/* dissable interrupts
		*/
	cli();

	{
		/* wait for completion of prev write
			*/
		while (CHECKBIT(EECR, EEWE)) {}

		/* set eeprom address reg
			*/
		EEAR = address;

		/* enable eeprom read
			*/
		SETBIT(EECR, EERE);

		/* read eeprom
			*/
		data = EEDR;
	}

	/* restore status reg
		*/
	SREG = stateSREG;
	
	return data;
}

void RNLIB::eepromWrite(const uint16_t address, uint8_t* buffer, const uint8_t size) {
	for (uint16_t i=0; i<size; i++)
		eepromWrite(address+i, buffer[i]);
}

uint16_t RNLIB::eepromRead(const uint16_t address, uint8_t* buffer) {
	uint16_t i=-1;

	do {
		i++;
		buffer[i] = eepromRead(address+i);
	} 
	while (buffer[i] != 0x00);

	return i;
}


/* buttons */
uint8_t RNLIB::getButton() {
	double REFVOLT;
	if (CHECKBIT(ADMUX, REFS1) && CHECKBIT(ADMUX, REFS0))
		REFVOLT = IREFVOLT;
	else
		REFVOLT = AREFVOLT;

	double oldValue = 0.00;
	double value = 1.00;
	while (oldValue != value) {
		oldValue = RNLIB::adcGetSingle(0x07, 0x01);
		value = RNLIB::adcGetSingle(0x07, 0x01);
	}

	value *= REFVOLT/1023.0;
	uint8_t button = 0x00;

	if (value>1.54 && value<=1.75)
		button = 0x01;
	if (value>1.38 && value<=1.54)
		button = 0x03;
	if (value>1.24 && value<=1.38)
		button = 0x02;
	if (value>1.11 && value<=1.24)
		button = 0x05;
	if (value>1.04 && value<=1.11)
		button = 0x06;
	if (value>0.91 && value<=1.04)
		button = 0x04;
	if (value>0.80 && value<=0.91)
		button = 0x09;
	if (value>0.74 && value<=0.80)
		button = 0x0A;
	if (value>0.68 && value<=0.74)
		button = 0x0C;
	if (value>0.55 && value<=0.68)
		button = 0x08;
	if (value>0.43 && value<=0.55)
		button = 0x11;
	if (value>0.40 && value<=0.43)
		button = 0x12;
	if (value>0.36 && value<=0.40)
		button = 0x14;
	if (value>0.34 && value<=0.36)
		button = 0x18;
	if (value>0.30 && value<=0.34)
		button = 0x10;

	return button;
}


/* vdd */
double RNLIB::getVdd() {
	double REFVOLT;
	if (CHECKBIT(ADMUX, REFS1) && CHECKBIT(ADMUX, REFS0))
		REFVOLT = IREFVOLT;
	else
		REFVOLT = AREFVOLT;

	return (RNLIB::adcGetSingle(0x06, 0x01) * REFVOLT/1023.0 * VDDREF);;
}

/* led */
void RNLIB::setLed(const uint8_t led) {
	RNLIB::pinMode(DDRC, OUTPUT, led);
	RNLIB::pinWrite(PORTC, led, LOW);
}

void RNLIB::clearLed(const uint8_t led) {
	RNLIB::pinMode(DDRC, OUTPUT, led);
	RNLIB::pinWrite(PORTC, led, HIGH);
}

void RNLIB::toggleLed(const uint8_t led) {
	RNLIB::pinMode(DDRC, OUTPUT, led);
	TOGGLEBIT(PORTC, led);
}


/* speaker */
void RNLIB::tone(const double& frequency, const uint8_t length) {
	uint8_t pinState = RNLIB::pinRead(DDRD, PD7);
	RNLIB::pinMode(DDRD, OUTPUT, PD7);

	double delay = 1000000.0 / (2.0 * frequency);
	for (uint32_t i=0; i<frequency*length/(1.0*0x20); i++) {
		TOGGLEBIT(PORTD, PD7);
		RNLIB::delay_us(delay);
	}

	RNLIB::pinMode(DDRD, pinState, PD7);
}

void RNLIB::beep() {
	tone(3520.0, 0x9);
}

void RNLIB::doubleBeep() {
	RNLIB::beep();
	RNLIB::delay_ms(10);
	RNLIB::beep();
}

/* Display */
void RNLIB::print7Seg(volatile uint8_t& port, const char& num) {
	RNLIB::portWrite(port, HIGH);

	switch (num) {
		case 0x00: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 6, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x01: {
			RNLIB::portWrite(port, HIGH);
			RNLIB::pinWrite(port, 1, LOW);
			RNLIB::pinWrite(port, 2, LOW);
			break;
		}
		case 0x02: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 2, HIGH);
			RNLIB::pinWrite(port, 5, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x03: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 4, HIGH);
			RNLIB::pinWrite(port, 5, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x04: {
			RNLIB::portWrite(port, HIGH);
			RNLIB::pinWrite(port, 1, LOW);
			RNLIB::pinWrite(port, 2, LOW);
			RNLIB::pinWrite(port, 5, LOW);
			RNLIB::pinWrite(port, 6, LOW);
			break;
		}
		case 0x05: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 1, HIGH);
			RNLIB::pinWrite(port, 4, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x06: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 1, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x07: {
			RNLIB::portWrite(port, HIGH);
			RNLIB::pinWrite(port, 0, LOW);
			RNLIB::pinWrite(port, 1, LOW);
			RNLIB::pinWrite(port, 2, LOW);
			break;
		}
		case 0x08: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x09: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 4, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x0A: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 3, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x0B: {
			RNLIB::portWrite(port, LOW);
			break;
		}
		case 0x0C: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 1, HIGH);
			RNLIB::pinWrite(port, 2, HIGH);
			RNLIB::pinWrite(port, 6, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x0D: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 6, HIGH);
			break;
		}
		case 0x0E: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 1, HIGH);
			RNLIB::pinWrite(port, 2, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
		case 0x0F: {
			RNLIB::portWrite(port, LOW);
			RNLIB::pinWrite(port, 1, HIGH);
			RNLIB::pinWrite(port, 2, HIGH);
			RNLIB::pinWrite(port, 3, HIGH);
			RNLIB::pinWrite(port, 7, HIGH);
			break;
		}
	}
}


/* Numpad */
void RNLIB::initPad(volatile uint8_t ddr, volatile uint8_t& port) {
	const uint8_t NUM = 0x04;

	uint8_t rowPins[NUM] = {0x07, 0x06, 0x05, 0x04};
	uint8_t colPins[NUM] = {0x03, 0x02, 0x01, 0x00};

	for (uint8_t i=0x00; i<NUM; i++) {
		RNLIB::pinMode(ddr, INPUT, rowPins[i]);
		RNLIB::pinWrite(port, rowPins[i], HIGH);
		RNLIB::pinMode(ddr, OUTPUT, colPins[i]);
		RNLIB::pinWrite(port, colPins[i], HIGH);
	}
}

uint8_t RNLIB::readPad(volatile uint8_t& port, volatile uint8_t pin) {
	uint8_t result = 0x00;
	const uint8_t ROWS = 0x04;
	const uint8_t COLS = 0x04;

	const char keys[ROWS][COLS] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'},
	};

	uint8_t rowPins[ROWS] = {0x07, 0x06, 0x05, 0x04};
	uint8_t colPins[COLS] = {0x03, 0x02, 0x01, 0x00};

	for (uint8_t c=0x00; c<COLS; c++) {
		RNLIB::pinWrite(port, colPins[c], LOW);
		for (uint8_t r=0x00; r<ROWS; r++) {
			if(!RNLIB::pinRead(pin, rowPins[r])) {
				RNLIB::pinWrite(port, colPins[c], HIGH);
				result = keys[r][c];
				break;
			}
		}
		RNLIB::pinWrite(port, colPins[c], HIGH);
	}

	return result;
}

/* time */
void RNLIB::printTime(const uint32_t time) {
	uint8_t hh = time / 3600000;
	uint8_t mm = (time - hh * 3600000) / 60000;
	uint8_t ss = (time - hh * 3600000 - mm * 60000) / 1000;
	uint16_t ms = time - hh * 3600000 - mm * 60000 - ss * 1000;

	printf("%02u:%02u:%02u:%03u%s", hh, mm, ss, ms, CRLF);
}
