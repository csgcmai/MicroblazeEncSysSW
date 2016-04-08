/*
 * timer.h
 *  Provide delay function using interrupt
 *  Created on: 2012-3-10
 *      Author: Maple
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "xparameters.h"
#include <xil_exception.h>
#include "xil_cache.h"
#include "xintc.h"
#include "xtmrctr.h"

int timecount=0;//counter of timer controller

//Parameter time length is the time to delay, in millisecond
void delay(int timelength)
{
	timecount=0;
	while(timecount<timelength);
}

void clock_handler(void * CallBackRef, u8 TmrCtrNumber)
{
	// Setup call back reference
	XTmrCtr *InstancePtr = (XTmrCtr *)CallBackRef;
	if (XTmrCtr_IsExpired(InstancePtr, TmrCtrNumber))
	{
		timecount++;
	}
}

int timer_init(XIntc * IntcInstancePtr, XTmrCtr *TmrCtrInstancePtr,
		u16 DeviceID, u16 IntrId, u8 TmrCtrNumber)
{
	int Status;
		//Connect timer controller
	XIntc_Stop(IntcInstancePtr);
	Status = XIntc_Connect(IntcInstancePtr, IntrId,
			(XInterruptHandler) XTmrCtr_InterruptHandler,
			(void *)TmrCtrInstancePtr);
	if (Status != XST_SUCCESS)
	    return XST_FAILURE;


	// Start the interrupt controller
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	// Enable the interrupt
	XIntc_Enable(IntcInstancePtr, IntrId);
	microblaze_enable_interrupts();

	// Initialize the exception table
	Xil_ExceptionInit();

	// Register the interrupt controller handler with the exception table
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
		(Xil_ExceptionHandler) XIntc_InterruptHandler, IntcInstancePtr);

	// Enable exceptions
	Xil_ExceptionEnable();

	// Setup the handler for timer
	XTmrCtr_SetHandler(TmrCtrInstancePtr, clock_handler, TmrCtrInstancePtr);

	//Setting of Timer
	// Set options for timer/counter 0
	XTmrCtr_SetOptions(TmrCtrInstancePtr, TmrCtrNumber,
		XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);

	// Set the number of cycles the timer counts down to [for 1 second at 50 MHz]
	// For each millisecond generate a interrupt, since that FFFFFFF-(50000)=FFFF3CAF
	XTmrCtr_SetResetValue(TmrCtrInstancePtr, TmrCtrNumber, 0xFFFF3CAF);

	// Start the timer counter
	XTmrCtr_Start(TmrCtrInstancePtr,TmrCtrNumber);


	return XST_SUCCESS;
}

#endif /* TIMER_H_ */
