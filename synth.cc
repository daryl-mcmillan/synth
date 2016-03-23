#include <avr/io.h>
#include <util/delay.h>

typedef char byte;

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

unsigned long next5 = 0;
unsigned long next7 = 0;
unsigned long next9 = 0;

unsigned long interval5 = 700;
unsigned long interval7 = 200;
unsigned long interval9 = 300;

int main(void) {
  // port D io direction
  DDRD = (1 << 3) | (1 << 4) | (1 << 5) | (1 << 7);

  // port B io direction
  DDRB = (1 << 1) | (1 << 3);

  PORTB = 0;
  PORTD = 0;

  next5 = interval5;
  next7 = interval7;
  next9 = interval9;

  // set timer2 to max speed pwm
  // clear interrupts enabled and pending
  TIMSK2 &= ~0b111;
  TIFR2 &= ~0b111;
  // don't force output, reserved, reset at top, prescale=1
  byte prescale = 0b001;
  //byte prescale = 0b010;
  TCCR2B = 0b00000000 | prescale;

  // pwm A, pwm B, reserved, phase correct PWM
  //TCCR2A = 0b10100001;

  // pwm A, pwm B, reserved, Fast PWM
  TCCR2A = 0b10100011;

  OCR2A = 255;
  OCR2B = 255;

  unsigned long time = 0;
  int mod1 = 0;
  int mod2 = 0;
  int fmod1 = 0;
  int fmod2 = 0;

  while (1) {
    time ++;
    if( time > next5 ) {
      pulseD(5);
      next5 += interval5;
    }
    if( time > next7 ) {
      pulseD(7);
      fmod1++;
      next7 += interval7 + SINE[fmod1 & 255];
    }
    if( time > next9 ) {
      pulseB(1);
      fmod2++;
      next9 += interval9 - SINE[fmod2 & 255];
    }
    if( (time % 25) == 0 ) {
      mod1 ++;
      mod2 ++;
      OCR2A = SINE[ (mod1) & 255 ];
      OCR2B = SINE[ (mod2) & 255 ];
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
