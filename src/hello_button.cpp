
#include "kc705_my_bist.h"
#include "xgpio_l.h"

int hello_button() {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**            KC705 - Button Test                     **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nPress any combination of direction buttons, or the rotary switch to exit.\n");

  // Set the first 5 GPIO bits to input. The channel 2 tri-state control register
  // is offset by 0xC bytes from the GPIO device base address.
  XGpio_WriteReg(XPAR_LEDS_BUTTONS_BASEADDR, 0x000C, 0b11111);

  u32 previous_buttons = 0;
  u32 current_buttons = 0;
  u32 cooldown_remaining = 0;

  // Loop until the rotary switch is pushed.
  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) {

    // Read the current values for the buttons. The channel 2 data register is
    // offset by 0x8 bytes from the GPIO device base address.
    current_buttons = XGpio_ReadReg(XPAR_LEDS_BUTTONS_BASEADDR, 0x0008);

    // If any button values changed, print which ones are currently pressed.
    if ((cooldown_remaining == 0) & (current_buttons != previous_buttons)) {

      if (current_buttons & 0b10000) xil_printf("North ");
      if (current_buttons & 0b00100) xil_printf("South ");
      if (current_buttons & 0b01000) xil_printf("East ");
      if (current_buttons & 0b00010) xil_printf("West ");
      if (current_buttons & 0b00001) xil_printf("Center ");

      // Print a newline unless we've released the last button, in which case the line has no text.
      if (current_buttons) xil_printf("\n");

      cooldown_remaining = 1000; // debounce for 10 us
      previous_buttons = current_buttons;
    }

    if (cooldown_remaining > 0) cooldown_remaining--;
  }

  xil_printf("Exiting...\n");
  return (0);
}
