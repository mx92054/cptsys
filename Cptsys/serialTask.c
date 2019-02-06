/* ------------------------------------------------------------
 * Serial Task: 
 * Copyright (C) 2018 chenming<chenm.idsse.ac.cn>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * -------------------------------------------------------------*/
#include "vxworks.h"
#include <stdio.h>
#include <string.h>
#include "timers.h"
#include "time.h"
#include "record.h"
#include "ioLib.h"

#include "serialTask.h"
#include ".\reg\partreg.h"

#define NUM_BOARD 20 /* 原位系统控制板数目*/
#define MAX_COUNT 5  /* 最大通信故障尝试次数  */

#define DPT_FRAME_LEN 90
#define DPT_TIMEOUT 3

extern USHORT usHoldReg[];

/* --------------------Global Variable Define -----------------*/
USHORT usModuleStatus[NUM_BOARD]; /*communication status of per module*/

int hSerHandle[NUM_SERIAL_PORT]; /* handle of serial port*/
int nBaudrate[NUM_SERIAL_PORT] = {384, 384, 384, 1152, 1152, 1152};
int nPort2Module[NUM_SERIAL_PORT + 1] = {0, 1, 2, 5, 9, 14, 15};

/*--------------------串口号 站地址 变量长度 协议类型 内存地址-----------------*/
moduleDef boardDef[] = {
	{0, 1, 2, 0, 0}, /*0  备用*/
					 /*---------------------------------------------------------------COM0*/

	{1, 1, 2, 4, 30}, /*1  绝缘检测板 */
					  /*---------------------------------------------------------------COM1*/

	{2, 0x61, 10, 2, 0},  /*2  1#DCDC转换模块*/
	{2, 0x62, 10, 2, 10}, /*3  2#DCDC转换模块*/
	{2, 0x63, 10, 2, 20}, /*4  3#DCDC转换模块*/
						  /*---------------------------------------------------------------COM2*/

	{3, 1, 40, 1, 100}, /*5  1电机控制模块*/
	{3, 2, 40, 1, 120}, /*6  2电机控制模块*/
	{3, 3, 40, 1, 140}, /*7  3电机控制模块*/
	{3, 4, 40, 1, 160}, /*8  姿态控制模块*/
						/*---------------------------------------------------------------COM3*/

	{4, 1, 23, 1, 200}, /*9 1#DIO模块*/
	{4, 2, 23, 1, 230}, /*10 2#DIO模块*/
	{4, 3, 10, 1, 260}, /*11 温度检测模块*/
	{4, 4, 10, 1, 270}, /*12 漏水检测模块*/
	{4, 5, 10, 1, 280}, /*13 控制罐开关模块*/
						/*---------------------------------------------------------------COM4*/

	{5, 1, 120, 1, 300}, /*14 多串口板 */
						 /*---------------------------------------------------------------COM5*/
};

