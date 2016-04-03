#include <avr/io.h>
#include <avr/interrupt.h>

typedef uint8_t byte;

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

void pulseD( int pin ) {
  PORTD |= (1 << pin);
  PORTD &= ~(1 << pin);
}

long next2 = 0;
long next4 = 0;
long next7 = 0;

long interval2 = 160000000 / 1648;
long interval4 = 160000000 / 1100;
long interval7 = 160000000 / 654;

volatile unsigned long tone1time = 0;
volatile unsigned long tone2time = 0;
volatile unsigned long tone3time = 0;

ISR(TIMER1_COMPA_vect) {
  tone1time = (tone1time + 1 ) % 256;
  OCR0A = SINE[tone1time] >> 1;

  tone2time = (tone2time + 1) % 10000;
  OCR0B = SINE[tone2time * 256 / 10000] >> 2;

  tone3time = (tone3time + 1) % 15000;
  OCR2B = SINE[tone3time * 256 / 15000] >> 2;
}

int main(void) {

  sei();

  // port D io direction
  DDRD = 0b11111100;

  // port B io direction
  DDRB = (1 << 1) | (1 << 3) | (1 << 5);

  PORTB = 0;
  PORTD = 0;

  next2 = interval2;
  next4 = interval4;
  next7 = interval7;

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
  byte prescale1 = 0b001;
  byte countToA = 0b1000;
  OCR1A = 16000;
  TCCR1B = 0b00000000 | prescale1 | countToA;
  TCCR1A = 0b00000000; // no output, no output, fast

  uint16_t lastticks = TCNT1;

  while (1) {
    uint16_t ticks = TCNT1;
    long elapsed;
    if( lastticks > ticks ) {
      elapsed = 16000 - lastticks + ticks;
    } else {
      elapsed = ticks - lastticks;
    }
    lastticks = ticks;
    next2 -= elapsed;
    if( next2 <= 0 ) {
      pulseD(2);
      next2 += interval2;
    }
    next4 -= elapsed;
    if( next4 <= 0 ) {
      pulseD(4);
      next4 += interval4;
    }
    next7 -= elapsed;
    if( next7 <= 0 ) {
      pulseD(7);
      next7 += interval7;
    }
  }

  return 0;

}
