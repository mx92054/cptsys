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
    ���еĵ�����ʵ��
    ͷ�ڵ㣺�ڱ����ã���������ݣ�������ʼ������ʱʹ��ͷ��βָ��ĵط�
    �׽ڵ㣺ͷ�ڵ���һ���ڵ㣬�������  
  -----------------------------------------------------------------------*/
#include "vxworks.h"
#include "base_queue.h"

#include "stdio.h"
#include "stdlib.h"

#define DEBUG 

/* ----------------------------------------------------------------------------
    ����Ԫ�ؿ���																								
------------------------------------------------------------------------------*/
void	CopyElement(Elementype* source, Elementype* dest) 
{
	dest->usAddr = source->usAddr ;
	dest->usVal = source->usVal ;	
}

/* ----------------------------------------------------------------------------
    ������г�ʼ������   																										
------------------------------------------------------------------------------*/
void InitQueue(PQueue queue) 
{
    queue->Front = queue->Rear = (PNODE)malloc(sizeof(NODE));    
    /*    ��̬����ͷ�ڵ㣬ʹ��ͷ����βָ��ýڵ�
    			ͷ�ڵ��൱���ڱ��ڵ�����ã����洢���ݣ��������׽ڵ㣩  */
    if (queue->Front == NULL) {        /*    �ж��ڴ��Ƿ����ɹ�  */
        DebugPrintf("can't alloc enough memory!\n");
        exit(-1);
    }
    queue->Front->Next = NULL;    /*    ��ʼ����ͷ�ڵ�ָ��ָ��Ϊ��  */
    DebugPrintf("Create a queue succes!\n");
}

/* ---------------------------------------------------------------------------
    �����ж϶����Ƿ�Ϊ�պ���                                                 
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
    ������Ӻ���
    �Ӷ���β����������val                                                  
------------------------------------------------------------------------------*/
void InsertQueue(PQueue queue, Elementype val) 
{
    PNODE P = (PNODE)malloc(sizeof(NODE));    /*    ����һ���½ڵ����ڴ�Ų����Ԫ��*/
    if (P == NULL) {
        DebugPrintf("Alloc memeory failure ,cna't insert node: %d...\n", val);
        exit(-1);
    }
    CopyElement(&val, &(P->Element)) ;		/* ��Ҫ��������ݷŵ��ڵ�������							*/
    
    P->Next = NULL;        								/*    �½ڵ�ָ��ָ��Ϊ��										*/
    queue->Rear->Next = P;  /*    ʹ��һ������β���Ľڵ�ָ��ָ���½��Ľڵ�	*/
    queue->Rear = P;    		/*    ���¶�βָ�룬ʹ��ָ��������Ľڵ�			*/
    DebugPrintf("Insert queue's value: %d-%d ...\n", val.usAddr,val.usVal);
}

/* ----------------------------------------------------------------------------
    ������Ӻ���
    �Ӷ��е��׽ڵ㿪ʼ����
    �����ӳɹ�����val������ֵ
------------------------------------------------------------------------------*/
void DeleteQueue(PQueue queue, Elementype* val) 
{
		PNODE P ;
    if (IsEmptyQueue(queue)) 
		{
        DebugPrintf("Queue is empty, can't out!\n");
        return ;
    }
    
		P= queue->Front->Next;    	/*    ��ʱָ��				*/		
		
    CopyElement(&(P->Element), val) ;		/* ��Ҫ��������ݷŵ��ڵ�������	*/
		
    *val = P->Element;    						/*    ������ֵ				*/
    queue->Front->Next = P->Next;     /*    ����ͷ�ڵ�			*/
    if (queue->Rear==P)
        queue->Rear = queue->Front;
    free(P);    											/*    �ͷ�ͷ����			*/
    P = NULL;    											/*    ��ֹ����Ұָ��	*/
    DebugPrintf("Out queue value: %d-%d\n", val->usAddr, val->usVal);
}

/* ----------------------------------------------------------------------------
    ������б�������
------------------------------------------------------------------------------*/
void TraverseQueue(PQueue queue) 
{
		PNODE P ;
    
    if (IsEmptyQueue(queue)) {
        exit(-1);
    }        
    P = queue->Front->Next;    /* �Ӷ����׽ڵ㿪ʼ��������ͷ�ڵ㣬ע�����֣�*/
    DebugPrintf("Traverse queue reault: ");
    while (P != NULL) {
        DebugPrintf("%d-%d  ", P->Element.usAddr, P->Element.usVal);
        P = P->Next;
    }
    DebugPrintf("\n");
}


/* ----------------------------------------------------------------------------
    ������еĴݻٺ���
    ɾ���������У�����ͷ�ڵ�
------------------------------------------------------------------------------*/
void DestroyQueue(PQueue queue) 
{
    /*	��ͷ�ڵ㿪ʼɾ��		*/
    while (queue->Front != NULL) {
        queue->Rear = queue->Front->Next;
        free(queue->Front);
        queue->Front = queue->Rear;
    }
    DebugPrintf("Destory queue success...\n");
}


/* ----------------------------------------------------------------------------
//    ������ն��к���
------------------------------------------------------------------------------*/
void ClearQueue(PQueue queue)
{
    PNODE P = queue->Front->Next;    	/*    ��ʱָ��							*/
    PNODE Q = NULL;        						/*    ��ʱָ��							*/
    queue->Rear = queue->Front;       /*    ʹ��βָ��ָ��ͷ�ڵ�	*/
    queue->Front->Next = NULL;
    /*    ���׽ڵ㿪ʼ���			*/
    while (P != NULL) {
        Q = P;
        P = P->Next;
        free(Q);
    }
    printf("��ն��гɹ�...\n");
}