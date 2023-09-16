/*
 * Example of using a non-maskable break in MicroBlaze.
 */

#include "kc705_my_bist.h"
#include "xtmrctr.h"
#include "xil_printf.h"

static int break_count = 0;

/*
 * Declaration of break handler with attributes.
 *
 * Using a break handler requires adding the break vector manually in the linker
 * script and adding the call to the handler in assembler. See the files
 * lscript.ld and vectors.S in this example.
 *
 * The break handler code must be defined with attribute break_handler to ensure that the
 * compiler will generate code to save and restore used registers and emit an rtbd
 * instruction to return from the handler:
 */
//void break_handler() __attribute__((break_handler));
//
//void break_handler() {
//  print("We're inside break_handler!\n");
//  break_count++;
//}

int hello_nonmaskable_interrupt() {

  const int k_timer_id = 0; // The timer/counter instance has two timers. Use the first one.
  const int k_delay = 300000000; // 3*100 MHz = 3 seconds

  print("Entering main...\n");

  // Initialize a timer (i.e. a timer/counter in countdown mode).
  XTmrCtr timer_counter;
  int status = XTmrCtr_Initialize(&timer_counter, XPAR_TMRCTR_0_DEVICE_ID);
  if (status != XST_SUCCESS) {
    xil_printf("Failed to initialize timer/counter instance.\n");
    return XST_FAILURE;
  }

  // Set up timer to count down to zero (XTC_DOWN_COUNT_OPTION) and generate a one clock
  // cycle pulse on the generateout0 output (XTC_EXT_COMPARE_OPTION).
  XTmrCtr_SetOptions(&timer_counter, k_timer_id, XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION);
  XTmrCtr_SetResetValue(&timer_counter, k_timer_id, k_delay);

  /* Wait until the timer has finished counting */
  xil_printf("Counting down for 3 seconds...\n");
  XTmrCtr_Start(&timer_counter, k_timer_id);
  while (!XTmrCtr_IsExpired(&timer_counter, k_timer_id)) { } // block until the timer expires
  xil_printf("Done counting down.\n");

  xil_printf("Break count = %d\n", break_count);

  /* Check that the break has occurred */
  if (break_count == 0) {
    xil_printf("Expected break did not occur\r\n");
    return XST_FAILURE;
  }

  xil_printf("Exiting main.\n\n");
  return XST_SUCCESS;
}