/*-----------------------function define ------------------------*/
/* 	@desc: communciation task bewteen PC104 and control board
		@param:		None
		@retval:	None
-----------------------------------------------------------------*/
STATUS task_board(void)
{
	int i, ser, s;
	BOOL sw1, sw2;
	eMBErrorCode err;
	struct timespec tp1, tp2;

	serialInitial();

	FOREVER
	{
		clock_gettime(CLOCK_REALTIME, &tp1);

		serialONOFF(); /*  根据指令打开或关闭相应的串口   	*/

		for (i = 0; i < 19; i++)
		{
			switch (boardDef[i].ucType)
			{
			case 1:
				Com_MBModule(boardDef[i].hCom, i, boardDef[i].nStation, 0, boardDef[i].usLength, boardDef[i].usRegAdr);
				break;
			case 2:
				Com_DCModule(boardDef[i].hCom, i, boardDef[i].nStation, boardDef[i].usRegAdr);
				break;
			case 3:
				Com_Altimeter(boardDef[i].hCom, i, boardDef[i].usRegAdr);
				break;
			case 4:
				Com_Insulation(boardDef[i].hCom, i, boardDef[i].usRegAdr);
				break;
			case 5:
				Com_Depth(boardDef[i].hCom, i, boardDef[i].usRegAdr);
				break;
			}
		}

		clock_gettime(CLOCK_REALTIME, &tp2);
		/*printf("the used time is %lds, %ldms\n",tp2.tv_sec-tp1.tv_sec,(tp2.tv_nsec - tp1.tv_nsec)/1000000);*/

		taskDelay(60);

		for (i = 0; i < FUNCPARA_LEN; i++)
		{
			if (ParamsDef[i].ucChanged)
			{
				printf("i = %d\n", i);
				s = ParamsDef[i].ucModule;
				ser = hSerHandle[boardDef[s].hCom];
				if (ser > 0 && ParamsDef[i].ucType == 0)
				{
					err = WriteHoldingRegister(ser, boardDef[s].nStation, ParamsDef[i].usAddress, (1000 + i));
					printf("MODUBS Reg: BD:%d, Station:%d, Addr:%d, val: %d\n", s, boardDef[s].nStation, ParamsDef[i].usAddress, GetHoldingReg(1000 + i));
					if (err == MB_ENOERR)
						ParamsDef[i].ucChanged = 0;
				}
				if (ser > 0 && ParamsDef[i].ucType == 1)
				{
					if (i % 2 == 0)
					{
						sw1 = GetHoldingReg(1000 + i);
						sw2 = GetHoldingReg(1001 + i);
					}
					else
					{
						sw1 = GetHoldingReg(999 + i);
						sw2 = GetHoldingReg(1000 + i);
					}
					err = writeDCDCBoard(ser, boardDef[s].nStation, sw1, sw2);
					printf("DCDC Reg: BD:%d, Station:%d, sw1:%d, sw2: %d\n", s, boardDef[s].nStation, sw1, sw2);
					if (err == MB_ENOERR)
						ParamsDef[i].ucChanged = 0;
				}
			}
		}
	}

	return OK;
}

/*---------------------------------------------------------------------
@desc: -communicate with modbus module
@param: nSer -  serial port
				nModule - module number
				nStat - station address
				nStartAdr - start address of module
				nLen - register length 
				RegAdr - register address
@retval:	None
------------------------------------------------------------------------*/
void Com_MBModule(int nSer, USHORT nModule, USHORT nStat, USHORT nStartAdr, USHORT nLen, USHORT RegAdr)
{
	eMBErrorCode err;

	if (hSerHandle[nSer] > 0 && usHoldReg[110 + nModule]) /*&& usModuleStatus[nModule] < MAX_COUNT)		*/
	{
		/*			printf("Module:%d Station:%d MODBUS \n", nModule, nStat) ;*/
		if (err = ReadHoldingRegister(hSerHandle[nSer], nStat, nStartAdr, nLen, RegAdr))
		{
			SetInputReg(MODULE_STATUS_ADDR + nModule, err);
			usModuleStatus[nModule]++;
			SetInputRegInc(MODULE_COMM_ERR + nModule);
		}
		else
		{
			usModuleStatus[nModule] = 0;
		}
	}
}

/*---------------------------------------------------------------------
@desc: communicate with DC module
@param: nSer -  serial port
				nModule - module number
				nStat - station address
				RegAdr - register address
@retval:	None
------------------------------------------------------------------------*/
void Com_DCModule(int nSer, USHORT nModule, USHORT nStat, USHORT RegAdr)
{
	eMBErrorCode err;

	if (hSerHandle[nSer] > 0 && usHoldReg[110 + nModule]) /*&& usModuleStatus[nModule] < MAX_COUNT)	*/
	{
		/*			printf("Module:%d Station:0x%X DCDC \n", nModule, nStat) ;*/
		if (err = readDCDCBoard(hSerHandle[nSer], nStat, RegAdr))
		{
			SetInputReg(MODULE_STATUS_ADDR + nModule, err);
			usModuleStatus[nModule]++;
			SetInputRegInc(MODULE_COMM_ERR + nModule);
		}
		else
			usModuleStatus[nModule] = 0;
	}
}

