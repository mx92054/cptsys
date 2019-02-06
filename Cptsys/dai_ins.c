/* ------------------------------------------------------------
 * Depth_Alimeter_Inslution Module: 
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
#include <sockLib.h>
#include <ioLib.h>
#include <taskLib.h>
#include <sioLib.h>

#include "serialTask.h"
#include "dai_ins.h"
#include ".\reg\partreg.h"


/* -------------------------------------------------------------------------------------------
   function:  高度计通信处理函数
   @param: 		hCom - 串口句柄
   						pHighInteger - 高度参数整数部分
   						pHighDecimal - 高度计小数部分
   @retval:  if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode	ReadAliMeter(int hCom, USHORT nAddr)
{
	USHORT 	nVal, nPos ;
	UCHAR 	frTxBuf[2] ;
	UCHAR		frRxBuf[20] ;
	USHORT	n,usCounter, usFrameLen, usBytes ;
	
	frTxBuf[0] = 'Z' ;
	frTxBuf[1] = 0 ;
	if ( !write(hCom, frTxBuf, 2) )
		{
			logMsg("readAltimeter: write command error!\n",0, 0, 0, 0, 0, 0) ;
			return MB_ETRANSMITTER ;
		}	
	
/*------------------Receive Frame-----------------------------------------*/	
	usFrameLen = 9 ;	
	usCounter = 0 ;
	do 
	{
		taskDelay(5) ;
		ioctl(hCom, FIONREAD, &n);
		if ( n >= usFrameLen )
			{
				usBytes = read(hCom, frRxBuf, usFrameLen);
				break ;	
			}
	} while ( usCounter++ < 3 ) ;
	
	if ( usCounter >= 3 || usBytes != usFrameLen )
		return MB_ERECEIVE ;

/*------------------Data handle----------------------------------------*/		
/*	for(n = 0 ; n < usBytes ; n++)
		{
			if ( frRxBuf[n] == '.')
				break ;			
		}
	
	if ( n == (usBytes - 1) )
		return MB_ERECEIVE ;
		
	nVal = 0 ;	
	for(n = 0 ; n < nPos ; n++)
	{
		nVal = 10*nVal + frRxBuf[n] - 0x30 ;
	}	
	
	SetInputReg(nAddr,nVal) ;
	nVal = (frRxBuf[nPos+1] - 0x30)*10 + frRxBuf[nPos + 2] - 0x30 ;
	SetInputReg(nAddr + 1, nVal) ;*/
	return MB_ENOERR ;	
}

/* -------------------------------------------------------------------------------------------
   @brief:  绝缘检测板处理函数
   @param: 	hCom - 串口句柄
						nAddr - Register address
   @retval:  if success return TRUE, otherwise return FALSE   
----------------------------------------------------------------------------------------------*/
eMBErrorCode	ReadInsMeter(int hCom, USHORT nAddr)
{
	UCHAR		frRxBuf[60], *ptr ;
	USHORT  n ;
	USHORT 	usBytesCur, usCounters = 0 ;
	
/*------------------Receive Frame-----------------------------------------*/	
	while ( usCounters++ < INS_TIMEOUT )
	{
		ioctl(hCom, FIONREAD, &n);
		if ( n >= INS_FRAME_LEN )
			{
				usBytesCur = read(hCom, frRxBuf, INS_FRAME_LEN) ;
				break ;
			}
		taskDelay(3) ;
	}
	
	if ( usCounters >= INS_TIMEOUT || usBytesCur != INS_FRAME_LEN)
		return MB_ERECEIVE ;
		
	frRxBuf[INS_FRAME_LEN] = 0 ;
	
	/* ---------------------find char 0xFF 0xFF 0xA5 -----------------*/
	ptr = frRxBuf ;
	for(usCounters = 0 ; usCounters < DPT_FRAME_LEN-6 ; usCounters++ )
	{
		if ( *ptr == 0xFF && *(ptr+1) == 0xFF && *(ptr+2) == 0xA5)
			{
				SetInputReg(nAddr, *(ptr+3) << 8 | *(ptr+4)) ;
				SetInputReg(nAddr + 1, *(ptr+5) << 8 | *(ptr+6)) ;				
				return MB_ENOERR ;
			}
			ptr++ ;
	}

	return MB_ERECEIVE ;	
}


/* -------------------------------------------------------------------------------------------
   @brief:  读深度计数据
   @param: 	hCom - 串口句柄
						nAddr - Register address
   @retval:  if success return TRUE, otherwise return FALSE   
   Frame format 1: $ISDPT,-000.485,M,000.9689,B,30.81,C*28
   Frame format 2: $ISHPR,060.8,-89.5,-081.1*60
----------------------------------------------------------------------------------------------*/
eMBErrorCode ReadDptMeter(int hCom, USHORT nAddr) 
{
	UCHAR		frRxBuf[100], *ptr ;
	USHORT  n ;
	USHORT 	usBytesCur, usCounters = 0 ;
	float   height, pressure, tempature ;

	/* ---------------------receive -----------------*/
	while ( usCounters++ < DPT_TIMEOUT )
	{
		ioctl(hCom, FIONREAD, &n);
		if ( n >= DPT_FRAME_LEN )
			{
				usBytesCur = read(hCom, frRxBuf, DPT_FRAME_LEN) ;
				break ;
			}
		taskDelay(3) ;
	}
	
	if ( usCounters >= DPT_TIMEOUT || usBytesCur != DPT_FRAME_LEN)
		return MB_ERECEIVE ;
		
	frRxBuf[DPT_FRAME_LEN] = 0 ;			
	/* ---------------------find char $ -----------------*/
	ptr = frRxBuf ;
	for(usCounters = 0 ; usCounters < DPT_FRAME_LEN ; usCounters++ )
	{
		if ( *ptr == '$' )
			{
				sscanf(ptr,"$ISDPT,%f,M,%f,B,%f,C",&height, &pressure, &tempature) ;
				SetInputReg(nAddr,(int)(height*10.0)) ;
				SetInputReg(nAddr + 1,(int)(pressure*10.0)) ;
				SetInputReg(nAddr + 2,(int)(tempature*10.0)) ;
				return MB_ENOERR ;
			}
			ptr++ ;
	}

	return MB_ERECEIVE ;
}	