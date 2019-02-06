/* usrAppInit.c - stub application initialization routine */

/* Copyright 1984-1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,02jun98,ms   written
*/

/*
DESCRIPTION
Initialize user application code.
*/ 

/******************************************************************************
*
* usrAppInit - initialize the users application
*/
#include <sockLib.h>
#include <ioLib.h>
#include <stdio.h>
#include <taskLib.h>
#include <sioLib.h>
#include <string.h>
 
#include "vxWorks.h"
#include "errno.h"
#include "time.h"

#include "serialTask.h"

#include "task_com1.h"
#include "task_com2.h"
#include "task_com3.h"
#include "task_com4.h"
#include "task_com5.h"


#include "record.h"
#include "md_rtu\modbus_rtu.h"
#include "reg\base_queue.h"
#include "reg\partreg.h"

#define S_xxx_NOT_IMPLEMENTED	1

extern USHORT usRegHoldingBuf[] ;
extern USHORT usRegInputBuf[] ;


int serialErrCount = 0 ;
int serialTotalCount = 0 ; 

STATUS powerb() ;

void usrAppInit (void)
 {
#ifdef	USER_APPL_INIT
	USER_APPL_INIT;		/* for backwards compatibility */
#endif

    /* add application specific code here */
		ModbusChain() ;
		printf("\nInitial modbus-tcp procotol success!\n") ;	
		
		biostime() ;
		printf("\n Initial system real time...") ;
		
		SerialInit() ;
	/*
	taskSpawn("BOARD", 101, 0, 2048,(FUNCPTR)task_board, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	
	taskSpawn("COM1", 101, 0, 4096,(FUNCPTR)Task_com1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	taskDelay(1) ;
	taskSpawn("COM2", 102, 0, 4096,(FUNCPTR)Task_com2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	taskDelay(1) ; */
	taskSpawn("COM3", 103, 0, 4096,(FUNCPTR)Task_com3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	taskDelay(1) ;
	/*
	taskSpawn("COM4", 104, 0, 4096,(FUNCPTR)Task_com4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	taskDelay(1) ;
	*/
	taskSpawn("COM5", 99, 0, 4096,(FUNCPTR)Task_com5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) ;
	taskDelay(1) ; 
}

/* -------------------------------------------------------------------------------- */
STATUS SwModule(int sw )
{
	int i ;
	for( i = 1111 ; i < 1129 ; i++)
		SetHoldingReg(i, sw) ;
	if ( sw == 0)
		printf("Turn off all module!\n") ;
	else
		printf("Turn on all module!\n") ;
	return OK ;	
}

/*---------------------------------------------------------------------------------*/
STATUS testqueue()
{
	PQueue q ;
	Elementype val ;
	
	InitQueue(q) ;
	IsEmptyQueue(q) ;
	val.usAddr = 10 ;
	val.usVal = 100 ;
	InsertQueue(q,val) ;
	val.usAddr = 11 ;
	val.usVal = 200 ;
	InsertQueue(q,val) ;
	val.usAddr = 12 ;
	val.usVal = 300 ;
	InsertQueue(q,val) ;
	InsertQueue(q,val) ;
	TraverseQueue(q) ;
	DeleteQueue(q,&val) ;
	TraverseQueue(q) ;
	DestroyQueue(q) ;
		
	return OK ;
}
