/* ------------------------------------------------------------
 * input and holding register mangment: 
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
#ifndef	__BASE_QUEUE__
#define	__BASE_QUEUE__

#include "stdarg.h"
#include "stdio.h"

/* ------------------------------------------------------------------------
    ���еĵ�����ʵ��
    ͷ�ڵ㣺�ڱ����ã���������ݣ�������ʼ������ʱʹ��ͷ��βָ��ĵط�
    �׽ڵ㣺ͷ�ڵ���һ���ڵ㣬�������
  -----------------------------------------------------------------------*/
typedef struct tagElementtype
{
	USHORT usAddr ;
	USHORT usVal ;
}	Elementype ;

void	CopyElement(Elementype* source, Elementype* dest) ;

typedef struct Node {
    Elementype Element;        		
    struct Node * Next;
}NODE, *PNODE;

/* ----------------------------------------------------------------------------
    ������нṹ��                         																	
------------------------------------------------------------------------------*/
typedef struct QNode 
{
    PNODE Front, Rear;        /*  ����ͷ��βָ��   */
} 	Queue, *PQueue;


void 	InitQueue(PQueue queue);    				/*    �������к���					*/
BOOL 	IsEmptyQueue(PQueue queue);    			/*    �ж϶����Ƿ�Ϊ�պ���	*/
void 	InsertQueue(PQueue, Elementype  val); 		/*    ��Ӻ���							*/
void 	DeleteQueue(PQueue, Elementype* val);  /*    ���Ӻ���							*/
void 	DestroyQueue(PQueue queue);    			/*    �ݻٶ��к���					*/
void 	TraverseQueue(PQueue queue);    		/*    �������к���					*/
void 	ClearQueue(PQueue queue);    				/*    ��ն��к���					*/
int 	LengthQueue(PQueue queue);    			/*    ����г��Ⱥ���				*/

#define DEBUG

#ifdef DEBUG
static int DebugPrintf(const char *format, ...)
{
    va_list argPtr;
    int     count;

    va_start(argPtr, format);                  /*  ��ȡ�ɱ�����б�  */
    fflush(stdout);                            /*  ǿ��ˢ�����������  */
    count = vfprintf(stderr, format, argPtr);  /*  ����Ϣ�������׼�������豸  */
    va_end(argPtr);                            /*  �ɱ�����б����  */
}
#else
static int DebugPrintf(const char *format, ...)
{

}
#endif

#endif
/* ----------------------- end of file ----------------------------------------*/