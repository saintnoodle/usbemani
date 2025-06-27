repeating_timer_t _button_timer;

static bool down[BUTTONS_ACTIVE] = { false };

void _impl_button_poll(void) {
  for (int i = 0; i < BUTTONS_ACTIVE; i++) {
    // Get the filtered analog value for this button.
    const uint8_t value = Analog_Get(i);
    // Determine if the button is pressed.
    if (down[i] && value > BUTTON_UP_THRESHOLD) {
      down[i] = false;
    } else if (!down[i] && value < BUTTON_DOWN_THRESHOLD) {
      down[i] = true;
    }
    // Update the debounce state
    _buttons.debounce[i] <<= 1; if (down[i]) _buttons.debounce[i] |= 1;
    // If we have 7 consecutive lows, the button is pressed
    if (_buttons.debounce[i] == 0x80) { _buttons.mask |=  (1 << i); continue; }
    // If we have 7 consecutive highs, the button has been released
    if (_buttons.debounce[i] == 0x7F) { _buttons.mask &= ~(1 << i); continue; }
  }
}

bool _impl_button_interrupt(repeating_timer_t *rt) {
  _impl_button_poll();
  return true;
}

void _impl_button_init(void) {
#if BUTTONS_ACTIVE > 0
  static const uint   poll = 1000000 / 1000;

  for (int i = 0; i < BUTTONS_ACTIVE; i++) {
    // Start with an empty debounce register
    _buttons.debounce[i] = 0x55;
  }

  alarm_pool_add_repeating_timer_us(
    _impl_arch_alarmPool(),
    poll,
    _impl_button_interrupt,
    NULL,
    &_button_timer
  );
#endif
}