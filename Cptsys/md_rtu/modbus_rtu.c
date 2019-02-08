#include "vxWorks.h"
#include "modbus_rtu.h"

#include <sockLib.h>
#include <stdio.h>
#include <ioLib.h>
#include <taskLib.h>
#include <sioLib.h>
#include <string.h>

#define VXDEBUG 1

int mb_err = 0;

SEM_ID serialSEM;

/**********************************************************************************************************
 * desc:  串口通信初始化
 * params: 	None
 * retval:	None
 * ********************************************************************************************************/
void SerialInit(void)
{
	serialSEM = semBCreate(SEM_Q_FIFO, SEM_FULL);
}

/**********************************************************************************************************
 * desc:  	串口端口初始化
 * params: 	
 * 		ucPort:	端口号 0-9
 * 		ulBaudRate: 波特率
 * 		pvSerialFD:	端口句柄（返回值)
 * retval:	FALSE  打开失败
 * 			TRUE	打开成功
 * ********************************************************************************************************/
BOOL xMBPortSerialInit(UCHAR ucPort, ULONG ulBaudRate, int *pvSerialFd)
{
	CHAR szDevice[16];
	int iSerialFd;

	*pvSerialFd = -1;

	sprintf(szDevice, "/tyCo/%d", ucPort);
	/*
	O_NOCTTY：	这个程序不想成为“控制终端”控制的程序，不说明这个标志的话，任何输入都会影响你的程序。
	O_NDELAY：	这个程序不关心DCD信号线状态，即其他端口是否运行，不说明这个标志的话，该程序就会在DCD信号线为低电平时停止。
	O_NONBLOCK:	设置为非阻塞模式
	*/

	if ((iSerialFd = open(szDevice, O_RDWR | O_NOCTTY | O_NONBLOCK, 0)) < 0)
	{
		logMsg("xMBPortSerialInit: Can't open serial port %s: %s\n", szDevice, strerror(errno), 0, 0, 0, 0);
		return FALSE;
	}

	ioctl(iSerialFd, FIOSETOPTIONS, OPT_RAW);
	ioctl(iSerialFd, FIOBAUDRATE, ulBaudRate);
	ioctl(iSerialFd, SIO_HW_OPTS_SET, CS8 | CLOCAL | CREAD);
	ioctl(iSerialFd, FIOFLUSH, 0);

	*pvSerialFd = iSerialFd;
	return TRUE;
}

/**********************************************************************************************************
 * desc:  	串口端口关闭
 * params: 	
 * 			pvSerialFD:	端口句柄
 * retval:	None
 * ********************************************************************************************************/
void vMBPortClose(int *pvSerialFd)
{
	if (*pvSerialFd != -1)
	{
		(void)close(*pvSerialFd);
		*pvSerialFd = -1;
	}
}

/**********************************************************************************************************
 * desc:  	串口端口读取
 * params: 	
 * 		pvSerialFD:	端口句柄
 * 		pucBuffer: 接收缓冲区
 * 		usNBytes:  接收字符长度
 * 		usNBytesRead: 实际接收字符长度
 * retval:	FALSE  读取失败
 * 			TRUE	读取成功
 * ********************************************************************************************************/
BOOL prvbMBPortSerialRead(int iSerialFd, UCHAR *pucBuffer, USHORT usNBytes, USHORT *usNBytesRead)
{
	BOOL bResult = FALSE;
	ssize_t res;
	int sres;
	struct fd_set rfds;
	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	FD_ZERO(&rfds);
	FD_SET(iSerialFd, &rfds);
	sres = select(iSerialFd + 1, &rfds, NULL, NULL, &tv);
	if (sres > 0)
	{
		if (FD_ISSET(iSerialFd, &rfds))
		{
			res = read(iSerialFd, pucBuffer, usNBytes);
			if (res > 0)
			{
				*usNBytesRead = (USHORT)res;
				bResult = TRUE;
			}
		}
	}

	if (mb_err)
		printf("Select : %d, read: %d len:%d\n", sres, res, usNBytes);
	return bResult;
}

/*--------------------------------------------------------------------------------------------------
BOOL	prvbMBPortSerialWrite(int iSerialFd,  UCHAR * pucBuffer, USHORT usNBytes) 
{
    ssize_t         res;
    size_t          left = ( size_t ) usNBytes;
    size_t          done = 0;

    while( left > 0 )
    {
        if( ( res = write( iSerialFd, pucBuffer + done, left ) ) == -1 )
        {
            if( errno != EINTR )
            {
                break;
            }
            /* call write again because of interrupted system call. 
            continue;
        }
        done += res;
        left -= res;
    }
    
  	return left == 0 ? TRUE : FALSE;	
} */

