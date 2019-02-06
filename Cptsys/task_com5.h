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
#ifndef __TASK_COM5__
#define __TASK_COM5__

#define BAUDRATE_COM5   	115200
#define MUL_ENABLE_ADR		1120
#define	MUL_ADDRESS				1
#define MUL_VAL_ADR				300
#define MUL_ERR_CODE_ADR	510
#define	MUL_ERR_NUM_ADR		530

STATUS  Task_com5(void) ;

#endif