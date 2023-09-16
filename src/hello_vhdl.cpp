
#include "kc705_my_bist.h"
#include "xgpio.h"

int hello_vhdl(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - Custom VHDL Test                       **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("\nWatch the LEDs.\n");
  xil_printf(" - the rotary switch maps to LEDS[7:5].\n");
  xil_printf(" - an internal counter maps to LEDS[4:2].\n");
  xil_printf(" - DIPS[3:2] map to LEDS[1:0].\n");
  xil_printf(" - push in the rotary switch to exit.\n\n");

  // Set the 3 rotary switch GPIO bits as inputs. The channel 1 tri-state control
  // register is offset by 0x4 bytes from the GPIO device base address.
  XGpio_WriteReg(ROTARY_BASEADDR, 0x4, 0b111);

  // Set up a pointer to the custom AXI-based MUX. The LSB of its first int register
  // determines which input signals (in0 or in1) get passed to the output out0.
  int* p_mux = (int*) MUX_BASEADDR;
  p_mux[0] = 1; // set the mux to pass in0

  // Read the current values for the rotary switch. The channel 1 data register is
  // at the GPIO device base address. Loop until the switch is pushed (middle bit).
  while (!(XGpio_ReadReg(ROTARY_BASEADDR, 0x0000) & 0b010)) {}

  xil_printf("Rotary pressed. Exiting...\n\n");
  p_mux[0] = 0; // set the mux to pass in0

  return 0;
}
