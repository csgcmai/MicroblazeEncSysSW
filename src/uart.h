/*
 * uart.h
 *  Implementation of send and receive
 *  Created on: 2012-3-11
 *      Author: Maple
 */

#ifndef UART_H_
#define UART_H_

#include "xparameters.h"
#include <xil_exception.h>
#include "xil_cache.h"
#include "xintc.h"
#include "xuartlite.h"
#include "xuartlite_l.h"
#include "timer.h"

#define seng_length 400
#define recv_length 200
u8 buff[recv_length];
u8 send[seng_length];
u8 recv[recv_length*2];
int recvindex;

int gprsstatus;
int checked;
int bluestatus;
int smsed;
u8 error[]={"ERROR"};
u8 ok[]={"OK"};


int cmdcmp(const u8* cmp,int i);

void sendcmd(const u8* src,XUartLite *UartInstancePtr)
{
	u8 i;
	gprsstatus=0;
	checked=0;
	bluestatus=0;
	recvindex=0;
	for(i=0;i<recv_length;recv[i]=0,i++);
	for(i=0;src[i];i++)
		send[i]=src[i];
	send[i]=0;
	while(UartInstancePtr->IsReady!= XIL_COMPONENT_IS_READY){}
	XUartLite_Send(UartInstancePtr,send,i);
}


void uart_rx_Handler(void *CallBackRef, unsigned int ByteCount)
{
	XUartLite *InstancePtr=(XUartLite*)CallBackRef;
	int status;
	int i;
	for(i=0;i<recv_length;buff[i]=0,i++);
	i=0;
	while(1)
	{
		if(!(XUartLite_IsReceiveEmpty(InstancePtr->RegBaseAddress)))
		{
			buff[i]=XUartLite_ReadReg(InstancePtr->RegBaseAddress, XUL_RX_FIFO_OFFSET);
			recv[recvindex++]=buff[i++];
		}
		if((recvindex>=4)&&(recv[recvindex-4]==0x0D)&&(recv[recvindex-3]==0x0A)&&(recv[recvindex-2]==0x0D)&&(recv[recvindex-1]==0x0A))
		{
			smsed=0;
		}
		if(buff[i-1]==0x0A)break;
		//if(buff[i-1]=='?')break;
		//if(buff[i-1]==0x0D)break;
		if((i>=2)&&(buff[i-2]=='O')&&(buff[i-1]=='K')&&(checked!=1))
			gprsstatus=1;
		if((i>=2)&&(buff[i-2]=='I')&&(buff[i-1]=='L')&&(checked!=1))
			gprsstatus=1;
		if(i>=5)
		{
			if(cmdcmp("ERROR",i)==XST_SUCCESS)
				gprsstatus=2;
		}
	}
	buff[i]=0;
	recv[recvindex]=0;
	//print anything recv from uartlite
	xil_printf(buff);
}

void uart_tx_Handler(void *CallBackRef, unsigned int ByteCount)
{
	XUartLite *InstancePtr=(XUartLite*)CallBackRef;
	while(XUartLite_IsSending(InstancePtr)){}
}

int uart_init(XIntc * IntcInstancePtr,XUartLite *UartInstancePtr,
		u16 DeviceID, u16 IntrId)
{
	int Status;

	  XIntc_Stop(IntcInstancePtr);
	  //Connect serial port
	  Status=XIntc_Connect(IntcInstancePtr, IntrId,
	 			  (XInterruptHandler) XUartLite_InterruptHandler,
	 			  (void *)UartInstancePtr);
	  if (Status != XST_SUCCESS)
		return XST_FAILURE;



	  // Start the interrupt controller
	  Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	  if (Status != XST_SUCCESS)
		  return XST_FAILURE;

	  // Enable the interrupt
	  XIntc_Enable(IntcInstancePtr, IntrId);
	  XUartLite_EnableInterrupt(UartInstancePtr);
	  microblaze_enable_interrupts();

	  // Initialize the exception table
	  Xil_ExceptionInit();

	  // Register the interrupt controller handler with the exception table
	  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			  (Xil_ExceptionHandler) XIntc_InterruptHandler, IntcInstancePtr);

	  // Enable exceptions
	  Xil_ExceptionEnable();

	  // Setup the handler for ttl_Recv, ttl_Send
	  XUartLite_SetRecvHandler(UartInstancePtr,uart_rx_Handler,UartInstancePtr);
	  XUartLite_SetSendHandler(UartInstancePtr,uart_tx_Handler,UartInstancePtr);
	  return XST_SUCCESS;
}


int cmdcmp(const u8* cmp,int i)
{
	int index,size;
	for(size=0;cmp[size];size++);
	for(index=0;index<size;index++)
	{
		if(buff[i-size+index]!=cmp[index])
			return XST_FAILURE;
	}
	return XST_SUCCESS;
}

#endif /* UART_H_ */