/*---------------------------------------------------------------------
@desc: communicate with Altimeter
@param: nSer -  serial port
				nModule - module number
				RegAdr - register address
@retval:	None
------------------------------------------------------------------------*/
void Com_Altimeter(int nSer, USHORT nModule, USHORT RegAdr)
{
	eMBErrorCode err;

	if (hSerHandle[nSer] > 0 && usHoldReg[110 + nModule]) /*&& usModuleStatus[nModule] < MAX_COUNT)*/
	{
		if (err = readAltimeter(hSerHandle[nSer], RegAdr))
		{
			SetHoldingReg(MODULE_STATUS_ADDR + nModule, err);
			usModuleStatus[nModule]++;
			SetInputRegInc(MODULE_COMM_ERR + nModule);
		}
		else
			usModuleStatus[nModule] = 0;
	}
}

/*---------------------------------------------------------------------
@desc: communicate with Com_Insulation
@param: nSer -  serial port
				nModule - module number
				RegAdr - register address
@retval:	None
------------------------------------------------------------------------*/
void Com_Insulation(int nSer, USHORT nModule, USHORT RegAdr)
{
	eMBErrorCode err;

	if (hSerHandle[nSer] > 0 && usHoldReg[110 + nModule]) /*&& usModuleStatus[nModule] < MAX_COUNT)*/
	{
		if (err = readInsulation(hSerHandle[nSer], RegAdr))
		{
			SetInputReg(MODULE_STATUS_ADDR + nModule, err);
			usModuleStatus[nModule]++;
			SetInputRegInc(MODULE_COMM_ERR + nModule);
		}
		else
			usModuleStatus[nModule] = 0;
	}
}

/*---------------------------------------------------------------------
@desc: communicate with Depth Meter
@param: nSer -  serial port
				nModule - module number
				RegAdr - register address
@retval:	None
------------------------------------------------------------------------*/
void Com_Depth(int nSer, USHORT nModule, USHORT RegAdr)
{
	eMBErrorCode err;

	if (hSerHandle[nSer] > 0 && usHoldReg[110 + nModule]) /*&& usModuleStatus[nModule] < MAX_COUNT)*/
	{
		if (err = readDepthMeter(hSerHandle[nSer], RegAdr))
		{
			SetInputReg(MODULE_STATUS_ADDR + nModule, err);
			usModuleStatus[nModule]++;
			SetInputRegInc(MODULE_COMM_ERR + nModule);
		}
		else
			usModuleStatus[nModule] = 0;
	}
}
/*-------------------------------------------------------------------------------------
   function:  serialInitial
   parameter: none   
   -------------------------------------------------------------------------------------------*/
void serialInitial(void)
{
	int i;

	for (i = 0; i < NUM_SERIAL_PORT; i++)
	{
		SetHoldingReg(SERIAL_START_ADDRESS + i, 0);
		SetHoldingReg(BAUDRATE_START_ADDRESS + i, nBaudrate[i]);
		hSerHandle[i] = -1;
	}

	for (i = 0; i < 20; i++)
		usHoldReg[110 + i] = 1;
}

/* -------------------------------------------------------------------------------------------
   function:  serialClose
   parameter: none
   
   -------------------------------------------------------------------------------------------*/
void serialClose(void)
{
	int i;

	for (i = 0; i < NUM_SERIAL_PORT; i++)
	{
		vMBPortClose(hSerHandle[i]);
		hSerHandle[i] = -1;
	}
}

/* -------------------------------------------------------------------------------------------
   @desc:  	open or close based on the value of channel0 - channel9
   @parm:	 	None
   @retval:	0 if success 1 if any error hanppened  
   -------------------------------------------------------------------------------------------*/
