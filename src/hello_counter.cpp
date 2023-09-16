
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_counter(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**          KC705 - Up/Down Counter Test              **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nWatch the LEDs.\n");
  xil_printf("Push the down button to count down, release to count up.\n");
  xil_printf("Push the rotary to exit.\n");

  // Set up a pointer to the custom AXI-based MUX. The two LSBs of its first int
  // register determines which input signals get passed to the output.
  int* p_mux = (int*) MUX_BASEADDR;
  p_mux[0] = 2; // set the mux to pass in2 to out0

  // Read the current values for the rotary switch. The channel 1 data register is
  // at the GPIO device base address. Loop until the switch is pushed (middle bit).
  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) {}

  xil_printf("Exiting...\n\n");
  p_mux[0] = 0; // set the mux to pass in0 to out0

  return 0;
}
