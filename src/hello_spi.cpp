
#include "kc705_my_bist.h"
#include "hello_spi.h"
#include "xspi.h"
#include "xspi_l.h"
#include "xil_printf.h"

#define SPI_DEVICE_ID XPAR_SPI_0_DEVICE_ID

int hello_spi(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - SPI Flash Test                         **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");

  XSpi spi;
  int status;

  XSpi_Config* p_spi_configuration = XSpi_LookupConfig(SPI_DEVICE_ID);
  if (p_spi_configuration == NULL) {
    xil_printf("SPI self-test example failed, could not get configuration.\n");
    return XST_DEVICE_NOT_FOUND;
  }

  status = XSpi_CfgInitialize(&spi, p_spi_configuration, p_spi_configuration->BaseAddress);
  if (status != XST_SUCCESS) {
    xil_printf("SPI self-test example failed, could not initialize.\n");
    return XST_FAILURE;
  }

  status = XSpi_SelfTest(&spi);
  if (status != XST_SUCCESS) {
    xil_printf("SPI self-test example failed.\n");
    return XST_FAILURE;
  }

  xil_printf("Successfully ran SPI self-test example.\n\n");
  return XST_SUCCESS;
}
