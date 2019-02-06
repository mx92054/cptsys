/*
 * Modbus rtu Libary: VxWorks Port
 * Copyright (C) 2018 chenm@idsse.ac.cn <IDSSE>
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
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: modbus_rtu.h,v 1.0 2018/05/20 20:58:49 wolti Exp $
 */
 #ifndef 	__MODBUS_RTU__
 #define	__MODBUS_RTU__
 
 
 /* ----------------------- Defines ------------------------------------------*/
#define MB_ADDRESS_BROADCAST    							( 0 )   /*! Modbus broadcast address. */
#define MB_ADDRESS_MIN          							( 1 )   /*! Smallest possible slave address. */
#define MB_ADDRESS_MAX          							( 247 ) /*! Biggest possible slave address. */
#define MB_FUNC_NONE                          (  0 )
#define MB_FUNC_READ_COILS                    (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MB_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_FUNC_WRITE_REGISTER                (  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MB_FUNC_ERROR                         ( 128 )
#define MB_FRAME_MAX_LEN											( 255 )

typedef char CHAR ;


/* ----------------------- Type definitions ---------------------------------*/
typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B
} eMBException;

typedef         eMBException( *pxMBFunctionHandler ) ( UCHAR * pucFrame, USHORT * pusLength );

typedef struct
{
    UCHAR           		ucFunctionCode;
    pxMBFunctionHandler pxHandler;
} xMBFunctionHandler;
 
/* ------------------------Register Mode----------------------------------------------*/
typedef enum
{
    MB_REG_READ,                /*!< Read register values and pass to protocol stack. */
    MB_REG_WRITE                /*!< Update register values. */
} eMBRegisterMode;

/* ------------------------Error Code------------------------------------------------*/
typedef enum
{
    MB_ENOERR = 0 ,                  /*!< no error. */
    MB_ENOREG = 1,                  /*!< illegal register address. */
    MB_EINVAL = 2,                  /*!< illegal argument. */
    MB_EPORTERR = 3,                /*!< porting layer error. */
    MB_ENORES = 4,                  /*!< insufficient resources. */
    
    MB_EIO = 5 ,                     /*!< I/O error. */
    MB_EILLSTATE = 6,               /*!< protocol stack in illegal state. */
    MB_ETIMEDOUT = 7,               /*!< timeout error occurred. */
    MB_ESTATION = 8,									/*!< illegal slave address. */
    MB_ECRC = 9,
    
    MB_ETRANSMITTER = 10,
    MB_ERECEIVE = 11			
} eMBErrorCode; 

/* ------------------------eMBParity ------------------------------------------------*/
typedef enum
{
	MB_NONE,
	MB_ODD,
	MB_EVEN
}	eMBParity ;
 
 
/* ------------------------FUNCTION ------------------------------------------------*/
void SerialInit(void) ;

BOOL 	xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, int* pvSerialFd) ;
void 	vMBPortClose( int* pvSerialFd ) ;
BOOL 	prvbMBPortSerialRead(int iSerialFd,  UCHAR * pucBuffer, USHORT usNBytes, USHORT * usNBytesRead ) ;
BOOL	prvbMBPortSerialWrite(int iSerialFd,  UCHAR * pucBuffer, USHORT usNBytes ) ;

/* 读保持寄存器值   */
eMBErrorCode
ReadHoldingRegisters(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr) ;

/*写保持寄存器值   */
eMBErrorCode
WriteHoldingRegister(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT curAddr) ;

/*写多个保持寄存器值 */
eMBErrorCode
WriteMulHoldingRegisters(int iSerialFd, UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr) ;

/* 读输入寄存器值  */
eMBErrorCode
ReadInputRegister(int iSerialFd,UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT curAddr) ; 


eMBErrorCode 
ReadHoldingReg(int iSerialFd,UCHAR ucStation, USHORT usAddress, USHORT usLen, USHORT* reg) ;

#endif
/* --------------------end of file ---------------------------------------------------*/
