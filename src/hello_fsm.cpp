
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_fsm(void) {


  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**       KC705 - Finite State Machine Test            **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nPress buttons in the sequence: Up, Up, Down, Down, Left, Right, Left, Right, Center.\n");
  xil_printf("The LEDs indicate the number of correct presses.\n");
  xil_printf("Push the rotary to exit.\n");

  // Set up a pointer to the custom AXI-based MUX. The LSB of its first int register
  // determines which input signals (in0 or in1) get passed to the output out0.
  int* p_mux = (int*) MUX_BASEADDR;
  p_mux[0] = 5; // set the mux to pass in5

  while (!(XGpio_ReadReg(XPAR_ROTARY_VIO_BASEADDR, 0x0000) & 0b010)) { }

  xil_printf("Rotary pressed. Exiting...\n\n");
  p_mux[0] = 0; // set the mux to pass in0
  return 0;
}