BOOL serialONOFF(void)
{
	int i, j;
	BOOL err;
	long usBaudrate;
	USHORT usPortStatus;

	err = TRUE;
	for (i = 0; i < NUM_SERIAL_PORT; i++)
	{
		usBaudrate = 100 * GetHoldingReg(BAUDRATE_START_ADDRESS + i);
		usPortStatus = GetHoldingReg(SERIAL_START_ADDRESS + i);

		if (usPortStatus != 0 && hSerHandle[i] == -1)
		{
			if (!xMBPortSerialInit(i, usBaudrate, &hSerHandle[i]))
			{
				logMsg("serialTask_switch: open serial port: %d failure!\n", i, 0, 0, 0, 0, 0);
				err = FALSE;
			}
			logMsg("serialTask_switch: open serial port: %d success!\n", i, 0, 0, 0, 0, 0);

			j = nPort2Module[i];
			do
			{
				usModuleStatus[j++] = 0;
			} while (j < nPort2Module[i + 1]);
		}
		if (usPortStatus == 0 && hSerHandle[i] != -1)
		{
			vMBPortClose(hSerHandle[i]);
			hSerHandle[i] = -1;
			logMsg("serialTask_switch: close serial port: %d success!\n", i, 0, 0, 0, 0, 0);
		}
	}
	return err;
}

/* ----------------------------------------------------------------------------------*/
int serialSwitch(USHORT nAddr, USHORT nVal) /*  open or close based on register */
{
	USHORT usBaudrate;
	int n = nAddr - SERIAL_START_ADDRESS;

	if (n >= 0 && n < NUM_SERIAL_PORT)
	{
		if (nVal != 0 && hSerHandle[n] == -1)
		{
			usBaudrate = 100 * GetHoldingReg(BAUDRATE_START_ADDRESS + n);
			if (!xMBPortSerialInit(n, usBaudrate, &hSerHandle[n]))
			{
				logMsg("serialTask_switch: open serial port: %d failure!\n", n, 0, 0, 0, 0, 0);
				return 1;
			}
			logMsg("serialTask_switch: open serial port: %d success!\n", n, 0, 0, 0, 0, 0);
		}
		if (nVal == 0 && hSerHandle[n] != -1)
		{
			vMBPortClose(hSerHandle[n]);
			hSerHandle[n] = -1;
			logMsg("serialTask_switch: close serial port: %d success!\n", n, 0, 0, 0, 0, 0);
		}
	}
	else
		return 2;

	return 0;
}

int serialTest(int n)
{
	int i, com;

	printf("Start serial %n Test\n", n);
	if (!xMBPortSerialInit(n, 115200, &com))
	{
		printf("serialTask_switch: open serial port: %d failure!\n", n);
		return 1;
	}
	for (i = 0; i < 10; i++)
	{
		prvbMBPortSerialWrite(com, "Test serial\n", 12);
		printf("Test serial\n");
		taskDelay(100);
	}
	return 0;
}

/* -------------------------------------------------------------------------------------------
   function:  readDCDCBoard
   in parameter: 		hCom
   out parameter:		pBuffer
   if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode readDCDCBoard(int hCom, UCHAR nStat, USHORT nAddr)
{
	USHORT i, usCounter, usBytesCur, usBytesLen, usNBytesRead;
	UCHAR frRxBuf[40];
	UCHAR frTxBuf[] = {0xFF, 0xFF, 0xA5, 0x61, 0x01, 0x05, 0x67, 0x26};

	frTxBuf[3] = nStat;
	frTxBuf[6] = frTxBuf[3] + frTxBuf[4] + frTxBuf[5];

	if (!prvbMBPortSerialWrite(hCom, frTxBuf, 8))
	{
		/*logMsg("readDCDCBorad: write command error!\n",0, 0, 0, 0, 0, 0) ;*/
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usBytesLen = 27;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(3);
		prvbMBPortSerialRead(hCom, &frRxBuf[usBytesCur], usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while ((usBytesCur < 27) && (usCounter++ < 3));

	if (usBytesCur != 27)
	{
		/*printf("readDCDCBorad: receive error:%d/27\n", usBytesCur) ;
			for(i = 0 ; i < usBytesCur ; i++)
				{
					printf("%02X ", frRxBuf[i]) ;
					if ( i % 10 == 9) printf("\n") ;
				}
			printf("\n") ;*/
		return MB_ERECEIVE;
	}

	/* ----------------------handle frame----------------------------------*/
	for (usCounter = 0; usCounter < 9; usCounter++)
	{
		i = (USHORT)(frRxBuf[5 + 2 * usCounter] << 8 | frRxBuf[6 + 2 * usCounter]);
		SetInputReg(nAddr + usCounter, i);
	}

	return MB_ENOERR;
}

