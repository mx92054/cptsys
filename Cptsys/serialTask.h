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
#ifndef __SERIAL_TASK__
#define __SERIAL_TASK__

#include "md_rtu\modbus_rtu.h"

#define NUM_SERIAL_PORT		6				/*系统总串口数*/

#define MAX_MODULE_NUM			20			/*装置电气设备总数*/
#define MAX_INPUT_REG_LEN		50				/*电气模块状态寄存器最大长度*/
#define MAX_HOLD_REG_LEN		50			/*电气模块保持寄存器最大长度*/

#define SERIAL_START_ADDRESS			1080		/* 串口开关起始地址  	*/
#define BAUDRATE_START_ADDRESS		1100		/* 串口波特率起始地址 */
#define MODULE_STATUS_ADDR				500			/* 模块状态起始地址 */
#define MODULE_COMM_ERR						520			/* 模块发生通信故障次数*/
/*---------------------------------------------------------------------------------*/
typedef struct
{
	int			hCom ;						/*串口地址*/
	USHORT	nStation ;				/*模块站地址*/
	USHORT 	usLength ;				/*模块寄存器长度*/
	UCHAR		ucType ;					/*模块协议类型 0-无 1-MODBUS 2-DCDC 3-Altimeter */
	USHORT	usRegAdr ;				/*寄存器保存地址*/
}  moduleDef ;

extern USHORT  	usModuleStatus[];								/*communication status of per module*/
extern int			hSerHandle[NUM_SERIAL_PORT] ;		/* handle of serial port*/

void 	serialInitial(void) ;		/* init all serial port */
void 	serialClose(void) ;			/*	close all serial port */
int   serialSwitch(USHORT nAddr, USHORT nVal ) ;				/*  open or close based on register */
int  	serialONOFF(void) ;
int 	serialTest(int n) ;

void Com_MBModule(int nSer,USHORT nModule,  USHORT nStat, USHORT nStartAdr, USHORT nLen, USHORT RegAdr) ;
void Com_DCModule(int nSer,USHORT nModule, USHORT nStat, USHORT RegAdr) ;
void Com_Altimeter(int nSer,USHORT nModule,  USHORT RegAdr) ;
void Com_Insulation(int nSer,USHORT nModule,  USHORT RegAdr) ;
void Com_Depth(int nSer,USHORT nModule,  USHORT RegAdr) ;

/* 读DCDC电源转换板			*/
eMBErrorCode	readDCDCBoard(int hCom, UCHAR nStat, USHORT nAddr) ;
eMBErrorCode	writeDCDCBoard(int hCom, UCHAR nStat, BOOL sw1, BOOL sw2) ;
eMBErrorCode	readInsulation(int hCom, USHORT nAddr) ;

/* 读高度计数据 */
eMBErrorCode	readAltimeter(int hCom, USHORT nAddr);

/* 读深度计数据 */
eMBErrorCode readDepthMeter(int hCom, USHORT nAddr) ;

STATUS task_board(void)  ;
 #endif
 /* -----------------end of file---------------------------------*/
