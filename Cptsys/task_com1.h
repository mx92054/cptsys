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
 #ifndef __TASK_COM1__
 #define __TASK_COM1__
 
 #define 	BAUDRATE_COM1   	38400
 #define  INS_ENABLE_ADR		1111
 #define  INS_VAL_ADR				30
 #define  INS_ERR_CODE_ADR	501
 #define	INS_ERR_NUM_ADR		521		
 
 STATUS  Task_com1(void) ; 
 
 
 
 #endif