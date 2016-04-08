/*
 *  initial.h
 *  Initial controller and devices
 *  Created on: 2012-3-11
 *      Author: Maple
 */

#ifndef INITIAL_H_
#define INITIAL_H_

#include "xparameters.h"
#include "xintc.h"
#include "xuartlite.h"
#include "timer.h"
#include "xgpio.h"
#include "xstatus.h"
#include "iic.h"


int ctr_init(XIntc *intcptr,XUartLite *gpctrptr,XUartLite *btctrptr,
		XTmrCtr *tmptr,XIic *irctrptr,XGpio *aireader)
{
	int status;
	//interrupt controller
	status= XIntc_Initialize(intcptr, XPAR_AXI_INTC_0_DEVICE_ID);
	if (status != XST_SUCCESS)     return XST_FAILURE;
	//gprs module controller
    status=XUartLite_Initialize(gpctrptr,  XPAR_GPRS_UART_DEVICE_ID);
    if (status != XST_SUCCESS)     return XST_FAILURE;
    //blue tooth controller
    status=XUartLite_Initialize(btctrptr,  XPAR_BLUE_UART_DEVICE_ID);
    if (status != XST_SUCCESS)     return XST_FAILURE;
    //timer controller
    status=XTmrCtr_Initialize(tmptr,XPAR_AXI_TIMER_0_DEVICE_ID);
    if (status != XST_SUCCESS)     return XST_FAILURE;
    //IRTP controller
    status=XIic_Initialize(irctrptr, XPAR_IRTP_IIC_DEVICE_ID);
    if (status != XST_SUCCESS)     return XST_FAILURE;
    //air quality controller
    status=XGpio_Initialize(aireader,XPAR_AIR_QUAL_DEVICE_ID);
    if (status != XST_SUCCESS)     return XST_FAILURE;

    //parameter initial
    XGpio_SetDataDirection(aireader, 1, 0xffffffff);

    XUartLite_Config gprs,bluetooth;

    gprs.BaudRate=XPAR_GPRS_UART_BAUDRATE;
    gprs.DataBits=XPAR_GPRS_UART_DATA_BITS;
    gprs.DeviceId=XPAR_GPRS_UART_DEVICE_ID;
    gprs.ParityOdd=XPAR_GPRS_UART_ODD_PARITY;
    gprs.RegBaseAddr=XPAR_GPRS_UART_BASEADDR;
    gprs.UseParity=XPAR_GPRS_UART_USE_PARITY;

    bluetooth.BaudRate=XPAR_BLUE_UART_BAUDRATE;
    bluetooth.DataBits=XPAR_BLUE_UART_DATA_BITS;
    bluetooth.DeviceId=XPAR_BLUE_UART_DEVICE_ID;
    bluetooth.ParityOdd=XPAR_BLUE_UART_ODD_PARITY;
    bluetooth.RegBaseAddr=XPAR_BLUE_UART_BASEADDR;
    bluetooth.UseParity=XPAR_BLUE_UART_USE_PARITY;

    status = XUartLite_CfgInitialize(gpctrptr,&gprs,XPAR_GPRS_UART_BASEADDR);
    if (status != XST_SUCCESS)     return XST_FAILURE;
    status = XUartLite_CfgInitialize(btctrptr,&bluetooth,XPAR_BLUE_UART_BASEADDR);
    if (status != XST_SUCCESS)     return XST_FAILURE;


    XIic_Config irtp;

    irtp.BaseAddress=XPAR_IRTP_IIC_BASEADDR;
    irtp.DeviceId=XPAR_IRTP_IIC_DEVICE_ID;
    irtp.GpOutWidth=XPAR_IRTP_IIC_GPO_WIDTH;
    irtp.Has10BitAddr=XPAR_IRTP_IIC_TEN_BIT_ADR;

    status = XIic_CfgInitialize(irctrptr,&irtp,XPAR_IRTP_IIC_BASEADDR);
    if (status != XST_SUCCESS)     return XST_FAILURE;

    //interrupt connect
    status = uart_init(intcptr,gpctrptr,XPAR_GPRS_UART_DEVICE_ID,
    		XPAR_AXI_INTC_0_GPRS_UART_INTERRUPT_INTR);
    if (status != XST_SUCCESS)     return XST_FAILURE;

    status = uart_init(intcptr,btctrptr, XPAR_BLUE_UART_DEVICE_ID,
    		XPAR_AXI_INTC_0_BLUE_UART_INTERRUPT_INTR);
    if (status != XST_SUCCESS)     return XST_FAILURE;

    status = timer_init(intcptr,tmptr,XPAR_AXI_TIMER_0_DEVICE_ID,
    		XPAR_AXI_INTC_0_AXI_TIMER_0_INTERRUPT_INTR,0);
    if (status != XST_SUCCESS)     return XST_FAILURE;

    status = iic_init(intcptr,irctrptr, XPAR_IRTP_IIC_DEVICE_ID,
    		XPAR_AXI_INTC_0_IRTP_IIC_IIC2INTC_IRPT_INTR);
    if (status != XST_SUCCESS)     return XST_FAILURE;


    status = XIic_SetAddress(irctrptr,XII_ADDR_TO_SEND_TYPE,irtpaddr);
    if (status != XST_SUCCESS)     return XST_FAILURE;


    return XST_SUCCESS;

}


#endif /* INITIAL_H_ */
