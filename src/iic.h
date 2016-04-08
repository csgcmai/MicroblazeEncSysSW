/*
 * iic.h
 *  Provide support of iic receive and send
 *  Created on: 2012-3-12
 *      Author: Maple
 */

#ifndef IIC_H_
#define IIC_H_

#include "xiic.h"

#define irtpaddr 0x5A   //address of irtp in the smbus
#define tempercmd 0x07

int recved,sended,busbusy;

int gettemp(XIic* InstancePtr,u32 *temp100);

void iic_sta_handler(void *CallBackRef, int StatusEvent)
{
	if(StatusEvent==XIIC_INTR_BNB_MASK)
		busbusy=0;
}

void iic_rx_handler(void *CallBackRef, int ByteCount)
{
	recved=0;
}

void iic_tx_handler(void *CallBackRef, int ByteCount)
{
	sended=0;
}

void test(void *IicInstancePtr)
{
	xil_printf("%d\n",XIic_ReadReg(((XIic*)IicInstancePtr)->BaseAddress,XIIC_IISR_OFFSET));
	xil_printf("%d\n",XIic_ReadReg(((XIic*)IicInstancePtr)->BaseAddress,XIIC_SR_REG_OFFSET));
	print("Hello World\n");
}

int iic_init(XIntc * IntcInstancePtr,XIic *IicInstancePtr,
		u16 DeviceID, u16 IntrId)
{
	int Status;

	 XIntc_Stop(IntcInstancePtr);
	//Connect iic controller
	Status=XIntc_Connect(IntcInstancePtr, IntrId,
		 (XInterruptHandler)XIic_InterruptHandler,
		 (void *)IicInstancePtr);
	 if (Status != XST_SUCCESS)  return XST_FAILURE;

	 // Start the interrupt controller
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS)    return XST_FAILURE;

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


	Status=XIic_Start(IicInstancePtr);
	if (Status != XST_SUCCESS)    return XST_FAILURE;

	//Setup handlers
	XIic_SetRecvHandler(IicInstancePtr, IicInstancePtr,iic_rx_handler);
	XIic_SetSendHandler(IicInstancePtr, IicInstancePtr,iic_tx_handler);
	XIic_SetStatusHandler(IicInstancePtr, IicInstancePtr,iic_sta_handler);


	return XST_SUCCESS;
}




#endif /* IIC_H_ */
