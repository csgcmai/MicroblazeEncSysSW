/*
 * irtp.h
 *
 *  Created on: 2012-3-13
 *      Author: Maple
 */

#ifndef IRTP_H_
#define IRTP_H_

#include "iic.h"
#include "xiic_l.h"
#include "xstatus.h"
#include "xparameters.h"

u8 temprec[]={0x07};
u8 temp[4];
int gettemp(XIic* InstancePtr,u32 *temp100)
{
	int status;

    status = XIic_SetAddress(InstancePtr,XII_ADDR_TO_SEND_TYPE,irtpaddr);
	if (status != XST_SUCCESS)
	{
			print("setadd fail\n");
			return XST_FAILURE;
	}

	InstancePtr->Options=XII_REPEATED_START_OPTION;

	status=XIic_Start(InstancePtr);
	if (status != XST_SUCCESS)
	{
			print("start fail\n");
			return XST_FAILURE;
	}
	sended=1;
	status=XIic_MasterSend(InstancePtr,temprec,1);
	if (status != XST_SUCCESS)
	{
		print("send fail£¬ bus is busy wait for not busy\n");
		return XST_FAILURE;
	}

	while(sended){}
	recved=1;
	InstancePtr->Options=0x00;
	status=XIic_MasterRecv(InstancePtr,temp,3);

	if (status != XST_SUCCESS)
	{
		print("recv fail\n");
		return XST_FAILURE;
	}

	while(recved){}
	(*temp100)= temp[1];
	(*temp100)=(*temp100)*256+temp[0];
	(*temp100)=(*temp100)*2-27315;
	xil_printf("temp is : %d cel ",*temp100);
	while(XIic_IsIicBusy(InstancePtr));
	int stoptry=0;
	stop:status=XIic_Stop(InstancePtr);
	if (status != XST_SUCCESS)
	{
		busbusy=1;
		stoptry++;
		if(stoptry>5)return XST_FAILURE;
		print("stop fail,retry\n");
		while(busbusy);
		goto stop;
	}

	return XST_SUCCESS;
}




#endif /* IRTP_H_ */
