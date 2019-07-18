#include "rs485.h"
#include "delay.h"

static void ( *interruptCb )( void ) = 0;
/*设置RS485中断回调函数*/
void rs485_setIterruptCallback( void ( *cb )( void ) )
{
    interruptCb = cb;
}

#ifdef EN_USART2_RX  //如果使能了接收

//接收缓存区
u8 RS485_RX_BUF[ 64 ];  //接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT = 0;

void USART2_IRQHandler( void )
{
    u8 res;

    if ( USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )  //接收到数据
    {

        res = USART_ReceiveData( USART2 );  //读取接收到的数据
        if ( RS485_RX_CNT < 64 )
        {
            RS485_RX_BUF[ RS485_RX_CNT ] = res;  //记录接收到的值
            RS485_RX_CNT++;                      //接收数据增加1
        }
    }
    else if ( USART_GetITStatus( USART2, USART_IT_IDLE ) != RESET )  //总线空闲
    {
        if ( USART_GetFlagStatus( USART2, USART_FLAG_IDLE ) != RESET )
        {
            res = USART2->SR;  //先读SR
            res = USART2->DR;  //再读DR，清除IDLE位
        }
        if ( interruptCb )
        {
            interruptCb();  //接收完成给中断信号量
        }
    }
}
#endif
//初始化IO 串口2
// pclk1:PCLK1时钟频率(Mhz)
// bound:波特率
void RS485_Init( u32 bound )
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE );  //使能GPIOA,D时钟
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE );                        //使能USART2时钟

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;        // PD7端口配置
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;       // PA2
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;             // PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    RCC_APB1PeriphResetCmd( RCC_APB1Periph_USART2, ENABLE );   //复位串口2
    RCC_APB1PeriphResetCmd( RCC_APB1Periph_USART2, DISABLE );  //停止复位

#ifdef EN_USART2_RX                                                                  //如果使能了接收
    USART_InitStructure.USART_BaudRate            = bound;                           //波特率设置
    USART_InitStructure.USART_WordLength          = USART_WordLength_9b;             // 8位数据长度(注意：：：：STM32数据长度是包含奇偶校验位之后的长度)
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;                //一个停止位
    USART_InitStructure.USART_Parity              = USART_Parity_Even;                 ///奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   //收发模式

    USART_Init( USART2, &USART_InitStructure );
    ;  //初始化串口

    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;  //使能串口2中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;            //先占优先级7级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;            //从优先级0级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;       //使能外部中断通道
    NVIC_Init( &NVIC_InitStructure );                                    //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

    //	USART_ITConfig(USART2, USART_IT_RXNE|USART_IT_IDLE, ENABLE); //开启RX非空和总线空闲中断
    USART_ITConfig( USART2, USART_IT_RXNE, ENABLE );
    USART_ITConfig( USART2, USART_IT_IDLE, ENABLE );
    USART_Cmd( USART2, ENABLE );  //使能串口

#endif

    RS485_TX_EN = 0;  //默认为接收模式
}

// RS485发送len个字节.
// buf:发送区首地址
// len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data( u8* buf, u8 len )
{
    u8 t;
    RS485_TX_EN = 1;             //设置为发送模式
    for ( t = 0; t < len; t++ )  //循环发送数据
    {
        while ( USART_GetFlagStatus( USART2, USART_FLAG_TC ) == RESET )
            ;
        USART_SendData( USART2, buf[ t ] );
    }

    while ( USART_GetFlagStatus( USART2, USART_FLAG_TC ) == RESET )
        ;
    RS485_RX_CNT = 0;
    RS485_TX_EN  = 0;  //设置为接收模式
}
// RS485查询接收到的数据
// buf:接收缓存首地址
// len:读到的数据长度
u8 RS485_Receive_Data( u8* buf, u8* len )
{
    u8 rxlen  = RS485_RX_CNT;
    u8 i      = 0;
    u8 rxFlag = 1;
    *len      = 0;                         //默认为0
    delay_ms( 10 );                        //等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
    if ( rxlen == RS485_RX_CNT && rxlen )  //接收到了数据,且接收完成了
    {
        for ( i = 0; i < rxlen; i++ )
        {
            buf[ i ] = RS485_RX_BUF[ i ];
        }
        *len         = RS485_RX_CNT;  //记录本次数据长度
        RS485_RX_CNT = 0;             //清零
        rxFlag       = 0;             //接收成功
    }
    return rxFlag;
}

// u8 *data;//数据起始地址，用于计算 CRC 值
// u8 length; //数据长度
//返回 unsigned integer 类型的 CRC 值。
u16 crc_chk( u8* data, u8 length )
{
    u8  j;
    u16 crc_reg = 0xFFFF;
    while ( length-- )
    {
        crc_reg ^= *data++;
        for ( j = 0; j < 8; j++ )
        {
            if ( crc_reg & 0x01 )
            {
                crc_reg = ( crc_reg >> 1 ) ^ 0xA001;
            }
            else
            {
                crc_reg = crc_reg >> 1;
            }
        }
    }
    return crc_reg;
}

