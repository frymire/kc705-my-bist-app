
#include "kc705_my_bist.h"
#include "xgpio.h"
#include "xstatus.h"

int hello_vio(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**             KC705 - VIO Test                       **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");

  XGpio gpio;
  XStatus status = XGpio_Initialize(&gpio, VIO_DEVICE);
  if (status != XST_SUCCESS) {
    print("GPIO instance didn't initialize.\n");
    return XST_FAILURE;
  }

  // Read the state of the VIO switches.
  u32 vio = XGpio_DiscreteRead(&gpio, VIO_CHANNEL);

  xil_printf("Data read from VIO input is 0x%x\n\n", (int) vio);
  return XST_SUCCESS;
}
