#include <avr/io.h>

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
  PORTD ^= (1 << pin);
}

void pulseB( int pin ) {
  PORTB |= (1 << pin);
  PORTB ^= (1 << pin);
}

unsigned long next2 = 0;
unsigned long next4 = 0;
unsigned long next7 = 0;

unsigned long interval2 = 1600000000 / 16481;
unsigned long interval4 = 1600000000 / 11000;
unsigned long interval7 = 1600000000 / 6541;

int main(void) {
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
  TIMSK0 &= ~0b111;
  TIFR0 &= ~0b111;
  byte prescale0 = 0b001;
  TCCR0B = 0b00000000 | prescale0;
  TCCR0A = 0b10100001; // pwm A, pwm B, phase correct
  OCR0A = 250;
  OCR0B = 250;

  // set timer2 to max speed pwm
  // clear interrupts enabled and pending
  TIMSK2 &= ~0b111;
  TIFR2 &= ~0b111;
  // don't force output, reserved, reset at top, prescale=1
  byte prescale = 0b001;
  //byte prescale = 0b010;
  TCCR2B = 0b00000000 | prescale;

  // pwm A, pwm B, reserved, phase correct PWM
  TCCR2A = 0b10100001;

  OCR2A = 255;
  OCR2B = 255;

  // timer1
  TIMSK1 &= ~0b111;
  TIFR1 &= ~0b111;
  byte prescale1 = 0b001;
  TCCR1B = 0b00000000 | prescale1;
  TCCR1A = 0b00000000; 

  unsigned long time = 0;
  int mod1 = 0;
  int mod2 = 0;
  int fmod1 = 0;
  int fmod2 = 0;

  uint16_t lastticks = TCNT1;

  while (1) {
    uint16_t ticks = TCNT1;
    if( lastticks > ticks ) {
      time += 65536 - lastticks + ticks;
    } else {
      time += ticks - lastticks;
    }
    lastticks = ticks;
    if( time > next2 ) {
      pulseD(2);
      next2 += interval2;
    }
    if( time > next4 ) {
      pulseD(4);
      next4 += interval4;
    }
    if( time > next7 ) {
      pulseD(7);
      next7 += interval7;
    }
    if( (time % 25) == 0 ) {
      mod1 ++;
      mod2 ++;
      OCR0A = SINE[ (mod1) & 255 ] >> 1;
      OCR0B = SINE[ (mod1 / 7) & 255 ] >> 1;
      OCR2B = SINE[ (mod2 / 16) & 255 ] >> 1;
    }
  }

  //OCR2B = 1;//( time / 10000 ) & 255;
  //OCR2A = 1;//( time / 40000 ) & 255;
  //OCR2B = SINE[( time / 10000 ) & 255];
  //OCR2A = SINE[( time / 40000 ) & 255];
  //int value = ( ( time / 1000 ) & 255 ) * 4;
  //if( value > 255 ) value = 255;
  //OCR2A = value;
  //analogWrite(11, ( time / 10000 ) & 255);
  //analogWrite(3, ( time / 40000 ) & 255);

  return 0;

}
