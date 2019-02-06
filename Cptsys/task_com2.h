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
#ifndef __TASK_COM2__
#define __TASK_COM2__

#define BAUDRATE_COM2 38400
#define DCDC1_ENABLE_ADR 1112
#define DCDC1_ADDRESS 0x61
#define DCDC1_VAL_ADR 0
#define DCDC1_ERR_CODE_ADR 502
#define DCDC1_ERR_NUM_ADR 522

STATUS Task_com2(void);

#endif