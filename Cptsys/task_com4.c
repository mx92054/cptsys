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
#include "task_com4.h"
#include ".\reg\partreg.h"


/*-----------------------function define ------------------------*/
/* 	@desc: communciation task bewteen PC104 COM2 and Altimeter
		@param:		None
		@retval:	None
-----------------------------------------------------------------*/
 STATUS  Task_com4(void) 
 {
 	int 		hCom;
 	BOOL		err ;
 	USHORT	usEnable;

	if ( !xMBPortSerialInit(4, BAUDRATE_COM4, &hCom)) 
	{
		logMsg("Serial port 4 failure: Altimeter\n",0,0,0,0,0,0)	;	
		return ERROR ;
	}	
	logMsg("Serial port 4 success:  Altimeter\n",0,0,0,0,0,0)	;	
	
	FOREVER
	{
		usEnable = GetHoldingReg(ALI_ENABLE_ADR) ;
		if ( usEnable != 0 )
			{
				if ( err = readAltimeter(hCom,ALI_VAL_ADR) )
					{
						SetInputReg(ALI_ERR_CODE_ADR, err) ;
						SetInputRegInc(ALI_ERR_NUM_ADR) ;
					}						
			}	
					
		taskDelay(60)	;	
	}
 	
 	return OK; 	
 }
 
 
 /*--------------end of file-------------------------------------*/