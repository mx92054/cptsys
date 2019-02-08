/* ------------------------------------------------------------
 * Serial Task COM3: 
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
#ifndef __TASK_COM3__
#define __TASK_COM3__

#define BAUDRATE_COM3   	115200
#define MOT_ENABLE_ADR		1115
#define	MOT_ADDRESS				1
#define MOT_VAL_ADR				100
#define MOT_ERR_CODE_ADR	505
#define	MOT_ERR_NUM_ADR		525

STATUS  Task_com3(void) ;

#endif
/*-----------------------end of file--------------------------------*/
