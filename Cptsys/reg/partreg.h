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
#ifndef __PART_REGISTER__
#define __PART_REGISTER__

#include "vxworks.h"
#include "msgQLib.h"

/* ----------------------- Defines ------------------------------------------*/
#define INPUT_REG_ADDR 0
#define INPUT_REG_LEN 600
#define HOLD_REG_ADDR 1000
#define HOLD_REG_LEN 901
#define EVENT_HANDLE_LEN 2
#define FUNCPARA_LEN 80

/* ------------------Event handle result define -----------------------------*/
typedef enum
{
	EvOK = 0,
	EvFAIL = 1,
	EvVALID = 2
} eResult;

typedef int (*pxEventCallback)(USHORT usAddr, USHORT usVal);

typedef struct tagpxEventFunction
{
	USHORT usStartAdr;
	USHORT usEndAdr;
	pxEventCallback pxHandler;
} pxEventFunction;

typedef struct
{
	UCHAR ucChanged;  /*是否修改标志*/
	UCHAR ucModule;   /*变量所在模块号*/
	USHORT usAddress; /*变量对应的模块地址*/
	UCHAR ucType;	 /*通信协议类型 0 - Modbus  1 - DCDC */
} paramDef;

extern MSG_Q_ID regMesQ;
extern paramDef ParamsDef[];

/* ----------------------- function-----------------------------------------*/
void InitRegisterFunction();

BOOL IsValidInputAddr(USHORT usAddr, USHORT usLen);
USHORT GetInputReg(USHORT usAddr);
void SetInputReg(USHORT usAddr, USHORT val);
void SetInputRegInc(USHORT usAddr);

BOOL IsValidHoldingAddr(USHORT usAddr, USHORT usLen);
USHORT GetHoldingReg(USHORT usAddr);
void SetHoldingReg(USHORT usAddr, USHORT val);

void ExecuteEventFunction(USHORT usAddr, USHORT usVal);
int Reg1Handler(USHORT usAddr, USHORT usVal);
int Reg2Handler(USHORT usAddr, USHORT usVal);

STATUS MessageHandleTask();

/* ----------------------------------------------------------------------*/
void OrganizeData(void); /*整理数据  将获得的数据写入输入寄存器中*/
#endif
