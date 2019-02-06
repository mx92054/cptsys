/* ------------------------------------------------------------
 * Serial Task COM2: 
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
 * Created on Nov,7 2018
 * -------------------------------------------------------------*/
#include "vxworks.h"
#include <stdio.h>
#include <string.h>
#include "timers.h"
#include "time.h"

#include "serialTask.h"
#include "task_com2.h"
#include ".\reg\partreg.h"


/*-----------------------function define ------------------------*/
/* 	@desc: communciation task bewteen PC104 COM2 and DCDC Module
		@param:		None
		@retval:	None
-----------------------------------------------------------------*/
 STATUS  Task_com2(void) 
 {
 	int 		hCom,i;
 	BOOL		err ;
 	USHORT	usEnable[3], n, adr, ins;
 	BOOL		sw1, sw2;

	if ( !xMBPortSerialInit(2, BAUDRATE_COM2, &hCom)) 
	{
		logMsg("Serial port 2 failure: DC-DC Module\n",0,0,0,0,0,0)	;	
		return ERROR ;
	}	
	logMsg("Serial port 2 success:  DC-DC Module\n",0,0,0,0,0,0)	;	
	
	FOREVER
	{
		for(i = 0 ; i < 3 ; i++)
		{
			usEnable[i] = GetHoldingReg(DCDC1_ENABLE_ADR + i) ;
			if ( usEnable[i] != 0 )
				{
					if ( err = readDCDCBoard(hCom,DCDC1_ADDRESS + i,DCDC1_VAL_ADR + i*10) )
						{
							SetInputReg(DCDC1_ERR_CODE_ADR + i, err) ;
							SetInputRegInc(DCDC1_ERR_NUM_ADR + i) ;
						}						
				}			
		}
		
		/*---------------write data---------------*/
		for(i = 30; i < 36; i++)
		{
			ins = ParamsDef[i].ucModule - 2 ;
			if ( ParamsDef[i].ucChanged && usEnable[ins])
				{
					adr = ParamsDef[i].usAddress ;
					if (adr % 2 == 0)
						{
							sw1 = GetHoldingReg(1000 + i) ;
							sw2 = GetHoldingReg(1001 + i) ;
						}
					else
						{
							sw1 = GetHoldingReg(999 + i) ;
							sw2 = GetHoldingReg(1000 + i) ;
						}
					
					n = DCDC1_ADDRESS + ParamsDef[i].ucModule - 2 ;
					err = writeDCDCBoard(hCom,n,sw1,sw2) ;
					printf(" No:0x%x DCDC Board, sw1:%d, sw2: %d\n", n,sw1,sw2) ;
					if ( err == MB_ENOERR )
						ParamsDef[i].ucChanged = 0 ;	
				}
		}
		
		taskDelay(60)	;	
	}
 	
 	return OK; 	
 }
 
 
 /*--------------end of file-------------------------------------*/