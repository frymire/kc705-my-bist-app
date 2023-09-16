
#include "kc705_my_bist.h"
#include "xtmrctr.h"
#include "xintc.h"
#include "xil_exception.h"

/*
 * The following constant is used to set the reset value of the timer counter,
 * making this number larger reduces the amount of time this example consumes
 * because it is the value the timer counter is loaded with when it is started
 */
#define RESET_VALUE	 0xFFFFF000

int TmrCtrIntrExample(XIntc* p_intc, XTmrCtr* p_timer_counter, u16 device_id, u16 IntrId, u8 TmrCtrNumber);
static int TmrCtrSetupIntrSystem(XIntc* p_intc, XTmrCtr* InstancePtr, u16 DeviceId, u16 IntrId, u8 TmrCtrNumber);
void TimerCounterHandler(void* CallBackRef, u8 TmrCtrNumber);
static void TmrCtrDisableIntr(XIntc* p_intc, u16 interrupt_id);

/*
 * The following variables are shared between non-interrupt processing and
 * interrupt processing such that they must be global.
 */
volatile int num_expirations;

int hello_timer(void) {

  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************");
  xil_printf("\n**     KC705 - Timer Test                             **");
  xil_printf("\n********************************************************");
  xil_printf("\n********************************************************\n");
  xil_printf("Starting Timer Example\n");

  XIntc intc;
  XTmrCtr timer_counter;  
  const int k_which_timer = 0; // use timer 0 within the timer/counter, not timer 1

  int status = TmrCtrIntrExample(&intc, &timer_counter, TMRCTR_DEVICE_ID, TMRCTR_INTERRUPT_ID, k_which_timer);
  if (status != XST_SUCCESS) {
    xil_printf("Failed XPS Timer Example!\r\n");
    return XST_FAILURE;
  }

  xil_printf("Completed XPS Timer Example!\r\n");
  return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function does a minimal test on the timer counter device and driver as a
 * design example.  The purpose of this function is to illustrate how to use the
 * XTmrCtr component.  It initializes a timer counter and then sets it up in
 * compare mode with auto reload such that a periodic interrupt is generated.
 *
 * This function uses interrupt driven mode of the timer counter.
 *
 * @param	IntcInstancePtr is a pointer to the Interrupt Controller
 *		driver Instance
 * @param	TmrCtrInstancePtr is a pointer to the XTmrCtr driver Instance
 * @param	DeviceId is the XPAR_<TmrCtr_instance>_DEVICE_ID value from
 *		xparameters.h
 * @param	IntrId is XPAR_<INTC_instance>_<TmrCtr_instance>_INTERRUPT_INTR
 *		value from xparameters.h
 * @param	TmrCtrNumber is the number of the timer to which this
 *		handler is associated with.
 *
 * @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE
 *
 * @note		This function contains an infinite loop such that if interrupts
 *		are not working it may never return.
 *
 *****************************************************************************/
int TmrCtrIntrExample(XIntc* p_intc, XTmrCtr* p_timer_counter, u16 DeviceId, u16 IntrId, u8 TmrCtrNumber) {

  num_expirations = 0; // The timer has not yet expired for this iteration of the test

  xil_printf("Setting up the timer counter and interrupt subsystem...\n");
  xil_printf("device id=%d, intr id=%d, tmr number=%d\n", DeviceId, IntrId, TmrCtrNumber);

  // Initialize the timer counter using the device ID generated in xparameters.h.
  int status = XTmrCtr_Initialize(p_timer_counter, DeviceId);
  if (status != XST_SUCCESS) {
    xil_printf("XTmrCtr_Initialize error\r\n");
    return XST_FAILURE;
  }

  // Perform a self-test to ensure that the hardware was built correctly.
  status = XTmrCtr_SelfTest(p_timer_counter, TmrCtrNumber);
  if (status != XST_SUCCESS) {
    xil_printf("XTmrCtr_SelfTest error\n");
    return XST_FAILURE;
  }

  // Connect the timer/counter to the interrupt subsystem.
  status = TmrCtrSetupIntrSystem(p_intc, p_timer_counter, DeviceId, IntrId, TmrCtrNumber);
  if (status != XST_SUCCESS) {
    xil_printf("XTmrCtr_SetupIntrSystem error\n");
    return XST_FAILURE;
  }

  /*
   * Setup the handler for the timer counter that will be called from the
   * interrupt context when the timer expires, specify a pointer to the
   * timer counter driver instance as the callback reference so the handler
   * is able to access the instance data
   */
  XTmrCtr_SetHandler(p_timer_counter, TimerCounterHandler, p_timer_counter);

  /*
   * Enable the interrupt of the timer counter so interrupts will occur
   * and use auto reload mode such that the timer counter will reload
   * itself automatically and continue repeatedly, without this option
   * it would expire once only
   */
  XTmrCtr_SetOptions(p_timer_counter, TmrCtrNumber, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

  /*
   * Set a reset value for the timer counter such that it will expire
   * earlier than letting it roll over from 0, the reset value is loaded
   * into the timer counter when it is started
   */
  XTmrCtr_SetResetValue(p_timer_counter, TmrCtrNumber, RESET_VALUE);
  xil_printf("XTmrCtr_SetResetValue to 0x%x\n", RESET_VALUE);

  // Start the timer counter, then wait for it to timeout a number of times.
  xil_printf("Starting the timer counter...\n");
  XTmrCtr_Start(p_timer_counter, TmrCtrNumber);

  int last_timer_expired = 0;

  while (true) {

    // Wait for the first timer counter to expire as indicated by the shared variable which the handler will increment.
    while (num_expirations == last_timer_expired) {}
    last_timer_expired = num_expirations;

    // If it has expired a number of times, then stop the timer counter and stop this example.
    if (num_expirations == 3) {
      xil_printf("Timer counter has expired.\n");
      XTmrCtr_Stop(p_timer_counter, TmrCtrNumber);
      break;
    }
  }

  TmrCtrDisableIntr(p_intc, DeviceId);
  xil_printf("XTmrCtr_DisableIntr success\r\n");
  return XST_SUCCESS;
}

/*
 * A handler for timer/counter events.
 *
 * @param	CallBackRef is a pointer to the callback function
 * @param	TmrCtrNumber is the number of the timer to which this handler is associated with.
 */
void TimerCounterHandler(void* CallBackRef, u8 TmrCtrNumber) {

  // The callback reference can be used as a pointer to the timer/counter that triggered the interrupt.
  XTmrCtr* p_timer_counter = (XTmrCtr*) CallBackRef;

  // Verify that the timer/counter has expired. This isn't necessary, but it proves that we can use the callback reference.
  if (XTmrCtr_IsExpired(p_timer_counter, TmrCtrNumber)) { xil_printf("[I] "); }

  num_expirations++;
  if (num_expirations == 3) { XTmrCtr_SetOptions(p_timer_counter, TmrCtrNumber, 0); }
}

/*****************************************************************************/
/**
 * This function setups the interrupt system such that interrupts can occur
 * for the timer counter. This function is application specific since the actual
 * system may or may not have an interrupt controller.  The timer counter could
 * be directly connected to a processor without an interrupt controller.  The
 * user should modify this function to fit the application.
 *
 * @param	IntcInstancePtr is a pointer to the Interrupt Controller
 *		driver Instance.
 * @param	TmrCtrInstancePtr is a pointer to the XTmrCtr driver Instance.
 * @param	DeviceId is the XPAR_<TmrCtr_instance>_DEVICE_ID value from
 *		xparameters.h.
 * @param	IntrId is XPAR_<INTC_instance>_<TmrCtr_instance>_VEC_ID
 *		value from xparameters.h.
 * @param	TmrCtrNumber is the number of the timer to which this
 *		handler is associated with.
 *
 * @return	XST_SUCCESS if the Test is successful, otherwise XST_FAILURE.
 *
 * @note		This function contains an infinite loop such that if interrupts
 *		are not working it may never return.
 *
 ******************************************************************************/
static int TmrCtrSetupIntrSystem(
    XIntc* p_intc,
    XTmrCtr* p_timer_counter,
    u16 DeviceId,
    u16 IntrId,
    u8 TmrCtrNumber) {


  /*
   * Initialize the interrupt controller driver so that
   * it's ready to use, specify the device ID that is generated in
   * xparameters.h
   */
  int status = XIntc_Initialize(p_intc, INTC_DEVICE_ID);
  if (status != XST_SUCCESS) {
    xil_printf("Unable to initialize interrupt controller.\n\r");
    return XST_FAILURE;
  }
  //xil_printf("XIntc_Initialize success\r\n");

  /*
   * Connect a device driver handler that will be called when an interrupt
   * for the device occurs, the device driver handler performs the specific
   * interrupt processing for the device
   */
  status = XIntc_Connect(p_intc, IntrId, (XInterruptHandler) XTmrCtr_InterruptHandler, (void *) p_timer_counter);
  if (status != XST_SUCCESS) {
    xil_printf("Unable to connect timer interrupt.\n\r");
    return XST_FAILURE;
  }
  //xil_printf("XIntc_Connect success\r\n");

  /*
   * Start the interrupt controller such that interrupts are enabled for
   * all devices that cause interrupts, specific real mode so that
   * the timer counter can cause interrupts thru the interrupt controller.
   */
  status = XIntc_Start(p_intc, XIN_REAL_MODE);
  if (status != XST_SUCCESS) {
    xil_printf("Unable to start interrupt controller.\n\r");
    return XST_FAILURE;
  }
  //xil_printf("XIntc_Start success\r\n");

  // Enable the interrupt for the timer counter
  XIntc_Enable(p_intc, IntrId);
  //xil_printf("XIntc_Enable success\r\n");

  // Initialize the exception table.
  Xil_ExceptionInit();

  // Register the interrupt controller handler with the exception table.
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XIntc_InterruptHandler, p_intc);

  // Enable non-critical exceptions.
  Xil_ExceptionEnable();

  return XST_SUCCESS;
}

/*
 * Disables the interrupts for the Timer.
 *
 * @param	p_intc is a reference to the Interrupt Controller driver Instance.
 * @param	interrupt_id is XPAR_<INTC_instance>_<Timer_instance>_VEC_ID value from xparameters.h.
 */
static void TmrCtrDisableIntr(XIntc* p_intc, u16 interrupt_id) {

  // Disable the interrupt for the timer counter.
  XIntc_Disable(p_intc, interrupt_id);

  // Disconnect the driver handler.
  XIntc_Disconnect(p_intc, interrupt_id);

  // Disable non-critical exceptions.
  Xil_ExceptionDisable();

  // Remove the interrupt controller handler from the exception table.
  Xil_ExceptionRemoveHandler(XIL_EXCEPTION_ID_INT);

  // Stop the interrupt controller such that interrupts are disabled for all devices that cause interrupts.
  XIntc_Stop(p_intc);
}
