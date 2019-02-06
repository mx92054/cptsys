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
#ifndef __DAI_MODULE_H__
#define __DAI_MODULE_H__

#define INS_FRAME_LEN 40
#define INS_TIMEOUT 3

#define DPT_FRAME_LEN 90
#define DPT_TIMEOUT 3

/* 读绝缘检测数据 */
eMBErrorCode ReadInsMeter(int hCom, USHORT nAddr);

/* 读高度计数据 */
eMBErrorCode ReadAliMeter(int hCom, USHORT nAddr);

/* 读深度计数据 */
eMBErrorCode ReadMeter(int hCom, USHORT nAddr);

#endif