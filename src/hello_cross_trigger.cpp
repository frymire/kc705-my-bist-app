
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_cross_trigger(void) {


  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**            KC705 - Cross Trigger Test              **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nWatch the LEDs. Running loop to detect triggers...\n");
  xil_printf("Push the rotary to exit.\n");

  XGpio gpio;
  if (XGpio_Initialize(&gpio, LED_GPIO_DEVICE) != XST_SUCCESS) {
    xil_printf("GPIO initialization failed.\n");
    return XST_FAILURE;
  }

  // Set the LED signals as outputs.
  XGpio_SetDataDirection(&gpio, k_led_channel, 0b00000000);

  u16 count = 0;
  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) {

    XGpio_DiscreteWrite(&gpio, k_led_channel, count); // put the low byte of count on the LEDs

    // Set a conditional breakpoint here, for example, "(count & 0x00FF) == 0xA5".
    count++;
  }

  XGpio_DiscreteWrite(&gpio, k_led_channel, 0); // clear LEDs
  xil_printf("Exiting...\n\n");
  return 0;
}