/* -------------------------------------------------------------------------------------------
   function:  writeDCDCBoard
   in parameter: 		hCom,nStat,SW1,SW2
   if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode writeDCDCBoard(int hCom, UCHAR nStat, BOOL sw1, BOOL sw2)
{
	USHORT i, usCounter, usBytesCur, usBytesLen, usNBytesRead;
	UCHAR frRxBuf[40];
	UCHAR frTxBuf[] = {0xFF, 0xFF, 0xA5, 0x61, 0x02, 0x06, 0x11, 0xFF, 0x26};

	frTxBuf[3] = nStat;
	frTxBuf[6] = ((sw1) ? 0x10 : 0x00) | ((sw2) ? 0x01 : 0x00);
	frTxBuf[7] = frTxBuf[3] + frTxBuf[4] + frTxBuf[5] + frTxBuf[6];

	if (!prvbMBPortSerialWrite(hCom, frTxBuf, 9))
	{
		/*logMsg("writeDCDCBorad: write command error!\n",0, 0, 0, 0, 0, 0) ;*/
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usBytesLen = 8;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(3);
		prvbMBPortSerialRead(hCom, &frRxBuf[usBytesCur], usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while ((usBytesCur < 8) && (usCounter++ < 3));

	if (usBytesCur != 8)
	{
		/*printf("writeDCDCBorad: receive error:%d/30\n", usBytesCur) ;
			for(i = 0 ; i < usBytesCur ; i++)
				{
					printf("%02X ", frRxBuf[i]) ;
					if ( i % 10 == 9) printf("\n") ;
				}
			printf("\n") ;*/
		return MB_ERECEIVE;
	}

	return MB_ENOERR;
}

