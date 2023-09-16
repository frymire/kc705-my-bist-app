
#include "kc705_my_bist.h"
#include "xgpio.h"


int hello_rotary() {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - Rotary Switch Test                     **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("Turn the rotary switch to increment or decrement the count.\n");
  xil_printf("Push in the rotary switch to exit the test.\n");

  // Set the 3 rotary switch GPIO bits as inputs. The channel 1 tri-state control
  // register is offset by 0x4 bytes from the GPIO device base address.
  XGpio_WriteReg(ROTARY_BASEADDR, 0x4, 0b111);

  u32 previous_rotary = 0;
  u32 current_rotary = 0;
  u32 cooldown_remaining = 0;
  int count = 0;

  while (1) {

    // Read the current values for the rotary switch. The channel 1 data register is
    // at the GPIO device base address.
    current_rotary = XGpio_ReadReg(ROTARY_BASEADDR, 0x0000);

    // If the rotary switch values changed...
    if ((cooldown_remaining == 0) & (current_rotary != previous_rotary)) {

      if (current_rotary & 0b010) {
        xil_printf("Rotary pressed. Exiting...\n\n");
        break;
      }

      if (current_rotary == 0b101) {
        if (previous_rotary == 0b100) {
          xil_printf("Right.");
          count++;
        } else if (previous_rotary == 0b001) {
          xil_printf("Left.");
          count--;
        }
        xil_printf(" Count = %d.\n", count);
      }

      cooldown_remaining = 1000; // debounce for 10 us
      previous_rotary = current_rotary;
    }

    if (cooldown_remaining > 0) cooldown_remaining--;
  }

  return (0);
}
