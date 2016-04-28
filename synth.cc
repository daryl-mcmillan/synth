#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint8_t byte;

long notes[] = {
  654,
  693,
  734,
  778,
  824,
  873,
  925,
  980
};

const byte SINE[] {
  128, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 162, 165, 168, 171, 174,
  176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216,
  218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 239, 240, 242, 243, 245,
  246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 255, 255, 255, 255, 255,
  255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 251, 250, 249, 248, 247,
  246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 228, 226, 224, 222, 220,
  218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179,
  176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 143, 140, 137, 134, 131,
  128, 124, 121, 118, 115, 112, 109, 106, 103,  99,  96,  93,  90,  87,  84,  81,
   79,  76,  73,  70,  67,  64,  62,  59,  56,  54,  51,  49,  46,  44,  42,  39,
   37,  35,  33,  31,  29,  27,  25,  23,  21,  19,  18,  16,  15,  13,  12,  10,
    9,   8,   7,   6,   5,   4,   3,   3,   2,   1,   1,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   1,   1,   2,   3,   3,   4,   5,   6,   7,   8,
    9,  10,  12,  13,  15,  16,  18,  19,  21,  23,  25,  27,  29,  31,  33,  35,
   37,  39,  42,  44,  46,  49,  51,  54,  56,  59,  62,  64,  67,  70,  73,  76,
   79,  81,  84,  87,  90,  93,  96,  99, 103, 106, 109, 112, 115, 118, 121, 124,
};

byte sine(byte t) {
  return SINE[t];
}

byte ramp(byte t) {
  return t;
}

byte saw(byte t) {
  return 255-t;
}

void pulseB( int pin ) {
  PORTB |= (1 << pin);
  PORTB &= ~(1 << pin);
}

void pulseD( int pin ) {
  PORTD |= (1 << pin);
  PORTD &= ~(1 << pin);
}

void pulse0() {
  pulseD(2);
}
void level0(byte level) {
  OCR2B = level;
}

void pulse1() {
  pulseD(4);
}
void level1(byte level) {
  OCR0B = level;
}

void pulse2() {
  pulseD(7);
}
void level2(byte level) {
  OCR0A = level;
}

void pulse3() {
  pulseB(2);
}
void level3(byte level) {
  OCR2A = level;
}

long next0 = 0;
long next1 = 0;
long next2 = 0;
long next3 = 0;

uint16_t interval0 = 160000000l / 654 / 256;
uint16_t interval1 = 160000000l / 660 / 256;
uint16_t interval2 = 160000000l / 670 / 256;
uint16_t interval3 = 160000000l / 2200 / 256;

volatile unsigned long tone0time = 0;
volatile unsigned long tone1time = 0;
volatile unsigned long tone2time = 0;
volatile unsigned long tone3time = 0;

volatile int scanCounter = 0;

ISR(TIMER1_COMPA_vect) {
  tone1time = (tone1time + 0x80 ) % 256;
  //OCR0A = SINE[tone1time] >> 1;

  tone2time = (tone2time + 1) % 5120;
  //OCR0B = SINE[tone2time * 256 / 5120] >> 2;
  level0( ( tone1time & 0x80 ) >> 1 );
  level1( ( tone1time & 0x80 ) >> 1 );
  level2( ( tone1time & 0x80 ) >> 1 );
  level3( ( tone1time & 0x80 ) ? 0x00 : 0x99 );

  tone3time = (tone3time + 1) % 1024;
  //OCR2B = SINE[tone3time * 256 / 1024] >> 1;
  //OCR2B = 64;

  //interval4 = 160000000 / notes[1];
  //interval2 = 160000000 / (notes[4] + 1);

  scanCounter ++;
  if( scanCounter == 10 ) {
    scanCounter = 0;
    pulseB(0);
  }
}

int main(void) {

  sei();

  // port D io direction
  DDRD = 0b11111100;

  // port B io direction
  DDRB = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 5);

  // port C io direction
  DDRC = 0;
  PORTC = 0;

  PORTB = 0;
  PORTD = 0;

  next0 = interval0;
  next1 = interval1;
  next2 = interval2;
  next3 = interval3;

  // timer0
  TIMSK0 = 0b000;
  TIFR0 = 0b000;
  byte prescale0 = 0b001;
  TCCR0B = 0b00000000 | prescale0;
  TCCR0A = 0b10100001; // pwm A, pwm B, phase correct

  // timer2
  TIMSK2 = 0b000;
  TIFR2 = 0b000;
  byte prescale2 = 0b001;
  TCCR2B = 0b00000000 | prescale2;
  TCCR2A = 0b10100001; // pwm A, pwm B, phase correct

  // timer1
  TIMSK1 = 0b010;
  TIFR1 = 0b000;
  byte prescale1 = 0b0100;
  byte countToA = 0b1000;
  byte countToMax = 0b0000;
  OCR1A = 16000;
  TCCR1B = 0b00000000 | prescale1 | countToMax;
  TCCR1A = 0b00000000; // no output, no output, fast

  uint16_t lastticks = TCNT1;

  while (1) {
    uint16_t ticks = TCNT1;
    uint16_t elapsed;
    elapsed = ticks - lastticks;
    lastticks = ticks;
    if( next0 < elapsed ) {
      pulse0();
      next0 += interval0;
    }
    next0 -= elapsed;
    if( next1 < elapsed ) {
      pulse1();
      next1 += interval1;
    }
    next1 -= elapsed;
    if( next2 < elapsed ) {
      pulse2();
      next2 += interval2;
    }
    next2 -= elapsed;
    if( next3 < elapsed ) {
      pulse3();
      next3 += interval3;
    }
    next3 -= elapsed;
  }

  return 0;

}
