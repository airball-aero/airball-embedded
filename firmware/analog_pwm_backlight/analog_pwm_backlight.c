#include <avr/interrupt.h>
#include <avr/io.h>

#define PWM_OUTPUT_MIN 3

int main(void) {
  // Set up pin PB4 as an output.
  DDRB |= _BV(PB4);
  PORTB &= ~_BV(PB4);

  // Enable the ADC in one-shot mode on pin PB3 with /8 prescaler.
  ADMUX = _BV(ADLAR) | _BV(MUX0) | _BV(MUX1);
  ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);

  // Set up PWM output on pin PB4/OC1B.
  TCCR1 |= _BV(CS10);
  GTCCR |= _BV(PWM1B) | _BV(COM1B1);

  // Set an initial value for PWM output (dim, but on).
  OCR1B = PWM_OUTPUT_MIN;

  // Enable interrupts.
  sei();

  // Main (infinite) loop.
  while (1) {
    // Request an ADC conversion and wait for it to complete.
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & _BV(ADSC));

    // Read the upper 8 bits of the ADC value.
    uint8_t adc_value = ADCH;
    OCR1B = (adc_value < PWM_OUTPUT_MIN) ? PWM_OUTPUT_MIN : adc_value;
  }
}
