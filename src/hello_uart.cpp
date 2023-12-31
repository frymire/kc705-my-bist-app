
/*
 * This file contains a design example using the UartLite driver (XUartLite) and
 * hardware device using the interrupt mode.
 *
 * The user must provide a physical loopback such that data which is
 * transmitted will be received.
 */

#include "kc705_my_bist.h"
#include "xuartlite.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xil_printf.h"

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the UartLite device.
 */
#define UART_BUFFER_SIZE 512

int UartLiteIntrExample(u16 DeviceId);
int SetupInterruptSystem(XUartLite *UartLitePtr);
void SendHandler(void *CallBackRef, unsigned int EventData);
void RecvHandler(void *CallBackRef, unsigned int EventData);

/************************** Variable Definitions *****************************/

static XUartLite UartLite; /* The instance of the UartLite Device */
static XIntc InterruptController;

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */

/*
 * The following buffers are used in this example to send and receive data
 * with the UartLite.
 */
u8 uart_send_buffer[UART_BUFFER_SIZE];
u8 uart_receive_buffer[UART_BUFFER_SIZE];

/*
 * The following counters are used to determine when the entire buffer has
 * been sent and received.
 */
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;

/******************************************************************************/
/**
 *
 * Main function to call the UartLite interrupt example.
 *
 * @param  None
 *
 * @return XST_SUCCESS if successful, XST_FAILURE if unsuccessful
 *
 * @note   None
 *
 *******************************************************************************/
int hello_uart(void) {

  xil_printf("\r\n********************************************************");
  xil_printf("\r\n********************************************************");
  xil_printf("\r\n**               KC705 - UART Test                    **");
  xil_printf("\r\n********************************************************");
  xil_printf("\r\n********************************************************\r\n");

  // I think the driver is treating the "enter" press from selecting this
  // subroutine as a separate return to inbyte(), so do one read to absorb it.
  volatile int dummy_choice = inbyte();

  xil_printf("Input a character: ");
  int choice = inbyte();
  xil_printf("You typed: %c\r\n", choice);

//  xil_printf("Running UART Lite interrupt test example...\r\n");
//  int status = UartLiteIntrExample(UARTLITE_DEVICE_ID);
//  if (status != XST_SUCCESS) {
//    xil_printf("UART Lite interrupt example failed\r\n");
//    return XST_FAILURE;
//  }
//  xil_printf("UART Lite interrupt example completed successfully.\r\n");
  return XST_SUCCESS;
}

/****************************************************************************/
/**
 *
 * This function does a minimal test on the UartLite device and driver as a
 * design example. The purpose of this function is to illustrate
 * how to use the XUartLite component.
 *
 * This function sends data and expects to receive the same data through the
 * UartLite. The user must provide a physical loopback such that data which is
 * transmitted will be received.
 *
 * This function uses interrupt driver mode of the UartLite device. The calls
 * to the UartLite driver in the handlers should only use the non-blocking
 * calls.
 *
 * @param  DeviceId is the Device ID of the UartLite Device and is the
 *   XPAR_<uartlite_instance>_DEVICE_ID value from xparameters.h.
 *
 * @return XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note
 *
 * This function contains an infinite loop such that if interrupts are not
 * working it may never return.
 *
 ****************************************************************************/
int UartLiteIntrExample(u16 DeviceId) {
  int Status;
  int Index;

  /*
   * Initialize the UartLite driver so that it's ready to use.
   */
  Status = XUartLite_Initialize(&UartLite, DeviceId);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Perform a self-test to ensure that the hardware was built correctly.
   */
  Status = XUartLite_SelfTest(&UartLite);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Connect the UartLite to the interrupt subsystem such that interrupts can
   * occur. This function is application specific.
   */
  Status = SetupInterruptSystem(&UartLite);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Setup the handlers for the UartLite that will be called from the
   * interrupt context when data has been sent and received, specify a
   * pointer to the UartLite driver instance as the callback reference so
   * that the handlers are able to access the instance data.
   */
  XUartLite_SetSendHandler(&UartLite, SendHandler, &UartLite);
  XUartLite_SetRecvHandler(&UartLite, RecvHandler, &UartLite);

  /*
   * Enable the interrupt of the UartLite so that interrupts will occur.
   */
  XUartLite_EnableInterrupt(&UartLite);

  /*
   * Initialize the send buffer bytes with a pattern to send and the
   * the receive buffer bytes to zero to allow the receive data to be
   * verified.
   */
  for (Index = 0; Index < UART_BUFFER_SIZE; Index++) {
    uart_send_buffer[Index] = 'A' + Index;
    uart_receive_buffer[Index] = 0;
  }

  /*
   * Start receiving data before sending it since there is a loopback.
   */
  XUartLite_Recv(&UartLite, uart_receive_buffer, UART_BUFFER_SIZE);

  /*
   * Send the buffer using the UartLite.
   */
  XUartLite_Send(&UartLite, uart_send_buffer, UART_BUFFER_SIZE);

  /*
   * Wait for the entire buffer to be received, letting the interrupt
   * processing work in the background, this function may get locked
   * up in this loop if the interrupts are not working correctly.
   */
  while ((TotalReceivedCount != UART_BUFFER_SIZE)
      || (TotalSentCount != UART_BUFFER_SIZE)) {
  }

  /*
   * Verify the entire receive buffer was successfully received.
   */
  for (Index = 0; Index < UART_BUFFER_SIZE; Index++) {
    if (uart_receive_buffer[Index] != uart_send_buffer[Index]) {
      return XST_FAILURE;
    }
  }

  return XST_SUCCESS;
}

