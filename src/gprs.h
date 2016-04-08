/*
 * gprs.h
 *  Iintialize the gprs module and provide
 *  the function of gprs, such sms, http request
 *  and send email
 *  all this funciton just provide the text mode
 *  Created on: 2012-3-16
 *      Author: Maple
 */

#ifndef GPRS_H_
#define GPRS_H_

#include "uart.h"
#include "timer.h"
#include "Xuartlite.h"
#include "xparameters.h"


int isrecvsever();

int gprs_init(XUartLite *InstancePtr)
{
	XUartLite_ResetFifos(InstancePtr);

	//if uart communication normal
	sendcmd("ATZ\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	//exist sim card of not
	sendcmd("AT+ESIMS?\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	//if login to network
	//sendcmd("AT+COPS?\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	//query strength of network
	sendcmd("AT+CSQ\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	//auto output when received message
	//sendcmd("AT+CNMI=2,2,0,0,1\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	//setting sms mode to text
	sendcmd("AT+CMGF=1\r\n",InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;

	return XST_SUCCESS;
}

int sendsms(const u8* sms,const u8* phone, XUartLite *InstancePtr)
{
	int index,i;
	int size; for(size=0;phone[size];size++);
	u8 smscmd[30]={"AT+CMGS=\""};
	u8 smsend[]={0x1A,0x0D,0x0A,0x00};
	u8 smscont[200];
	int smssize;for(smssize=0;sms[smssize];smscont[smssize]=sms[smssize++]);
	smscont[smssize++]=0x1A;
	smscont[smssize++]=0x0D;
	smscont[smssize++]=0x0A;
	smscont[smssize]=0x00;
	XUartLite_ResetFifos(InstancePtr);
	for(index=0;smscmd[index];index++);
	for(i=0;i<size;smscmd[index++]=phone[i++]);
	smscmd[index++]=0x22;
	smscmd[index++]=0x0D;
	smscmd[index++]=0x0A;
	smscmd[index]=0;
	smsed=1;
	sendcmd(smscmd,InstancePtr);
	delay(70);
	//while(smsed);
	sendcmd(smscont,InstancePtr);
	//sendcmd(smsend,InstancePtr);
	delay(2000);
	//while(gprsstatus==0){}
	//if(gprsstatus!=1) return XST_FAILURE;
	return XST_SUCCESS;
}

int sendmail(const u8* content,XUartLite *InstancePtr)
{
	XUartLite_ResetFifos(InstancePtr);
	sendcmd("AT^CIPSTATUS\r\n",InstancePtr);
	int i,j;
	int reclose=0;
	u8 sendcont[seng_length]={"AT^CIPSEND="};
	for(i=0;sendcont[i];i++);
	for(j=0;content[j];sendcont[i++]=content[j++]);
	sendcont[i++]=0x0D;
	sendcont[i++]=0x0A;
	sendcont[i]=0;
	while(1)
	{
		for(j=0;j<recvindex-2;j++){
			if((recv[j-4]=='S')&&(recv[j-3]=='T')&&(recv[j-2]=='A')&&(recv[j-1]=='T')&&(recv[j]=='E'))
			{
				if(recv[j+2]=='0')
					goto next;

				else
					close:sendcmd("AT^CIPCLOSE\r\n",InstancePtr);
				while(gprsstatus==0){}
				if(gprsstatus==1)goto next;
				else
				{
					sendcmd("AT+CGATT=0\r\n",InstancePtr);
					while(gprsstatus==0){}
					if(gprsstatus!=1)
						goto close;
					reclose++;
					if(reclose>5)return XST_FAILURE;
				}
			}
		}
	}
	next:sendcmd("AT^CIPSTART=TELNET,221.176.9.170,25\r\n",InstancePtr);
	delay(150);  XUartLite_ResetFifos(InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}
	//say hello to server
	sendcmd("AT^CIPSENDHEX=65686C6F2068690D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}
	//tell server to login
	sendcmd("AT^CIPSENDHEX=41555448204C4F47494E0D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//user name  maigc008 encode by base64 and transfer to Hex
	sendcmd("AT^CIPSENDHEX=625746705A324D774D44673D0D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//user password  27min@$ encode by base64 and transfer to Hex
	sendcmd("AT^CIPSENDHEX=4D6A6474615735414A413D3D0D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//mail from: <maigc008@139.com>
	sendcmd("AT^CIPSENDHEX=4D41494C2046524F4D3A3C6D61696763303038403133392E636F6D3E0D0A\r\n",InstancePtr);
	//sendcmd("",InstancePtr);
	delay(200);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//RCPT TO:<maigc008@qq.com>
	sendcmd("AT^CIPSENDHEX=5243505420544F3A3C6D616967633030384071712E636F6D3E0D0A\r\n",InstancePtr);
	//sendcmd("",InstancePtr);
	delay(200);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//DATA
	sendcmd("AT^CIPSENDHEX=444154410D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//Title of email
	sendcmd("AT^CIPSEND=TO: Maple Mai\r\n",InstancePtr);
	delay(100);

	sendcmd("AT^CIPSEND=From: SCUT LAB\r\n",InstancePtr);
	delay(100);

	sendcmd("AT^CIPSEND=SUBJECT:Testing Email Function\r\n",InstancePtr);
	delay(150);

	sendcmd("AT^CIPSENDHEX=0D0A0D0A\r\n",InstancePtr);
	delay(100);

	sendcmd(sendcont,InstancePtr);
	delay(100);

	sendcmd("AT^CIPSENDHEX=0D0A2E0D0A\r\n",InstancePtr);
	delay(150);	XUartLite_ResetFifos(InstancePtr);
	while(1)
	{
		timecount=0;
		if(timecount>5000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}
	delay(1000);
	return XST_SUCCESS;
}


int post(const u8* content,XUartLite *InstancePtr)
{
	XUartLite_ResetFifos(InstancePtr);
	//sendcmd("AT+CGATT=0\r\n",InstancePtr);
	//while(gprsstatus==0){};
	//sendcmd("AT+CGATT=1\r\n",InstancePtr);
	//while(gprsstatus==0){};
	//sendcmd("AT^CIPCLOSE\r\n",InstancePtr);
	//while(gprsstatus==0){};
	sendcmd("AT^CIPSTATUS\r\n",InstancePtr);
	int size,j,i;
	int reclose=0;
	for(size=0;content[size];size++);
	u8 contlength[60]={"AT^CIPSENDHEX=436F6E74656E742D4C656E6774683A20"};
	for(i=0;contlength[i];i++);
	u8 sizestr[5]; j=0;
	while(size%10)
	{
		sizestr[j++]=size%10+'0';
		size/=10;
	}
	while(j>0)
	{
		contlength[i++]=sizestr[--j]/16+'0';
		contlength[i++]=sizestr[j]%16+'0';
	}
	contlength[i++]='0';contlength[i++]='D';
	contlength[i++]='0';contlength[i++]='A';
	contlength[i++]=0x0D; contlength[i++]=0x0A;
	contlength[i]=0;

	u8 contcmd[300]={"AT^CIPSEND="};
	for(size=0;contcmd[size];size++);
	for(i=0;content[i];contcmd[size++]=content[i++]);
	//contcmd[size++]='0';contcmd[size++]='D';
	//contcmd[size++]='0';contcmd[size++]='A';
	contcmd[size++]=0x0D;
	contcmd[size++]=0x0A;
	contcmd[size]=0;

	while(1)
	{
		for(j=0;j<recvindex-2;j++){
			if((recv[j-4]=='S')&&(recv[j-3]=='T')&&(recv[j-2]=='A')&&(recv[j-1]=='T')&&(recv[j]=='E'))
			{
				if(recv[j+2]=='0')
					goto next;

				else
					close:sendcmd("AT^CIPCLOSE\r\n",InstancePtr);
				while(gprsstatus==0){}
				if(gprsstatus==1)goto next;
				else
				{
					sendcmd("AT+CGATT=0\r\n",InstancePtr);
					while(gprsstatus==0){}
					if(gprsstatus!=1)
						goto close;
					reclose++;
					if(reclose>5)return XST_FAILURE;
				}
			}
		}
	}
	next:sendcmd("AT^CIPSTART=TCP,121.194.0.190,80\r\n",InstancePtr);
	delay(150);  XUartLite_ResetFifos(InstancePtr);
	while(gprsstatus==0){}
	if(gprsstatus!=1) return XST_FAILURE;
	while(0)
	{
		timecount=0;
		if(timecount>15000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}

	//POST /index.php/Index/addstring HTTP/1.1
	sendcmd("AT^CIPSENDHEX=504F5354202F696E6465782E7068702F496E6465782F616464737472696E6720485454502F312E310D0A\r\n",InstancePtr);
	delay(300);
	//Content-Type: application/x-www-form-urlencoded
	sendcmd("AT^CIPSENDHEX=436F6E74656E742D547970653A206170706C69636174696F6E2F782D7777772D666F726D2D75726C656E636F6465640D0A\r\n",InstancePtr);
	delay(300);
	//Host: scutiot.sinaapp.com
	sendcmd("AT^CIPSENDHEX=486F73743A2073637574696F742E73696E616170702E636F6D0D0A\r\n",InstancePtr);
	delay(300);
	//Content-Length:
	sendcmd(contlength,InstancePtr);
	//sendcmd("AT^CIPSENDHEX=0D0A\r\n",InstancePtr);
	delay(200);
	//Connection: Keep-Alive
	sendcmd("AT^CIPSENDHEX=436F6E6E656374696F6E3A204B6565702D416C6976650D0A\r\n",InstancePtr);
	delay(200);

	sendcmd("AT^CIPSENDHEX=0D0A\r\n",InstancePtr);
	delay(100);

	sendcmd(contcmd,InstancePtr);
	delay(200);

	sendcmd("\r\n",InstancePtr);
	delay(100);
	while(1)
	{
		timecount=0;
		//print("Hello\n");
		if(timecount>5000) return XST_FAILURE;
		if(isrecvsever()==XST_SUCCESS)break;
	}
	sendcmd("AT+CGATT=0\r\n",InstancePtr);
	while(gprsstatus==0){};
	return XST_SUCCESS;
}

int isrecvsever()
{
	int j;
	for(j=0;j<recvindex;j++){
		if((recv[j-5]=='C')&&(recv[j-4]=='I')&&(recv[j-3]=='P')&&(recv[j-2]=='R')&&(recv[j-1]=='E')&&(recv[j]=='C'))  return XST_SUCCESS;
	}
	return XST_FAILURE;
}



#endif /* GPRS_H_ */
