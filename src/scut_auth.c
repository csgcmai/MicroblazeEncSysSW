/*
 * scut_auth.c
 * Main function of whole system
 *  Created on: 2012-3-11
 *      Author: Maple
 */
#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include <xil_exception.h>
#include "xil_cache.h"
#include "xintc.h"
#include "xtmrctr.h"
#include "xuartlite.h"
#include "xiic.h"
#include "xgpio.h"
#include "xiic_l.h"


#include "uart.h"
#include "timer.h"
#include "initial.h"
#include "irtp.h"
#include "gprs.h"
#include "airdetec.h"
#include "sign.h"

XIntc interrupt;
XUartLite gprsctr;
XUartLite bluetoothctr;
XTmrCtr timer;
XIic irtpctr;
XGpio airqual;
u32 temperature;
u32 airquality;

void print(char *str);


int test4iic();
u8 testbuff[100];

int main()
{
    init_platform();

    int status;
    status=ctr_init(&interrupt,&gprsctr,&bluetoothctr,
    		&timer,&irtpctr,&airqual);
    if (status == XST_SUCCESS)
    	print("Controller initialize success\n");
    else print("Controller initialize failed\n");

    status=XIic_SelfTest(&irtpctr);
    if (status == XST_SUCCESS)
        	print("irtp initialize success\n");
        else print("irtp initialize failed\n");

    status=gprs_init(&gprsctr);
    if (status == XST_SUCCESS)
    print("GPRS initialize success\n");
    else print("GPRS initialize failed\n");
    //post("sstring=Uoiu",&gprsctr);
   // sendsms("fakeyouhaha","15889937255",&gprsctr);
    //post("sstring=fvas",&gprsctr);
   // sendmail("MaiGuangcan South China University of Technology",&gprsctr);
  	//post("sstring=abcd",&gprsctr);
   // sendmail("MaiGuangcan South China University of Technology 1",&gprsctr);
   // sendsms("fakeyou1111","15889937255",&gprsctr);
    u8 alarm[50]={"10:fire,temp:"};
    int alsize; for(alsize=0;alarm[alsize];alsize++);
    u8 posthead[40]={"sstring="};
    //u8 emailmeg[100]{"The current temperature and degree of air quilty is :"};
    int posize; for(posize=0;posthead[posize];posize++);
    int a,b,c,d; u32 signin,signout[2];
    int count=0,alarmcount=0,postcount;
    while(1)
    {
    	getairqual(&airqual,&airquality);
    	gettemp(&irtpctr,&temperature);
    	a=temperature/256; b=temperature%256;
		c=(8+airquality)*16+10; d=(8+airquality)*12+airquality;

		signin=temperature*65536+c*256+d;
		rainbow_sign(signin,signout);
		xil_printf("%x %x %x\n",signin,signout[0],signout[1]);
		posthead[posize]  =a/16+'a';
		posthead[posize+1]=a%16+'a';
		posthead[posize+2]=b/16+'a';
		posthead[posize+3]=b%16+'a';
		posthead[posize+4]=c/16+'a';
		posthead[posize+5]=c%16+'a';
		posthead[posize+6]=d/16+'a';
		posthead[posize+7]=d%16+'a';

		for(postcount=8;postcount<20;postcount++)
		{
			if(postcount<16)
			{
				posthead[posize+postcount]=signout[1]%16+'a';
				signout[1]/=16;
			}
			else
			{
				posthead[posize+postcount]=signout[0]%16+'a';
				signout[0]/=16;
			}
		}



		posthead[posize+postcount]=0x00;
    	if(temperature>=4000)
    	{
    		alarmcount++;
    		if(alarmcount<5)continue;
    		alarm[alsize]=temperature/1000+'0';
    		a=temperature/256; b=temperature%256;
    		temperature=temperature/100;
    		alarm[alsize+1]=temperature%10+'0';
    		alarm[alsize+2]=0x00;
    		sendsms(alarm,"15889937255",&gprsctr);
    		post(posthead,&gprsctr);
    		sendmail(alarm,&gprsctr);
    	}
    	alarmcount=0;
    	count++;
    	if(count>50)
    	{
    		count=0;
    		post(posthead,&gprsctr);
    	}
    	delay(1000);
    }
#ifdef haha

    int a,b[2];
    //while(1);
    {
    	a=0x4957ef83;
    	rainbow_sign(a,b);
    	xil_printf("test 49: %x %x \n",b[0],b[1]);

    	a=0x00000000;
    	rainbow_sign(a,b);
    	xil_printf("test 00: %x %x \n",b[0],b[1]);

    	a=0xffffffff;
    	rainbow_sign(a,b);
    	xil_printf("test ff:%x %x \n",b[0],b[1]);

    	a=0x4957ef83;
    	rainbow_sign(a,b);
    	xil_printf("%x %x \n",b[0],b[1]);
    }
#endif
    cleanup_platform();

    return 0;
}

