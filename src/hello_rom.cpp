
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_rom(void) {


  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**                 KC705 - ROM Test                   **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nDIP switches set the ROM address. See LEDs for the output values.\n");
  xil_printf("Should be \'Hello, world. (48 65 6C 6C 6F 2C 20 77 6F 72 6C 64 2E 00)\'\n");
  xil_printf("Push the rotary to exit.\n");

  // Set up a pointer to the custom AXI-based MUX. The LSB of its first int register
  // determines which input signals (in0 or in1) get passed to the output out0.
  int* p_mux = (int*) MUX_BASEADDR;
  p_mux[0] = 4; // set the mux to pass in4

  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) { }

  xil_printf("Rotary pressed. Exiting...\n\n");
  p_mux[0] = 0; // set the mux to pass in0
  return 0;
}
