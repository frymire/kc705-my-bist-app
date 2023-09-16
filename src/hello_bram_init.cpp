
#include "kc705_my_bist.h"
#include "xil_printf.h"

#define BASE_ADDR XPAR_BRAM_0_BASEADDR

int hello_bram_init() {

  xil_printf("\n********************************************************\n");
  xil_printf("********************************************************\n");
  xil_printf("**        KC705 - BRAM Initialization Test            **\n");
  xil_printf("********************************************************\n");
  xil_printf("********************************************************\n");

  xil_printf("\nSee kc705-my-bist-app/_ide/bitstream/BRAM Initialization Notes.txt.\n");
  xil_printf("Also, note that running the BRAM Test overwrites the initialized BRAM contents.\n");

  const char* rom_string = (char*) BASE_ADDR;

  xil_printf("\nPrinting 13 bytes from BRAM...\n");
  for (int i = 0; i < 13; i++) { xil_printf("0x%08x: %c\n", rom_string + i, rom_string[i]); }

  if (rom_string[13] == '\0') {
    xil_printf("\nPrinting the same BRAM data as a c-style string...\n");
    xil_printf("%s\n", rom_string);
  }

  return 0;
}
