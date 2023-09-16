
#include "kc705_my_bist.h"
#include "lcd.h"

int hello_lcd() {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**            KC705 - LCD Test                        **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");

  xil_printf("Writing to LCD...\n");
  kc_InitializeLCD();
  kc_PrintTwoLines("Mark\0", "Frymire\0");
  xil_printf("Done.\n");
  return 0;
}
