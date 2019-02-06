#include <vxWorks.h>
#include <wdLib.h>
#include <ioLib.h>
#include <taskLib.h>
#include <sioLib.h>
#include <stdio.h>

/*total COM Number*/
#define	COM_NUM		10
/*com name length*/
#define	COM_NAME_LEN	16
/*task name length*/
#define	TASK_NAME_LEN	16

/* sending and recving buffer size*/
#define	T_BUFSIZE 	512

#define TASK_PRIORITY	100
#define	TASK_STACKSIZE	0x6000

/*sending count*/
unsigned long sendcnt[COM_NUM] = {0};
/*receiving count*/
unsigned long recvcnt[COM_NUM] = {0};


/*file descripter*/
int fd1[COM_NUM] = {0};
/*receive task id*/
int rid[COM_NUM] = {0};
/*send task id*/
int sid[COM_NUM] = {0};
int ttyRecv(int rcom)
{

	char readbuf[T_BUFSIZE] = {0};
	#if 1
	int readcnt = 0;
	FOREVER
	{
		readcnt = read(fd1[rcom], readbuf, T_BUFSIZE);
		if(readcnt > 0)
		{
			recvcnt[rcom] += readcnt;
			
			#if 1
			readbuf[readcnt] = '\0';
			printf("%s", readbuf);
			#endif
		}
		#if 1
		taskDelay(30);	
		#endif
	}
	return OK;
	#endif
}

int ttySend(int scom)
{
	int size;
	char sendbuf[T_BUFSIZE] = {0};

	sprintf(sendbuf, "channel %d:SBS Embedded Computer", scom);
	size = strlen(sendbuf);

	FOREVER
	{
		sendcnt[scom] += write(fd1[scom], sendbuf, size);
		taskDelay(60);	
	}
	return OK;
}


STATUS setupParam(int com, int baud)
{
	if(ERROR == ioctl(fd1[com], FIOBAUDRATE, baud))
	{
		
		printf("Can not set bautrate of COM PORT %d\n", com);
		return ERROR;
	}

	/*no parity*/
	if(ERROR == ioctl(fd1[com], SIO_HW_OPTS_SET, CS8 | CLOCAL | CREAD))
	{
		printf("Can not set HW Options of COM PORT %d\n", com);
		return ERROR;
	}

	if(ERROR == ioctl(fd1[com], FIOSETOPTIONS, OPT_RAW))
	{
		printf("Can not set working mode\n");
		return ERROR;
	}

	if(ERROR == ioctl(fd1[com], FIOFLUSH, 0))
	{
		printf("can not flush\n");
		return ERROR;
	}
	return OK;
}


int openCom(int com, int baud)
{
	char comName[COM_NAME_LEN] = {0};
	sprintf(comName, "/tyCo/%d", com);
	if((fd1[com] = open(comName,O_RDWR,0)) == ERROR)
	{
		printf("Can not open COM %s \n", comName);
		return ERROR;
	}

	if (ERROR == setupParam(com, baud))
	{
		printf("setup COM %d params Failed", com);
		return ERROR;
	}
	return OK;
}


STATUS rcom()
{
	int	i;
	int 	col = 0;
	FOREVER
	{	
		printf("\n-------------PMI(0) MSP(2,3,4,5) MSP-4(6,7,8,9) COM TEST--------------\n"); 
		for(i = 0; i < COM_NUM; ++i)
		{
			/*com 1 is not being test*/
			if(i == 1)
				continue;
			col++;
			printf("COM%d: r%10d s%10d\t\t   ", i, recvcnt[i], sendcnt[i]);
			if(col % 2 == 0)
				printf("\n");
			
		}
		col = 0;

		taskDelay(sysClkRateGet() * 8);
	}
	return OK;
}

STATUS kt(int com)
{

	if((com < 0) || (com >= COM_NUM))
	{
		printf("invalid com port number\n");
		return ERROR;
	}


	if(rid[com])
	{
		td(rid[com]);
		rid[com] = 0;
	}
	if(sid[com])
	{
		td(sid[com]);
		sid[com] = 0;
	}

	if(fd1[com])
	{
		close(fd1[com]);
		fd1[com] = 0;
	}

	return OK;
}

/* com should be start with 0, eg. COM0--->0, COM1---->1*/

STATUS rtn(int com, int baud)
{
	char sn[TASK_NAME_LEN] = {0};
	char rn[TASK_NAME_LEN] = {0};
	WDOG_ID wid = wdCreate();
	if((com < 0) || (com >= COM_NUM) || (fd1[com]))
	{
		if(!fd1[com])
			printf("invalid com port number %d\n", com);
		else
			printf("this serial port has already opened!\n");
		return ERROR;
	}
	sprintf(sn, "send%d", com);
	sprintf(rn, "recv%d", com);

	if(ERROR == openCom(com, baud))
	{
		return ERROR;
	}
	if((rid[com] = taskSpawn(rn, TASK_PRIORITY, 0, TASK_STACKSIZE, \
		(FUNCPTR)ttyRecv, com, 0, 0, 0, 0, 0, 0, 0, 0, 0)) == ERROR)		
	{
		printf("Spawn Recv Task Failed!\n");
		return ERROR;
	}

	/**/
	if((sid[com] = taskSpawn(sn, TASK_PRIORITY, 0, TASK_STACKSIZE, \
		(FUNCPTR)ttySend, com, 0, 0, 0, 0, 0, 0, 0, 0, 0)) == ERROR)
	{
		printf("Spawn Send Task Failed!\n");
		return ERROR;
	}
	
	return OK;
}

STATUS siotest(void)
{
	int i;
	for(i = 0; i < COM_NUM; i++)
	{
		if(ERROR == rtn(i, 115200))
		{
			return ERROR;
		}
	}
	return OK;
}
STATUS pfcom()
{
	int	i;
	int 	col = 0;
	
	printf("\n------------- COM TEST--------------\n"); 
	for(i = 0; i < COM_NUM; ++i)
	{
		col++;
		printf("COM%d: r%10d s%10d\t\t   ", i, recvcnt[i], sendcnt[i]);
		if(col % 2 == 0)
			printf("\n");
			
	}
	col = 0;

	return OK;
}
