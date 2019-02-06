/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mb.c,v 1.28 2018/05/18 13:54:40 chenming Exp $
 */

#include "vxworks.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "..\reg\partreg.h"

/* ----------------------- Defines ------------------------------------------*/
#define PROG _T("freemodbus")

STATUS ModbusTask();

/* -------------------------------------------------------------------------*/
STATUS ModbusChain()
{
	eMBErrorCode eResult;

	eResult = eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);

	if (eResult != MB_ENOERR)
	{
		logMsg("ModbusChain: Socket Initiailze error(%d)", eResult, 0, 0, 0, 0, 0);
		return (ERROR);
	}

	eResult = eMBEnable();

	if (eResult != MB_ENOERR)
	{
		logMsg("ModbusChain: Enable error(%d)", eResult, 0, 0, 0, 0, 0);
		return (ERROR);
	}

	taskSpawn("mbPoll", 110, 0, 2048,
			  (FUNCPTR)ModbusTask, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	return (OK);
}

/* -------------------------------------------------------------------------*/
STATUS ModbusTask()
{
	FOREVER
	{
		if (eMBPoll() != MB_ENOERR)
			break;
		taskDelay(5);
	}

	logMsg("ModbusTask: Modbus procotol chain exit.\n", 0, 0, 0, 0, 0, 0);
	return (ERROR);
}

/* -------------------------------------------------------------------------*/
eMBErrorCode
eMBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
	USHORT val;
	if (!IsValidInputAddr(usAddress, usNRegs))
		return MB_ENOREG;

	while (usNRegs > 0)
	{
		val = GetInputReg(usAddress++);
		*pucRegBuffer++ = (unsigned char)(val >> 8);
		*pucRegBuffer++ = (unsigned char)(val & 0xFF);
		usNRegs--;
	}
	return MB_ENOERR;
}

/* -------------------------------------------------------------------------*/
eMBErrorCode
eMBRegInputSetting(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
	USHORT val;

	if (!IsValidInputAddr(usAddress, usNRegs))
		return MB_ENOREG;

	while (usNRegs > 0)
	{
		val = *pucRegBuffer++ << 8;
		val |= *pucRegBuffer++;
		SetInputReg(usAddress++, val);
		usNRegs--;
	}

	return MB_ENOERR;
}

/* -------------------------------------------------------------------------*/
eMBErrorCode
eMBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
	USHORT val;

	if (!IsValidHoldingAddr(usAddress, usNRegs))
		return MB_ENOREG;

	switch (eMode)
	{
	case MB_REG_READ:
		while (usNRegs > 0)
		{
			val = GetHoldingReg(usAddress++);
			*pucRegBuffer++ = (unsigned char)(val >> 8);
			*pucRegBuffer++ = (unsigned char)(val & 0xFF);
			usNRegs--;
		}
		break;
	case MB_REG_WRITE:
		while (usNRegs > 0)
		{
			val = *pucRegBuffer++ << 8;
			val |= *pucRegBuffer++;
			SetHoldingReg(usAddress++, val);
			usNRegs--;
		}
		break;
	}
	return MB_ENOERR;
}

/* -------------------------------------------------------------------------*/
eMBErrorCode
eMBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
	return MB_ENOREG;
}

/* -------------------------------------------------------------------------*/
eMBErrorCode
eMBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
	return MB_ENOREG;
}

/* -------------------------------------------------------------------------*/
void vMBPortTimersDelay(USHORT usTimeOutMS)
{
	taskDelay(usTimeOutMS / 10);
}
