#ifndef __TASK_MODBUS_H
#define __TASK_MODBUS_H
#include "sys.h"

#define MODBUS_ACK_OK ( 0 )
#define MODBUS_ACK_NOTOK ( 1 )
#define MODBUS_ACK_CRC_ERR ( 2 )
#define MODBUS_ACK_FRAME_ERR ( 3 )
#define MODBUS_ACK_TIMEOUT ( 4 )
#define MODBUS_ACK_LOOPERR ( 5 )
#define MODBUS_ACK_SENDERR ( 6 )

typedef struct {
    uint8_t  id;
    uint8_t  cmd;
    uint8_t  addrH;
    uint8_t  addrL;
    uint8_t  lenH;
    uint8_t  lenL;
    uint16_t crc;
} modbusRSCmd_t;
typedef struct {
    uint8_t  id;
    uint8_t  cmd;
    uint8_t  addrH;
    uint8_t  addrL;
    uint8_t  dataH;
    uint8_t  dataL;
    uint16_t crc;
} modbusWSCmd_t;

void Modbus_Init( void );
void vModbusTask( void* param );

#endif
