/*
 * sign.h
 *
 *  Created on: 2012-4-27
 *      Author: Maple
 */

#ifndef SIGN_H_
#define SIGN_H_

#include "xparameters.h"
#include "rainbow.h"
#include "timer.h"

void rainbow_sign(int input,int *output)
{
	RAINBOW_mWriteReg(XPAR_RAINBOW_0_BASEADDR,RAINBOW_SLV_REG1_OFFSET,0x00);
	RAINBOW_mWriteReg(XPAR_RAINBOW_0_BASEADDR,RAINBOW_SLV_REG0_OFFSET,input);
    RAINBOW_mWriteReg(XPAR_RAINBOW_0_BASEADDR,RAINBOW_SLV_REG1_OFFSET,0x01);
    delay(100);
    output[0]=RAINBOW_mReadReg(XPAR_RAINBOW_0_BASEADDR,RAINBOW_SLV_REG3_OFFSET);
    output[1]=RAINBOW_mReadReg(XPAR_RAINBOW_0_BASEADDR,RAINBOW_SLV_REG2_OFFSET);
}
#endif /* SIGN_H_ */
