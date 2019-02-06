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
 * Lesser General Public License for more details.Fi*/
#include "vxworks.h"
#include <stdio.h>
#include <string.h>
#include "timers.h"
#include "time.h"

#include "serialTask.h"
#include "task_com3.h"
#include ".\reg\partreg.h"

/*-----------------------function define ------------------------*/
/* 	@desc: communciation task bewteen PC104 COM2 and Motor Control Module
		@param:		None
		@retval:	None
-----------------------------------------------------------------*/
 STATUS  Task_com3(void) 
 {
 	int 		hCom,i;
 	BOOL		err, adr ;

	if ( !xMBPortSerialInit(3, BAUDRATE_COM3, &hCom)) 
	{
		logMsg("Serial port 3 failure: Motor Control Module\n",0,0,0,0,0,0)	;	
		return ERROR ;
	}	
	logMsg("Serial port 3 success:  Motor Control Module\n",0,0,0,0,0,0)	;	
	
	FOREVER
	{
		if ( err = ReadHoldingRegister(hCom, 1, 0, 40, MOT_VAL_ADR) )
			{
				SetInputReg(MOT_ERR_CODE_ADR, err) ;
				SetInputRegInc(MOT_ERR_NUM_ADR) ;
			}	
			
		taskDelay(5)	;
									
		/*---------------write data---------------*/
		for(i = 0; i < 30; i++)
		{			
			if ( ParamsDef[i].ucChanged)
				{
					adr = ParamsDef[i].usAddress ;
					err = WriteHoldingRegister(hCom, 1, adr, 1000 + i) ;
					/*printf(" Com3 Modbus stat:%d, adr:%d, val:%d\n", n,adr,val) ;*/
					if ( err == MB_ENOERR )
						ParamsDef[i].ucChanged = 0 ;	
				}
		}		
 			
		taskDelay(10)	;	

	}
 	
 	return OK; 	
 }
 
 
 /*--------------end of file-------------------------------------*/