#include "task_modbus.h"
#include "rs485.h"
#include "task_key.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define MAX_CURRENT 500			//x*0.01=5A
#define MAX_SPEED	6000		//x*0.1*20/4=3000rpm
#define MAX_FAIL_COUNT	10

u8 ackStatus;
u8 count=0;
u8 cmd_complete;

static xSemaphoreHandle rs485rxIT;

/*RS485外部中断回调函数*/
static void rs485_interruptCallback( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( rs485rxIT, &xHigherPriorityTaskWoken );
    //	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); //如果需要的话进行一次任务切换
}
void Modbus_Init( void )
{
    RS485_Init( 9600 );
    rs485rxIT = xSemaphoreCreateBinary();
    rs485_setIterruptCallback( rs485_interruptCallback );
}
u8 ModbusReadSReg( u8 id, u16 addr, u16* data )
{
    modbusRSCmd_t   readData;
    u16           rx_crc, cal_crc;
    u8            ackStatus;
    portBASE_TYPE state;
    readData.id    = id;
    readData.cmd   = 0x03;  // read cmd
    readData.addrH = addr >> 8;
    readData.addrL = addr & 0x00FF;
    readData.lenH  = 0x00;
    readData.lenL  = 0x01;
    readData.crc   = crc_chk( ( u8* )&readData, 6 );
    RS485_Send_Data( ( u8* )&readData, 8 );
    state = xSemaphoreTake( rs485rxIT, 100 );
    if ( state == pdTRUE )
    {
        if ( RS485_RX_CNT == 7 )
        {
            cal_crc = crc_chk( RS485_RX_BUF, RS485_RX_CNT - 2 );
            rx_crc  = ( ( u16 )RS485_RX_BUF[ RS485_RX_CNT - 1 ] << 8 ) + RS485_RX_BUF[ RS485_RX_CNT - 2 ];
            if ( cal_crc == rx_crc )
            {
                *data     = ( RS485_RX_BUF[ 3 ] << 8 ) + RS485_RX_BUF[ 4 ];
                ackStatus = MODBUS_ACK_OK;
            }
            else
            {
                //校验错误
                ackStatus = MODBUS_ACK_CRC_ERR;
            }
        }
        else if ( RS485_RX_CNT == 5 )
        {
            ackStatus = MODBUS_ACK_NOTOK;
        }
        else
        {
            ackStatus = MODBUS_ACK_FRAME_ERR;
        }
    }
    else
    {
        //超时
        ackStatus = MODBUS_ACK_TIMEOUT;
    }
    return ackStatus;
}
u8 ModbusWriteSReg( u8 id, u16 addr, s16 data )
{
    modbusWSCmd_t   writeData;
    u16           rx_crc, cal_crc;
    u8            ackStatus;
    portBASE_TYPE state;
    writeData.id    = id;
    writeData.cmd   = 0x06;  // read cmd
    writeData.addrH = addr >> 8;
    writeData.addrL = addr & 0x00FF;
    writeData.dataH  = (data>>8)&0xFF;
    writeData.dataL  = data&0xFF;
    writeData.crc   = crc_chk( ( u8* )&writeData, 6 );
    RS485_Send_Data( ( u8* )&writeData, 8 );
    state = xSemaphoreTake( rs485rxIT, 100 );
    if ( state == pdTRUE )
    {
        if ( RS485_RX_CNT == 8 )
        {
            cal_crc = crc_chk( RS485_RX_BUF, RS485_RX_CNT - 2 );
            rx_crc  = ( ( u16 )RS485_RX_BUF[ RS485_RX_CNT - 1 ] << 8 ) + RS485_RX_BUF[ RS485_RX_CNT - 2 ];
            if ( cal_crc == rx_crc )
            {
                ackStatus = MODBUS_ACK_OK;
            }
            else
            {
                //校验错误
                ackStatus = MODBUS_ACK_CRC_ERR;
            }
        }
        else if ( RS485_RX_CNT == 5 )
        {
            ackStatus = MODBUS_ACK_NOTOK;
        }
        else
        {
            ackStatus = MODBUS_ACK_FRAME_ERR;
        }
    }
    else
    {
        //超时
        ackStatus = MODBUS_ACK_TIMEOUT;
    }
    return ackStatus;
}

//百分比刹车
void brake(u8 percent)
{
	s16 current;
	s16	speed;
	
	if(percent>100)
	{
		percent =100;
	}
	if(percent==0)
	{
		current=10*MAX_CURRENT/100;
		speed=MAX_SPEED;
	}
	else
	{
		current=percent*MAX_CURRENT/100;
		speed=-MAX_SPEED;
	}
	cmd_complete=0;
	count=0;
	while(!cmd_complete)
	{
		ackStatus = ModbusWriteSReg( 0x02, 0x006B, current );  //  current
		if ( ackStatus == MODBUS_ACK_OK )
		{
			cmd_complete=1;
		}
		count++;
		if(count>MAX_FAIL_COUNT) break;
	}
	cmd_complete=0;
	count=0;
	while(!cmd_complete)
	{
		ackStatus = ModbusWriteSReg( 0x02, 0x0043, speed );	//speed
		if ( ackStatus == MODBUS_ACK_OK )
		{
			cmd_complete=1;
		}
		count++;
		if(count>MAX_FAIL_COUNT) break;
	}
}
//停电机
void stopMotor(void)
{
	cmd_complete=0;
	count=0;
	while(!cmd_complete)
	{
		ackStatus = ModbusWriteSReg( 0x02, 0x0040, 0 );  //  0：正常停止
		if ( ackStatus == MODBUS_ACK_OK )
		{
			cmd_complete=1;
		}
		count++;
		if(count>MAX_FAIL_COUNT) break;
	}
}

void vModbusTask( void* param )
{
    
	static u8 uKeyState=0;
	static u8 auto_mode=0;
	
    while ( 1 )
    {
        vTaskDelay( 50 );		
		uKeyState=getKeyState();
		if(uKeyState == WKUP_SHORT_PRESS)
		{
			auto_mode=1;
		}
		
		if(auto_mode)
		{
			stopMotor();
			vTaskDelay(1000);
			brake(100);
			vTaskDelay(5000);
			brake(0);
			vTaskDelay(5000);
		}
		else 
		{
			if(uKeyState == KEY0_SHORT_PRESS)
			{
				brake(100);				
			}
			else if(uKeyState == KEY1_SHORT_PRESS)
			{
				brake(0);
			}
			else
			{
				stopMotor();
			}
		}
    }
}

