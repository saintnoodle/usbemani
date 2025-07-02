// Jump table entry, used for process unrolling.
#define _ANALOG_JUMP_TABLE(x) case x: if (x < ANALOG_CHANNELS_ACTIVE) _impl_analog_processChannel(x); return;

#if defined(ANALOG_CHANNELS_ACTIVE) && (ANALOG_CHANNELS_ACTIVE > 0)
_impl_magnetic_calibration_t _magnetic_calibration[ANALOG_CHANNELS_ACTIVE];
#endif

volatile uint8_t _analogs_index = 0;

// Set calibration values for a specific analog channel.
void _impl_analog_setCalibration(int i, uint16_t up, uint16_t down) {
  if (i < 0 || i >= ANALOG_CHANNELS_ACTIVE) return;
  _magnetic_calibration[i].min = up < down ? up : down;
  _magnetic_calibration[i].max = up > down ? up : down;
  _magnetic_calibration[i].invert = up > down;
}

// Sets up the IRQ handler on the second core and begin the first conversion
int64_t _impl_analog_startConversion(alarm_id_t id, void *user_data) {
  hw_set_bits(&adc_hw->cs, ADC_CS_START_ONCE_BITS);
  return 0;
}

// Process a single ADC channel, switching to the next channel afterward.
static inline void _impl_analog_processChannel(const uint8_t i) {
  // Read the raw value from the ADC FIFO.
  uint16_t raw = adc_fifo_get();

  // Keep a copy of the latest raw value.
  _impl_magnetic_calibration_t *c = &_magnetic_calibration[i];
  c->last = raw;

  // Clamp to pre-calibrated range.
  if (raw < c->min) raw = c->min;
  if (raw > c->max) raw = c->max;

  // Scale the raw value from 16-bit to 8-bit.
  _analogs[i].raw = (raw - c->min) * 255 / (c->max - c->min) ^ -c->invert;

  // Switch the ADC to the next channel
  const uint8_t next_index = (i+1) % ANALOG_CHANNELS_ACTIVE;
  gpio_put_masked(ANALOG_MAGNETIC_PIN_MASK, next_index << 1 & ANALOG_MAGNETIC_PIN_MASK);
  _analogs_index = next_index;

  // Flag the highest bit of the averaging index, to indicate a new ADC read is ready
  _analogs[i].average.index |= 0x80;

  // Start the next conversion after a brief delay
  alarm_pool_add_alarm_in_us(
    _impl_arch_alarmPool(),
    10,
    _impl_analog_startConversion,
    NULL,
    true
  );
}

// Interrupt handler after each conversion is completed.
void _impl_analog_interrupt(void) {
  switch(_analogs_index) {
    _ANALOG_JUMP_TABLE(0);
    _ANALOG_JUMP_TABLE(1);
    _ANALOG_JUMP_TABLE(2);
    _ANALOG_JUMP_TABLE(3);
    _ANALOG_JUMP_TABLE(4);
    _ANALOG_JUMP_TABLE(5);
    _ANALOG_JUMP_TABLE(6);
    _ANALOG_JUMP_TABLE(7);
    _ANALOG_JUMP_TABLE(8);
    _ANALOG_JUMP_TABLE(9);
    _ANALOG_JUMP_TABLE(10);
    _ANALOG_JUMP_TABLE(11);
    _ANALOG_JUMP_TABLE(12);
    _ANALOG_JUMP_TABLE(13);
    _ANALOG_JUMP_TABLE(14);
    _ANALOG_JUMP_TABLE(15);
  }
}

// Sets up the IRQ handler on the second core and begin the first conversion
int64_t _impl_analog_multicoreInit(alarm_id_t id, void *user_data) {
  adc_irq_set_enabled(true);

  irq_set_exclusive_handler(ADC_IRQ_FIFO, _impl_analog_interrupt);
  irq_set_enabled(ADC_IRQ_FIFO, true);

  hw_set_bits(&adc_hw->cs, ADC_CS_START_ONCE_BITS);
  return 0;
}

// Initializes the analog subsystem
void _impl_analog_init(void) {
  // Setup the analog subsystem
  adc_init();
  adc_fifo_setup(
    true,   // Write each conversion to the ADC FIFO
    false,  // Don't use DMA
    1,      // Threshold for interrupts
    false,  // Don't set the error flag
    false   // Raw 16-bit samples we'll convert to 8-bit later
  );
  adc_set_clkdiv(9600); // ~5000 samples/second to reduce noise

  // Setup ADC pin for analog usage
  adc_gpio_init(ANALOG_MAGNETIC_ADC_INPUT + ADC_BASE_PIN);
  adc_select_input(ANALOG_MAGNETIC_ADC_INPUT);

  // Setup switch pins
  gpio_init_mask(ANALOG_MAGNETIC_PIN_MASK);
  gpio_set_dir_out_masked(ANALOG_MAGNETIC_PIN_MASK);

  // Copy stock calibration values
  const uint16_t up[ANALOG_CHANNELS_ACTIVE] = { ANALOG_CALIBRATION_UP };
  const uint16_t down[ANALOG_CHANNELS_ACTIVE] = { ANALOG_CALIBRATION_DOWN };

  for (int i = 0; i < ANALOG_CHANNELS_ACTIVE; ++i) {
    _impl_analog_setCalibration(i, up[i], down[i]);
  }

  // Start the interupt handler on the second core. We'll use a single-fire timer to achieve this
  alarm_pool_add_alarm_in_us(
    _impl_arch_alarmPool(),
    1,
    _impl_analog_multicoreInit,
    NULL,
    true
  );
}