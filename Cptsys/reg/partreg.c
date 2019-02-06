/* ------------------------------------------------------------
 * input and holding register mangment: 
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
#include "semLib.h"
#include "partreg.h"
#include "..\serialTask.h"

#define MAX_MSGS (10)
#define MAX_MSG_LEN (100)
/* ----------------------- Static variables ---------------------------------*/
USHORT usInputRegAdr = INPUT_REG_ADDR;
USHORT usInputReg[INPUT_REG_LEN];

USHORT usHoldRegAdr = HOLD_REG_ADDR;
USHORT usHoldReg[HOLD_REG_LEN];

pxEventFunction pxHandleTbl[100];
USHORT pxHandleTblLen = 2;

/*-----------------------------------------------------------------------------*/
paramDef ParamsDef[] = {
	/*1#贯入正传      1#贯入反转         2#贯入正传       2#贯入反转       油泵运行  */
	{0, 5, 10, 0},
	{0, 5, 11, 0},
	{0, 5, 12, 0},
	{0, 5, 13, 0},
	{0, 5, 14, 0}, /*1000 - 1009*/
	/* 1#姿态运行     2#姿态运行        1#贯入额定电流    1#贯入动作时间    1#贯入速断倍数  */
	{0, 5, 60, 0},
	{0, 8, 61, 0},
	{0, 5, 62, 0},
	{0, 5, 63, 0},
	{0, 5, 64, 0},
	/*2#贯入额定电流  2#贯入动作时间    2#贯入速断倍数    油泵额定电流      油泵动作时间 */
	{0, 5, 65, 0},
	{0, 5, 66, 0},
	{0, 5, 67, 0},
	{0, 5, 68, 0},
	{0, 5, 69, 0}, /*1010 - 1019*/
	/*油泵速断倍数    1#姿态过流设定    2#姿态过流设定    姿态手自动         1#姿态速断设定*/
	{0, 5, 70, 0},
	{0, 5, 71, 0},
	{0, 5, 72, 0},
	{0, 5, 73, 0},
	{0, 5, 74, 0},
	/*1#姿态方向设定  2#姿态速断设定    2#姿态方向设定     姿态设定开关     PITCH */
	{0, 5, 75, 0},
	{0, 5, 76, 0},
	{0, 5, 77, 0},
	{0, 5, 78, 0},
	{0, 5, 79, 0}, /*1020 - 1029*/
	/*YAW             姿态P							姿态I							姿态D             姿态输出限幅*/
	{0, 5, 80, 0},
	{0, 5, 81, 0},
	{0, 5, 82, 0},
	{0, 5, 83, 0},
	{0, 5, 84, 0},

	{0, 2, 0, 1},
	{0, 2, 1, 1},
	{0, 3, 0, 1},
	{0, 3, 1, 1},
	{0, 4, 0, 1}, /*1030 - 1039*/
	{0, 4, 1, 1},
	{0, 13, 0, 0},
	{0, 13, 1, 0},
	{0, 13, 2, 0},
	{0, 13, 3, 0},

	{0, 13, 4, 0},
	{0, 13, 5, 0},
	{0, 13, 6, 0},
	{0, 13, 7, 0},
	{0, 9, 0, 0}, /*1040 - 1049*/
	{0, 9, 1, 0},
	{0, 9, 2, 0},
	{0, 9, 3, 0},
	{0, 9, 4, 0},
	{0, 9, 5, 0},

	{0, 9, 6, 0},
	{0, 9, 7, 0},
	{0, 10, 0, 0},
	{0, 10, 1, 0},
	{0, 10, 2, 0}, /*1050 - 1059*/
	{0, 10, 3, 0},
	{0, 10, 4, 0},
	{0, 10, 5, 0},
	{0, 10, 6, 0},
	{0, 10, 7, 0},
	/*剪切装置命令设置*/
	{0, 14, 100, 0},
	{0, 14, 101, 0},
	{0, 14, 102, 0},
	{0, 14, 103, 0},
	{0, 14, 104, 0}, /*1060 - 1069*/
	{0, 14, 105, 0},
	{0, 14, 106, 0},
	{0, 14, 107, 0},
	{0, 14, 108, 0},
	{0, 14, 109, 0},

	{0, 14, 110, 0},
	{0, 14, 111, 0},
	{0, 14, 112, 0},
	{0, 14, 113, 0},
	{0, 14, 114, 0}, /*1070 - 1079*/
	{0, 14, 115, 0},
	{0, 14, 116, 0},
	{0, 14, 117, 0},
	{0, 14, 118, 0},
	{0, 14, 119, 0}};

MSG_Q_ID regMesQ;

/* ----------------------- function-----------------------------------------*/
/* Initialize register function  																						*/
void InitRegisterFunction()
{
	/* ------- define function 1-------------------*/
	pxHandleTbl[0].usStartAdr = SERIAL_START_ADDRESS;
	pxHandleTbl[0].usEndAdr = SERIAL_START_ADDRESS + NUM_SERIAL_PORT - 1;
	pxHandleTbl[0].pxHandler = serialSwitch;

	/* ------- define function 2-------------------*/
	pxHandleTbl[1].usStartAdr = 1010;
	pxHandleTbl[1].usEndAdr = 1019;
	pxHandleTbl[1].pxHandler = Reg2Handler;

	pxHandleTblLen = 2;

	regMesQ = msgQCreate(MAX_MSGS, MAX_MSG_LEN, MSG_Q_PRIORITY);
}

int Reg1Handler(USHORT usAddr, USHORT usVal)
{
	printf("Reg1 Handler: %d - %d\n", usAddr, usVal);
	return EvOK;
}

