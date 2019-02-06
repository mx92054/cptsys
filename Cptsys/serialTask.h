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

#define NUM_SERIAL_PORT		6				/*ϵͳ�ܴ�����*/

#define MAX_MODULE_NUM			20			/*װ�õ����豸����*/
#define MAX_INPUT_REG_LEN		50				/*����ģ��״̬�Ĵ�����󳤶�*/
#define MAX_HOLD_REG_LEN		50			/*����ģ�鱣�ּĴ�����󳤶�*/

#define SERIAL_START_ADDRESS			1080		/* ���ڿ�����ʼ��ַ  	*/
#define BAUDRATE_START_ADDRESS		1100		/* ���ڲ�������ʼ��ַ */
#define MODULE_STATUS_ADDR				500			/* ģ��״̬��ʼ��ַ */
#define MODULE_COMM_ERR						520			/* ģ�鷢��ͨ�Ź��ϴ���*/
/*---------------------------------------------------------------------------------*/
typedef struct
{
	int			hCom ;						/*���ڵ�ַ*/
	USHORT	nStation ;				/*ģ��վ��ַ*/
	USHORT 	usLength ;				/*ģ��Ĵ�������*/
	UCHAR		ucType ;					/*ģ��Э������ 0-�� 1-MODBUS 2-DCDC 3-Altimeter */
	USHORT	usRegAdr ;				/*�Ĵ��������ַ*/
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

/* ��DCDC��Դת����			*/
eMBErrorCode	readDCDCBoard(int hCom, UCHAR nStat, USHORT nAddr) ;
eMBErrorCode	writeDCDCBoard(int hCom, UCHAR nStat, BOOL sw1, BOOL sw2) ;
eMBErrorCode	readInsulation(int hCom, USHORT nAddr) ;

/* ���߶ȼ����� */
eMBErrorCode	readAltimeter(int hCom, USHORT nAddr);

/* ����ȼ����� */
eMBErrorCode readDepthMeter(int hCom, USHORT nAddr) ;

STATUS task_board(void)  ;
 #endif
 /* -----------------end of file---------------------------------*/