/* -------------------------------------------------------------------------------------------
   function:  高度计通信处理函数
   @param: 		hCom - 串口句柄
   						pHighInteger - 高度参数整数部分
   						pHighDecimal - 高度计小数部分
   @retval:  if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode readAltimeter(int hCom, USHORT nAddr)
{
	USHORT nVal, nPos, n;
	UCHAR frTxBuf[2];
	UCHAR frRxBuf[20];
	USHORT usBytesLen, usBytesCur, usCounter, usNBytesRead;

	frTxBuf[1] = 'Z';
	if (!prvbMBPortSerialWrite(hCom, frTxBuf, 2))
	{
		logMsg("readAltimeter: write command error!\n", 0, 0, 0, 0, 0, 0);
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usBytesLen = 9;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(3);
		prvbMBPortSerialRead(hCom, &frRxBuf[usBytesCur], usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while ((usBytesCur < 8) && (usCounter++ < 3));

	if (frRxBuf[usBytesCur - 1] != 0x0A || frRxBuf[usBytesCur - 2] != 0x0D)
	{
		return MB_ERECEIVE;
	}

	nPos = 0;
	for (nPos = 0; nPos < usBytesCur; nPos++)
	{
		if (frRxBuf[nPos] == '.')
			break;
	}

	if (nPos == (usBytesCur - 1))
		return MB_ERECEIVE;

	nVal = 0;

	for (n = 0; n < nPos; n++)
	{
		nVal = 10 * nVal + frRxBuf[n] - 0x30;
	}

	SetInputReg(nAddr, nVal);
	nVal = (frRxBuf[nPos + 1] - 0x30) * 10 + frRxBuf[nPos + 2] - 0x30;
	SetInputReg(nAddr + 1, nVal);
	return MB_ENOERR;
}

/* -------------------------------------------------------------------------------------------
   @brief:  绝缘检测板处理函数
   @param: 	hCom - 串口句柄
						nAddr - Register address
   @retval:  if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode readInsulation(int hCom, USHORT nAddr)
{
	UCHAR frRxBuf[20];
	UCHAR frBuf[10];
	UCHAR ch, i;
	USHORT usCurPos = 0;
	USHORT usCounter = 0;
	USHORT res;

	/*------------------Receive Frame-----------------------------------------*/
	while (usCounter++ < 40)
	{
		if (!prvbMBPortSerialRead(hCom, &frBuf[0], 1, &res))
		{
			printf("Instulation: read error! %d,  %d |0x%X|\n", usCounter, res, frBuf[0]);
			return MB_ERECEIVE;
		}
		frRxBuf[usCurPos++] = frBuf[0];

		if (usCurPos == 3)
		{
			if (frRxBuf[0] != 0xFF || frRxBuf[1] != 0xFF || frRxBuf[2] != 0xA5)
			{
				usCurPos = 0;
				continue;
			}
		}

		if (usCurPos >= 10)
			break;
	}

	SetInputReg(nAddr, frRxBuf[4] << 8 | frRxBuf[5]);
	SetInputReg(nAddr + 1, frRxBuf[6] << 8 | frRxBuf[7]);

	/*	printf("Instulation Frame:") ;
	for(i = 0 ; i < usCurPos ; i++)
		{
			printf("%02X ", frRxBuf[i]) ;
		}
	printf("\n") ;		*/

	return MB_ENOERR;
}

/* -------------------------------------------------------------------------------------------
   @brief:  读深度计数据
   @param: 	hCom - 串口句柄
						nAddr - Register address
   @retval:  if success return TRUE, otherwise return FALSE   
   Frame format 1: $ISDPT,-000.485,M,000.9689,B,30.81,C*28
   Frame format 2: $ISHPR,060.8,-89.5,-081.1*60
----------------------------------------------------------------------------------------------*/
eMBErrorCode readDepthMeter(int hCom, USHORT nAddr)
{
	UCHAR frRxBuf[100], *ptr;
	USHORT usBytesLen = DPT_FRAME_LEN;
	USHORT usBytesCur = 0;
	USHORT usCounter = 0;
	USHORT usNBytesRead;
	float height, pressure, tempature;

	/*------------------Receive Frame-----------------------------------------*/
	do
	{
		taskDelay(5);
		prvbMBPortSerialRead(hCom, frRxBuf + usBytesCur, usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while (usCounter++ < DPT_TIMEOUT && usBytesCur < DPT_FRAME_LEN);

	if (usCounter >= DPT_TIMEOUT)
	{
		printf("\n Depth error: %d time len: %d", usCounter, usBytesCur);
		return MB_ERECEIVE;
	}
	/*
	if ( usCounters >= DPT_TIMEOUT || usBytesCur != DPT_FRAME_LEN)
		{
			printf("\n Depth error: %d time len: %d", usCounters, usBytesCur) ;
			return MB_ERECEIVE ;
			
		}*/

	frRxBuf[DPT_FRAME_LEN] = 0;
	/* ---------------------find char $ -----------------*/
	ptr = frRxBuf;
	for (usCounter = 0; usCounter < DPT_FRAME_LEN; usCounter++)
	{
		if (*ptr == '$')
		{
			sscanf(ptr, "$ISDPT,%f,M,%f,B,%f,C", &height, &pressure, &tempature);
			SetInputReg(nAddr, (int)(height * 100.0));
			SetInputReg(nAddr + 1, (int)(pressure * 100.0));
			SetInputReg(nAddr + 2, (int)(tempature * 100.0));
			return MB_ENOERR;
		}
		ptr++;
	}

	return MB_ERECEIVE;
}
/*-------------end of file--------------------------------------------------------*/