int Reg2Handler(USHORT usAddr, USHORT usVal)
{
	printf("Reg2 Handler: %d - %d\n", usAddr, usVal);
	return EvOK;
}

/* ----------------------- function-----------------------------------------*/
/* execute event handler  */
void ExecuteEventFunction(USHORT usAddr, USHORT usVal)
{
	int i;

	for (i = 0; i < pxHandleTblLen; i++)
	{
		if (usAddr >= pxHandleTbl[i].usStartAdr && usAddr <= pxHandleTbl[i].usEndAdr)
			pxHandleTbl[i].pxHandler(usAddr, usVal);
	}
}

/* ----------------------- function-----------------------------------------*/
/* message handle task  */
STATUS MessageHandleTask()
{
	USHORT usAddr, usVal;
	char msgBuf[MAX_MSG_LEN];

	FOREVER
	{
		/* get message from queue; if necessary wait until msg is available */
		if (msgQReceive(regMesQ, msgBuf, 4, WAIT_FOREVER) == ERROR)
			return (ERROR);

		usAddr = (USHORT)(msgBuf[0] << 8);
		usAddr |= (USHORT)(msgBuf[1] & 0x00FF);
		usVal = (USHORT)(msgBuf[2] << 8);
		usVal |= (USHORT)(msgBuf[3] & 0x00FF);

		/* display message */
		ExecuteEventFunction(usAddr, usVal);
	}

	return OK;
}

/* ----------------------- function-----------------------------------------*/
/* Determine if the address is valid  */
BOOL IsValidInputAddr(USHORT usAddr, USHORT usLen)
{
	if (usAddr >= usInputRegAdr && (usAddr + usLen) < (usInputRegAdr + INPUT_REG_LEN))
		return TRUE;
	else
		return FALSE;
}

/* Obtain input register value  */
USHORT GetInputReg(USHORT usAddr)
{
	if (usAddr >= usInputRegAdr && usAddr < (usInputRegAdr + INPUT_REG_LEN))
		return usInputReg[usAddr - usInputRegAdr];
	else
		return 0;
}

/*  Setting input register value  */
void SetInputReg(USHORT usAddr, USHORT val)
{
	if (usAddr >= usInputRegAdr && usAddr < (usInputRegAdr + INPUT_REG_LEN))
		usInputReg[usAddr - usInputRegAdr] = val;
}

/*  Increase input register value  */
void SetInputRegInc(USHORT usAddr)
{
	if (usAddr >= usInputRegAdr && usAddr < (usInputRegAdr + INPUT_REG_LEN))
		usInputReg[usAddr - usInputRegAdr]++;
}

/* ----------------------- function-----------------------------------------*/
/* Determine if the address is valid  */
BOOL IsValidHoldingAddr(USHORT usAddr, USHORT usLen)
{
	if (usAddr >= usHoldRegAdr && (usAddr + usLen) < (usHoldRegAdr + HOLD_REG_LEN))
		return TRUE;
	else
		return FALSE;
}

/* ----------------------- function-----------------------------------------*/
/* Obtain holding register value  */
USHORT GetHoldingReg(USHORT usAddr)
{
	if (usAddr >= usHoldRegAdr && usAddr < (usHoldRegAdr + HOLD_REG_LEN))
		return usHoldReg[usAddr - usHoldRegAdr];
	else
		return 0;
}

/* ----------------------- function-----------------------------------------*/
/* Setting holding register value  */
void SetHoldingReg(USHORT usAddr, USHORT val)
{
	USHORT no;

	no = usAddr - usHoldRegAdr;
	if (usAddr >= usHoldRegAdr && no < HOLD_REG_LEN)
	{
		usHoldReg[no] = val;
		if (no < FUNCPARA_LEN)
		{
			/* printf("no = %d\n", no); */
			ParamsDef[no].ucChanged = 1;
		}
	}
}

/* ---------------------------------------------------------------------------*/
/* 整理数据  将获得的数据写入输入寄存器中 */
void OrganizeData(void)
{
	int i;

	/* 电机控制板1*/
	for (i = 0; i < 3; i++)
		usInputReg[i] = usHoldReg[240 + i];
	usInputReg[3] = 0;
	if (usHoldReg[252] == 1)
		usInputReg[3] = 1;
	if (usHoldReg[253] == 1)
		usInputReg[3] = 2;
	usInputReg[4] = 0;
	if (usHoldReg[248] == 1)
		usInputReg[4] = 1;
	if (usHoldReg[248] == 1)
		usInputReg[4] += 2;

	/* 电机控制板2*/
	for (i = 0; i < 3; i++)
		usInputReg[5 + i] = usHoldReg[260 + i];
	usInputReg[8] = 0;
	if (usHoldReg[272] == 1)
		usInputReg[8] = 1;
	if (usHoldReg[273] == 1)
		usInputReg[8] = 2;
	usInputReg[9] = 0;
	if (usHoldReg[268] == 1)
		usInputReg[9] = 1;
	if (usHoldReg[268] == 1)
		usInputReg[9] += 2;

	/* 电机控制板3*/
	for (i = 0; i < 3; i++)
		usInputReg[10 + i] = usHoldReg[280 + i];
	usInputReg[13] = 0;
	if (usHoldReg[292] == 1)
		usInputReg[13] = 1;
	if (usHoldReg[293] == 1)
		usInputReg[13] = 2;
	usInputReg[14] = 0;
	if (usHoldReg[288] == 1)
		usInputReg[14] = 1;
	if (usHoldReg[288] == 1)
		usInputReg[14] += 2;
}
