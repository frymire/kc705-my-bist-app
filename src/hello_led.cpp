
#include "kc705_my_bist.h"
#include "xgpio.h"
#include "xstatus.h"

static void pause(int delay) { for (volatile int i = 0; i < delay; i++) { } }

int hello_led(void) {

  const int k_num_leds = 8;
  const int k_led_channel = 1; // the GPIO channel that the LEDs are on, either 1 or 2

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - GPIO LED Test                          **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("Watch the LEDs...\n");

  XGpio gpio;
  if (XGpio_Initialize(&gpio, LED_GPIO_DEVICE) != XST_SUCCESS) {
    xil_printf("GPIO initialization failed.\n");
    return XST_FAILURE;
  }

  // Set the LED signals as outputs.
  XGpio_SetDataDirection(&gpio, k_led_channel, 0b00000000);

  // Cylon eyes...
  int delay = 1000000; // 10 ms
  for (int i = 0; i < 3; i++) {

    // To the left.
    for (int position = 0; position < k_num_leds; position++) {
      XGpio_DiscreteWrite(&gpio, k_led_channel, 1 << position);
      if (position < k_num_leds - 1) pause(delay);
    }

    // To the right.
    for (int position = k_num_leds - 1; position >= 0; position--) {
      XGpio_DiscreteWrite(&gpio, k_led_channel, 1 << position);
      if (position > 0) pause(delay);
    }
  }

  // Toggling with a mask...
  u32 leds = 0b01010101;
  delay = 5000000;
  for (int i = 0; i < 10; i++) {
    XGpio_DiscreteWrite(&gpio, k_led_channel, leds); // set LED bits
    pause(delay);
    leds ^= 0b11110000; // 0xF0, toggle the first 4 LEDs, leave the last 4 untouched.
  }

  XGpio_DiscreteWrite(&gpio, k_led_channel, 0); // clear LEDs
  return 0;
}
