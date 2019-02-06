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
#include "task_com1.h"
#include ".\reg\partreg.h"


/*-----------------------function define ------------------------*/
/* 	@desc: communciation task bewteen PC104 COM2 and insulation detection
		@param:		None
		@retval:	None
-----------------------------------------------------------------*/
STATUS  Task_com1(void) 
	{
	int 		hCom;
	BOOL		err ;
	USHORT	usEnable;

	if ( !xMBPortSerialInit(1, BAUDRATE_COM1, &hCom)) 
	{
		logMsg("Serial port 1 failure: Insulation detection\n",0,0,0,0,0,0)	;	
		return ERROR ;
	}	
	logMsg("Serial port 1 success: Insulation detection\n",0,0,0,0,0,0)	;	

	FOREVER
	{
		usEnable = GetHoldingReg(INS_ENABLE_ADR) ;
		if ( usEnable != 0 )
			{
				if ( err = readInsulation(hCom,INS_VAL_ADR) )
					{
						SetInputReg(INS_ERR_CODE_ADR, err) ;
						SetInputRegInc(INS_ERR_NUM_ADR) ;
					}						
			}
		taskDelay(60)	;	
	}

	return OK; 	
	}
	 
 
 /*--------------end of file-------------------------------------*/