/************************************************************************
 * 向串口通道写数据,
 * 参数:  iSerialFd   - 设备ID, 
 *        pucBuffer 	- 数据指针, 
 *        usNBytes    - 发送数据长度，单位字节 
 ************************************************************************/
BOOL prvbMBPortSerialWrite(int iSerialFd, UCHAR *pucBuffer, USHORT usNBytes)
{
	USHORT nwritten;

	ioctl(iSerialFd, FIOFLUSH, 0);
	nwritten = write(iSerialFd, pucBuffer, usNBytes);

	return nwritten == usNBytes ? TRUE : FALSE;

	/*	
	struct fd_set writeFds;
	struct timeval timeval;
	int ret;

	USHORT nleft;
	USHORT nwritten;
	UCHAR *ptr;

	timeval.tv_sec = 	0;
	timeval.tv_usec = 10000;

	ptr = (UCHAR *)pucBuffer;
	nleft = usNBytes ;

	while (nleft > 0)
	{
	        FD_ZERO(&writeFds);
	        FD_SET(iSerialFd, &writeFds); 
	        ret = select(iSerialFd + 1, 0, &writeFds, 0, &timeval);
	        if (ret == 0)
	        {
	                return (FALSE);
	        }
	        if (FD_ISSET(iSerialFd, &writeFds))
	        {
		        		semTake(serialSEM, WAIT_FOREVER) ;
		        		nwritten = write(iSerialFd, ptr, nleft) ;
		        		semGive(serialSEM) ;
			          if (nwritten  <= 0)
	                {
	                        if (errno == EINTR)
	                                nwritten = 0;
	                        else
	                                return FALSE;
	                }
	                nleft -= nwritten;
	                ptr += nwritten;
	        }
	}

	return  TRUE;*/
}

