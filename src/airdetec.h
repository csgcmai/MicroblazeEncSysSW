/*
 * airdetec.h
 *  Driver of air quality detection
 *  Created on: 2012-3-29
 *      Author: Maple
 */

#ifndef AIRDETEC_H_
#define AIRDETEC_H_

int getairqual(XGpio* InstancePtr,u32 *airqual)
{
	(*airqual)=XGpio_DiscreteRead(InstancePtr,1);
	xil_printf("The degree of air quality is : %d\n",*airqual);
	return XST_SUCCESS;
}

#endif /* AIRDETEC_H_ */