/*****************************************************************************/
/**
 *
 * This function is the handler which performs processing to send data to the
 * UartLite. It is called from an interrupt context such that the amount of
 * processing performed should be minimized. It is called when the transmit
 * FIFO of the UartLite is empty and more data can be sent through the UartLite.
 *
 * This handler provides an example of how to handle data for the UartLite,
 * but is application specific.
 *
 * @param  CallBackRef contains a callback reference from the driver.
 *   In this case it is the instance pointer for the UartLite driver.
 * @param  EventData contains the number of bytes sent or received for sent
 *   and receive events.
 *
 * @return None.
 *
 * @note   None.
 *
 ****************************************************************************/
void SendHandler(void *CallBackRef, unsigned int EventData) {
  TotalSentCount = EventData;
}

/****************************************************************************/
/**
 *
 * This function is the handler which performs processing to receive data from
 * the UartLite. It is called from an interrupt context such that the amount of
 * processing performed should be minimized.  It is called data is present in
 * the receive FIFO of the UartLite such that the data can be retrieved from
 * the UartLite. The size of the data present in the FIFO is not known when
 * this function is called.
 *
 * This handler provides an example of how to handle data for the UartLite,
 * but is application specific.
 *
 * @param  CallBackRef contains a callback reference from the driver, in
 *   this case it is the instance pointer for the UartLite driver.
 * @param  EventData contains the number of bytes sent or received for sent
 *   and receive events.
 *
 * @return None.
 *
 * @note   None.
 *
 ****************************************************************************/
void RecvHandler(void *CallBackRef, unsigned int EventData) {
  TotalReceivedCount = EventData;
}

/****************************************************************************/
/**
 *
 * This function setups the interrupt system such that interrupts can occur
 * for the UartLite device. This function is application specific since the
 * actual system may or may not have an interrupt controller. The UartLite
 * could be directly connected to a processor without an interrupt controller.
 * The user should modify this function to fit the application.
 *
 * @param    UartLitePtr contains a pointer to the instance of the UartLite
 *           component which is going to be connected to the interrupt
 *           controller.
 *
 * @return   XST_SUCCESS if successful, otherwise XST_FAILURE.
 *
 * @note     None.
 *
 ****************************************************************************/
int SetupInterruptSystem(XUartLite *UartLitePtr) {

  int Status;

  /*
   * Initialize the interrupt controller driver so that it is ready to
   * use.
   */
  Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Connect a device driver handler that will be called when an interrupt
   * for the device occurs, the device driver handler performs the
   * specific interrupt processing for the device.
   */
  Status = XIntc_Connect(&InterruptController, UARTLITE_INT_IRQ_ID,
      (XInterruptHandler) XUartLite_InterruptHandler, (void *) UartLitePtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Start the interrupt controller such that interrupts are enabled for
   * all devices that cause interrupts, specific real mode so that
   * the UartLite can cause interrupts through the interrupt controller.
   */
  Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  /*
   * Enable the interrupt for the UartLite device.
   */
  XIntc_Enable(&InterruptController, UARTLITE_INT_IRQ_ID);

  /*
   * Initialize the exception table.
   */
  Xil_ExceptionInit();

  /*
   * Register the interrupt controller handler with the exception table.
   */
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
      (Xil_ExceptionHandler) XIntc_InterruptHandler, &InterruptController);

  /*
   * Enable exceptions.
   */
  Xil_ExceptionEnable();

  return XST_SUCCESS;
}