/*--------------------------------------------------------------------------------------------------
  函数名称：读多个保持继电器
  输入参数：
  				iSerialFd: 	读写串口地址
  				ucStation: 	读写站地址
  				usAddress: 	读写寄存器地址
  				usLen:			读写寄存器长度
  				curAddr:		读取到的数据保存到本地寄存器的地址
  返回参数：
  				读取成功返回E_NOERROR，否则返回错误代码
---------------------------------------------------------------------------------------------------*/
eMBErrorCode
ReadHoldingRegister(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr)
{
	UCHAR ucFrame[MB_FRAME_MAX_LEN];
	UCHAR *ptr;
	USHORT i, usCRC;
	USHORT usBytesCur, usBytesLen, usCounter = 1;
	USHORT usFrameLen, usNBytesRead;
	eMBErrorCode err;
	BOOL res;

	if (ucStation < MB_ADDRESS_MIN || ucStation > MB_ADDRESS_MAX)
		return MB_ESTATION;

	/*------------------Product Transmitter Frame--------------------------------*/
	ucFrame[0] = ucStation;
	ucFrame[1] = MB_FUNC_READ_HOLDING_REGISTER;
	ucFrame[2] = (UCHAR)(usAddress >> 8);
	ucFrame[3] = (UCHAR)(usAddress & 0x00FF);
	ucFrame[4] = (UCHAR)(usLen >> 8);
	ucFrame[5] = (UCHAR)(usLen & 0x00FF);
	usCRC = usMBCRC16(ucFrame, 6);
	ucFrame[6] = (UCHAR)(usCRC & 0x00FF);
	ucFrame[7] = (UCHAR)(usCRC >> 8);

	/*------------------Transmitter Frame--------------------------------------*/
	/*ioctl(iSerialFd, FIOFLUSH, 0) ; */
	do
	{
		if (prvbMBPortSerialWrite(iSerialFd, ucFrame, 8))
			break;
		taskDelay(2);
	} while (usCounter++ < 3);

	if (usCounter >= 3)
	{
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usFrameLen = 5 + 2 * usLen;
	usBytesLen = usFrameLen;
	usBytesCur = 0;
	usCounter = 0;

	do
	{
		taskDelay(5);
		if (prvbMBPortSerialRead(iSerialFd, ucFrame + usBytesCur, usBytesLen, &usNBytesRead))
		{
			usBytesCur += usNBytesRead;
			usBytesLen -= usNBytesRead;
		}

	} while (usCounter++ < 5 && usBytesCur < usFrameLen);

	/*------------------Handle Frame--------------------------------------*/
	if (usCounter >= 5)
	{
		printf("Read Hold Reg: Adr:%d, Len:%d, Fact:%d/%dbyte(%d) \n", usAddress, usLen, usBytesCur, usFrameLen, usCounter);
		mb_err = 1;
		for (i = 0; i < usBytesCur; i++)
		{
			printf("%02X ", ucFrame[i]);
			if (i % 10 == 9)
				printf("\n");
		}
		if (usBytesCur % 10 != 9)
			printf("\n");
		return MB_ERECEIVE;
	}

	mb_err = 0;
	/*------------------Handle Frame------------------------------------*/
	if (ucFrame[0] != ucStation || ucFrame[1] != MB_FUNC_READ_HOLDING_REGISTER)
	{
		printf("ReadHoldingRegister: frame error. \n");
		for (i = 0; i < usBytesCur; i++)
		{
			printf("%02X ", ucFrame[i]);
			if (i % 10 == 9)
				printf("\n");
		}
		printf("\n");
		return MB_EIO;
	}

	usCRC = usMBCRC16(ucFrame, usFrameLen - 2);
	if (usCRC != (ucFrame[usFrameLen - 1] << 8 | ucFrame[usFrameLen - 2]))
	{
		printf("ReadHoldingRegister: Recv Frame error %04X : %02X%02X\n", usCRC, ucFrame[usFrameLen - 2], ucFrame[usFrameLen - 1]);
		return MB_ECRC;
	}

	ptr = ucFrame + 3;
	for (i = 0; i < usLen; i++)
	{
		err = eMBRegInputSetting(ptr, curAddr, usLen);
	}

	return err;
}

/*--------------------------------------------------------------------------------------------------
  函数名称：写单个保持继电器
  输入参数：
  				iSerialFd: 	读写串口地址
  				ucStation: 	读写站地址
  				usAddress: 	读写寄存器地址
  				curAddr:		需要写入的内容来自本地寄存器的地址
  返回参数：
  				写成功返回E_NOERROR，否则返回错误代码
---------------------------------------------------------------------------------------------------*/
eMBErrorCode
WriteHoldingRegister(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT curAddr)
{
	UCHAR ucFrame[MB_FRAME_MAX_LEN], *ptr;
	USHORT i, usCRC;
	USHORT usBytesCur, usBytesLen, usCounter = 1;
	USHORT usFrameLen, usNBytesRead;
	eMBErrorCode err;
	BOOL res;

	if (ucStation < MB_ADDRESS_MIN || ucStation > MB_ADDRESS_MAX)
		return MB_ESTATION;

	/*------------------Product Transmitter Frame--------------------------------*/
	ucFrame[0] = ucStation;
	ucFrame[1] = MB_FUNC_WRITE_REGISTER;
	ucFrame[2] = (UCHAR)(usAddress >> 8);
	ucFrame[3] = (UCHAR)(usAddress & 0x00FF);

	ptr = ucFrame + 4;
	i = GetHoldingReg(curAddr) ;
	ucFrame[4] = (UCHAR)(i >> 8);
	ucFrame[5] = (UCHAR)(i & 0x00FF);

	usCRC = usMBCRC16(ucFrame, 6);
	ucFrame[6] = (UCHAR)(usCRC & 0x00FF);
	ucFrame[7] = (UCHAR)(usCRC >> 8);

	/*------------------Transmitter Frame--------------------------------------*/
	/*ioctl(iSerialFd, FIOFLUSH, 0) ;*/
	do
	{
		if (prvbMBPortSerialWrite(iSerialFd, ucFrame, 8))
			break;
		taskDelay(2);
	} while (usCounter++ < 3);

	if (usCounter >= 3)
	{
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usFrameLen = 8;
	usBytesLen = usFrameLen;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(2);
		if (prvbMBPortSerialRead(iSerialFd, &ucFrame[usBytesCur], usBytesLen, &usNBytesRead))
		{
			usBytesCur += usNBytesRead;
			usBytesLen -= usNBytesRead;
		}
	} while ((usBytesCur < usFrameLen) && (usCounter++ < 5));

	if (usBytesCur != usFrameLen)
		return MB_ERECEIVE;

	usCRC = usMBCRC16(ucFrame, usFrameLen - 2);
	if (usCRC != (ucFrame[usFrameLen - 1] << 8 | ucFrame[usFrameLen - 2]))
	{
		printf("WriteHoldingRegister: Recv Frame error %04X : %02X%02X\n", usCRC, ucFrame[usFrameLen - 2], ucFrame[usFrameLen - 1]);
		return MB_ECRC;
	}

	return MB_ENOERR;
}

/*--------------------------------------------------------------------------------------------------
  函数名称：写多个保持继电器
  输入参数：
  				iSerialFd: 	读写串口地址
  				ucStation: 	读写站地址
  				usAddress: 	读写寄存器地址
  				usLen:			读写寄存器长度
  				curAddr:		需要写入的内容来自本地寄存器的地址
  返回参数：
  				写成功返回E_NOERROR，否则返回错误代码
---------------------------------------------------------------------------------------------------*/
eMBErrorCode
WriteMulHoldingRegisters(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr)
{
	UCHAR ucFrame[MB_FRAME_MAX_LEN], *ptr;
	USHORT i, usCRC;
	USHORT usBytesCur, usBytesLen, usCounter = 1;
	USHORT usFrameLen, usNBytesRead;
	eMBErrorCode err;
	BOOL res;

	if (ucStation < MB_ADDRESS_MIN || ucStation > MB_ADDRESS_MAX)
		return MB_ESTATION;

	usFrameLen = 9 + 2 * usLen;

	/*------------------Product Transmitter Frame--------------------------------*/
	ucFrame[0] = ucStation;
	ucFrame[1] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	ucFrame[2] = (UCHAR)(usAddress >> 8);
	ucFrame[3] = (UCHAR)(usAddress & 0x00FF);
	ucFrame[4] = (UCHAR)(usLen >> 8);
	ucFrame[5] = (UCHAR)(usLen & 0x00FF);
	ucFrame[6] = 2 * usLen;

	ptr = ucFrame + 7;
	err = eMBRegHoldingCB(ptr, curAddr, usLen, MB_REG_READ);
	if (err != MB_ENOERR)
	{
		return err;
	}

	usCRC = usMBCRC16(ucFrame, usFrameLen - 2);
	ucFrame[usFrameLen - 2] = (UCHAR)(usCRC & 0x00FF);
	ucFrame[usFrameLen - 1] = (UCHAR)(usCRC >> 8);

	/*------------------Transmitter Frame--------------------------------------*/
	do
	{
		if (prvbMBPortSerialWrite(iSerialFd, ucFrame, usFrameLen))
			break;
		taskDelay(2);
	} while (usCounter++ < 3);

	if (usCounter >= 3)
	{
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usFrameLen = 8;
	usBytesLen = usFrameLen;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(2);
		prvbMBPortSerialRead(iSerialFd, &ucFrame[usBytesCur], usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while ((usBytesCur < usFrameLen) && (usCounter++ < 5));

	if (usBytesCur != usFrameLen)
	{
		return MB_ERECEIVE;
	}

	usCRC = usMBCRC16(ucFrame, usFrameLen - 2);
	if (usCRC != (ucFrame[usFrameLen - 1] << 8 | ucFrame[usFrameLen - 2]))
	{
		printf("ReadHoldingRegister: Recv Frame error %04X : %02X%02X\n", usCRC, ucFrame[usFrameLen - 2], ucFrame[usFrameLen - 1]);
		return MB_ECRC;
	}

	return MB_ENOERR;
}

/*--------------------------------------------------------------------------------------------------
  函数名称：读多个输入继电器
  输入参数：
  			iSerialFd: 	读写串口地址
  			ucStation: 	读写站地址
  			usAddress: 	读写寄存器地址
  			usLen:			读写寄存器长度
  			curAddr:		读取到的数据保存到本地寄存器的地址
  返回参数：
  			读取成功返回E_NOERROR，否则返回错误代码
---------------------------------------------------------------------------------------------------*/
eMBErrorCode
ReadInputRegister(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr)
{
	UCHAR ucFrame[MB_FRAME_MAX_LEN], *ptr;
	USHORT i, usCRC;
	USHORT usBytesCur, usBytesLen, usCounter = 1;
	USHORT usFrameLen, usNBytesRead;
	eMBErrorCode err;
	BOOL res;

	if (ucStation < MB_ADDRESS_MIN || ucStation > MB_ADDRESS_MAX)
		return MB_ESTATION;

	/*------------------Product Transmitter Frame--------------------------------*/
	ucFrame[0] = ucStation;
	ucFrame[1] = MB_FUNC_READ_INPUT_REGISTER;
	ucFrame[2] = (UCHAR)(usAddress >> 8);
	ucFrame[3] = (UCHAR)(usAddress & 0x00FF);
	ucFrame[4] = (UCHAR)(usLen >> 8);
	ucFrame[5] = (UCHAR)(usLen & 0x00FF);
	usCRC = usMBCRC16(ucFrame, 6);
	ucFrame[6] = (UCHAR)(usCRC & 0x00FF);
	ucFrame[7] = (UCHAR)(usCRC >> 8);

	/*------------------Transmitter Frame--------------------------------------*/
	/* ioctl(iSerialFd, FIOFLUSH, 0) ; */
	do
	{
		if (prvbMBPortSerialWrite(iSerialFd, ucFrame, 8))
			break;
		taskDelay(2);
	} while (usCounter++ < 3);

	if (usCounter >= 3)
	{
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usFrameLen = 5 + 2 * usLen;
	usBytesLen = usFrameLen;
	usBytesCur = 0;
	usCounter = 0;
	do
	{
		taskDelay(2);
		prvbMBPortSerialRead(iSerialFd, &ucFrame[usBytesCur], usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while ((usBytesCur < usFrameLen) && (usCounter++ < 3));

	if (usBytesCur != usFrameLen)
	{
		/*	printf("ReadInputRegister: receive error:%d/%d \n", usBytesCur, usFrameLen) ;
				for(i = 0 ; i < usBytesCur ; i++)
					{
						printf("%02X ", ucFrame[i]) ;
						if ( i % 10 == 9) printf("\n") ;
					}
				printf("\n") ;*/
		return MB_ERECEIVE;
	}

	/*------------------Handle Frame------------------------------------*/
	if (ucFrame[0] != ucStation || ucFrame[1] != MB_FUNC_READ_INPUT_REGISTER)
	{
		/*	printf("ReadInputRegister: frame error. \n") ;
				for(i = 0 ; i < usBytesCur ; i++)
					{
						printf("%02X ", ucFrame[i]) ;
						if ( i % 10 == 9) printf("\n") ;
					}
				printf("\n") ;*/
		return MB_EIO;
	}

	usCRC = usMBCRC16(ucFrame, usFrameLen - 2);
	if (usCRC != (ucFrame[usFrameLen - 1] << 8 | ucFrame[usFrameLen - 2]))
	{
		printf("ReadInputRegister: Recv Frame error %04X : %02X%02X\n", usCRC, ucFrame[usFrameLen - 2], ucFrame[usFrameLen - 1]);
		return MB_ECRC;
	}

	ptr = ucFrame + 3;
	for (i = 0; i < usLen; i++)
	{
		err = eMBRegInputSetting(ptr, curAddr, usLen);
	}

	return err;
}

/* --------------------------------------------------------


   ---------------------------------------------------------*/
eMBErrorCode ReadHoldingReg(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT *reg)
{
	UCHAR ucFrame[MB_FRAME_MAX_LEN];
	UCHAR *ptr;
	USHORT n, i, usCRC;
	USHORT usBytes, usCounter;
	USHORT usFrameLen, usBytesLen, usBytesCur, usNBytesRead;

	if (ucStation < MB_ADDRESS_MIN || ucStation > MB_ADDRESS_MAX)
		return MB_ESTATION;

	/*------------------Product Transmitter Frame--------------------------------*/
	ucFrame[0] = ucStation;
	ucFrame[1] = MB_FUNC_READ_HOLDING_REGISTER;
	ucFrame[2] = (UCHAR)(usAddress >> 8);
	ucFrame[3] = (UCHAR)(usAddress & 0x00FF);
	ucFrame[4] = (UCHAR)(usLen >> 8);
	ucFrame[5] = (UCHAR)(usLen & 0x00FF);
	usCRC = usMBCRC16(ucFrame, 6);
	ucFrame[6] = (UCHAR)(usCRC & 0x00FF);
	ucFrame[7] = (UCHAR)(usCRC >> 8);

	/*ioctl(iSerialFd, FIOFLUSH, 0) ; */
	usCounter = 0;
	do
	{
		if (prvbMBPortSerialWrite(iSerialFd, ucFrame, 8))
			break;
		taskDelay(2);
	} while (usCounter++ < 3);

	if (usCounter >= 3)
	{
		return MB_ETRANSMITTER;
	}

	/*------------------Receive Frame-----------------------------------------*/
	usFrameLen = 5 + 2 * usLen;
	usBytesLen = usFrameLen;
	usBytesCur = 0;
	usCounter = 0;

	do
	{
		taskDelay(5);
		prvbMBPortSerialRead(iSerialFd, ucFrame + usBytesCur, usBytesLen, &usNBytesRead);
		usBytesCur += usNBytesRead;
		usBytesLen -= usNBytesRead;
	} while (usCounter++ < 5 && usBytesCur < usFrameLen);

	if (usCounter >= 5 || usBytes != usFrameLen)
		return FALSE;

	ptr = ucFrame + 3;
	for (i = 0; i < usLen; i++)
	{
		reg[i] = (USHORT)(ptr[2 * i] * 256 + ptr[2 * i + 1]);
	}

	return MB_ENOERR;
}
/* --------------------end of file ---------------------------------------------------*/
