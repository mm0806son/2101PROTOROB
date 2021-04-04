#include <avr/io.h>
#include <util/delay.h>

int main (void) {
  DDRB = 0xff;
  while (1) {
    //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    PORTB = 0b10000000;
    _delay_ms(100);                       // wait for a second
    //digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    PORTB = 0x00;
    _delay_ms(100);                       // wait for a second

  }
}
