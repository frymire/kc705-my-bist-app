
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_multiplier(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**             KC705 - Multiplier Test                **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nThe LEDs reflect the square of the DIPS switches, both treated as u4.\n");
  xil_printf("Press the center button to enable the square calculation.\n");
  xil_printf("Press the down button to reset the multiplier registers.\n");
  xil_printf("Push rotary to exit.\n");

  // Set the 3 rotary switch GPIO bits as inputs. The channel 1 tri-state control
  // register is offset by 0x4 bytes from the GPIO device base address.
  XGpio_WriteReg(XPAR_ROTARY_VIO_BASEADDR, 0x4, 0b111);

  // Set up a pointer to the custom AXI-based MUX. The two LSBs of its first int
  // register determines which input signals get passed to the output.
  int* p_mux = (int*) MUX_BASEADDR;
  p_mux[0] = 3; // set the mux to pass in3 to out0

  // Read the current values for the rotary switch. The channel 1 data register is
  // at the GPIO device base address. Loop until the switch is pushed (middle bit).
  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) {}

  xil_printf("Exiting...\n\n");
  p_mux[0] = 0; // set the mux to pass in0 to out0

  return 0;
}
