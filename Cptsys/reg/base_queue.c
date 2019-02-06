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

/* ------------------------------------------------------------------------
    队列的单链表实现
    头节点：哨兵作用，不存放数据，用来初始化队列时使队头队尾指向的地方
    首节点：头节点后第一个节点，存放数据  
  -----------------------------------------------------------------------*/
#include "vxworks.h"
#include "base_queue.h"

#include "stdio.h"
#include "stdlib.h"

#define DEBUG 

/* ----------------------------------------------------------------------------
    数据元素拷贝																								
------------------------------------------------------------------------------*/
void	CopyElement(Elementype* source, Elementype* dest) 
{
	dest->usAddr = source->usAddr ;
	dest->usVal = source->usVal ;	
}

/* ----------------------------------------------------------------------------
    定义队列初始化函数   																										
------------------------------------------------------------------------------*/
void InitQueue(PQueue queue) 
{
    queue->Front = queue->Rear = (PNODE)malloc(sizeof(NODE));    
    /*    动态创建头节点，使队头，队尾指向该节点
    			头节点相当于哨兵节点的作用，不存储数据（区别于首节点）  */
    if (queue->Front == NULL) {        /*    判断内存是否分配成功  */
        DebugPrintf("can't alloc enough memory!\n");
        exit(-1);
    }
    queue->Front->Next = NULL;    /*    初始队列头节点指针指向为空  */
    DebugPrintf("Create a queue succes!\n");
}

/* ---------------------------------------------------------------------------
    定义判断队列是否为空函数                                                 
------------------------------------------------------------------------------*/
BOOL IsEmptyQueue(PQueue queue) 
{
    if (queue->Front == queue->Rear) {
        DebugPrintf("Queue is empty...\n");
        return TRUE;
    }
    else {
        DebugPrintf("Queue is not empty\n");  
        return FALSE;
    }        
}

/* ----------------------------------------------------------------------------
    定义入队函数
    从队列尾部插入数据val                                                  
------------------------------------------------------------------------------*/
void InsertQueue(PQueue queue, Elementype val) 
{
    PNODE P = (PNODE)malloc(sizeof(NODE));    /*    创建一个新节点用于存放插入的元素*/
    if (P == NULL) {
        DebugPrintf("Alloc memeory failure ,cna't insert node: %d...\n", val);
        exit(-1);
    }
    CopyElement(&val, &(P->Element)) ;		/* 把要插入的数据放到节点数据域							*/
    
    P->Next = NULL;        								/*    新节点指针指向为空										*/
    queue->Rear->Next = P;  /*    使上一个队列尾部的节点指针指向新建的节点	*/
    queue->Rear = P;    		/*    更新队尾指针，使其指向队列最后的节点			*/
    DebugPrintf("Insert queue's value: %d-%d ...\n", val.usAddr,val.usVal);
}

/* ----------------------------------------------------------------------------
    定义出队函数
    从队列的首节点开始出队
    若出队成功，用val返回其值
------------------------------------------------------------------------------*/
void DeleteQueue(PQueue queue, Elementype* val) 
{
		PNODE P ;
    if (IsEmptyQueue(queue)) 
		{
        DebugPrintf("Queue is empty, can't out!\n");
        return ;
    }
    
		P= queue->Front->Next;    	/*    临时指针				*/		
		
    CopyElement(&(P->Element), val) ;		/* 把要插入的数据放到节点数据域	*/
		
    *val = P->Element;    						/*    保存其值				*/
    queue->Front->Next = P->Next;     /*    更新头节点			*/
    if (queue->Rear==P)
        queue->Rear = queue->Front;
    free(P);    											/*    释放头队列			*/
    P = NULL;    											/*    防止产生野指针	*/
    DebugPrintf("Out queue value: %d-%d\n", val->usAddr, val->usVal);
}

/* ----------------------------------------------------------------------------
    定义队列遍历函数
------------------------------------------------------------------------------*/
void TraverseQueue(PQueue queue) 
{
		PNODE P ;
    
    if (IsEmptyQueue(queue)) {
        exit(-1);
    }        
    P = queue->Front->Next;    /* 从队列首节点开始遍历（非头节点，注意区分）*/
    DebugPrintf("Traverse queue reault: ");
    while (P != NULL) {
        DebugPrintf("%d-%d  ", P->Element.usAddr, P->Element.usVal);
        P = P->Next;
    }
    DebugPrintf("\n");
}


/* ----------------------------------------------------------------------------
    定义队列的摧毁函数
    删除整个队列，包括头节点
------------------------------------------------------------------------------*/
void DestroyQueue(PQueue queue) 
{
    /*	从头节点开始删除		*/
    while (queue->Front != NULL) {
        queue->Rear = queue->Front->Next;
        free(queue->Front);
        queue->Front = queue->Rear;
    }
    DebugPrintf("Destory queue success...\n");
}


/* ----------------------------------------------------------------------------
//    定义清空队列函数
------------------------------------------------------------------------------*/
void ClearQueue(PQueue queue)
{
    PNODE P = queue->Front->Next;    	/*    临时指针							*/
    PNODE Q = NULL;        						/*    临时指针							*/
    queue->Rear = queue->Front;       /*    使队尾指针指向头节点	*/
    queue->Front->Next = NULL;
    /*    从首节点开始清空			*/
    while (P != NULL) {
        Q = P;
        P = P->Next;
        free(Q);
    }
    printf("清空队列成功...\n");
}