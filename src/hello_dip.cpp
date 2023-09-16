
#include "kc705_my_bist.h"
#include "xgpio.h"
#include "xstatus.h"

int hello_dip(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - DIP Switch Test                        **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");

  XGpio gpio;
  XStatus status = XGpio_Initialize(&gpio, DIP_DEVICE);
  if (status != XST_SUCCESS) {
    print("GPIO instance didn't initialize.\n");
    return XST_FAILURE;
  }

  // Set the direction for all signals to be inputs
  XGpio_SetDataDirection(&gpio, DIP_CHANNEL, 0b1111);

  // Read the state of the data so that it can be  verified.
  u32 switches = XGpio_DiscreteRead(&gpio, DIP_CHANNEL);

  xil_printf("Data read from GPIO Input is 0x%x\n\n", (int) switches);
  return XST_SUCCESS;
}
