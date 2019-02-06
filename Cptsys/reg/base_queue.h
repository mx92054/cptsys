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
    队列的单链表实现
    头节点：哨兵作用，不存放数据，用来初始化队列时使队头队尾指向的地方
    首节点：头节点后第一个节点，存放数据
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
    定义队列结构体                         																	
------------------------------------------------------------------------------*/
typedef struct QNode 
{
    PNODE Front, Rear;        /*  队列头，尾指针   */
} 	Queue, *PQueue;


void 	InitQueue(PQueue queue);    				/*    创建队列函数					*/
BOOL 	IsEmptyQueue(PQueue queue);    			/*    判断队列是否为空函数	*/
void 	InsertQueue(PQueue, Elementype  val); 		/*    入队函数							*/
void 	DeleteQueue(PQueue, Elementype* val);  /*    出队函数							*/
void 	DestroyQueue(PQueue queue);    			/*    摧毁队列函数					*/
void 	TraverseQueue(PQueue queue);    		/*    遍历队列函数					*/
void 	ClearQueue(PQueue queue);    				/*    清空队列函数					*/
int 	LengthQueue(PQueue queue);    			/*    求队列长度函数				*/

#define DEBUG

#ifdef DEBUG
static int DebugPrintf(const char *format, ...)
{
    va_list argPtr;
    int     count;

    va_start(argPtr, format);                  /*  获取可变参数列表  */
    fflush(stdout);                            /*  强制刷新输出缓冲区  */
    count = vfprintf(stderr, format, argPtr);  /*  将信息输出到标准出错流设备  */
    va_end(argPtr);                            /*  可变参数列表结束  */
}
#else
static int DebugPrintf(const char *format, ...)
{

}
#endif

#endif
/* ----------------------- end of file ----------------------------------------*/