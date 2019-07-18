#include "sensor.h"

void Power_Init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF, ENABLE );  //使能PE,PF端口时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //匹配传感器NPN光耦输出，IO口上拉
    GPIO_Init( GPIOE, &GPIO_InitStructure );       //根据设定参数初始化GPIOE.7/9/11/13

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // IGBT Err输入PNP光耦，IO口下拉
    GPIO_Init( GPIOE, &GPIO_InitStructure );       //根据设定参数初始化GPIOE.8/10/12

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;  //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  // IO口速度为10MHz
    GPIO_Init( GPIOF, &GPIO_InitStructure );           //根据设定参数初始化GPIOF.0~5
    GPIO_ResetBits( GPIOF, GPIO_Pin_0 );               //输出口复位
    GPIO_ResetBits( GPIOF, GPIO_Pin_1 );
    GPIO_ResetBits( GPIOF, GPIO_Pin_2 );
    GPIO_ResetBits( GPIOF, GPIO_Pin_3 );
    GPIO_ResetBits( GPIOF, GPIO_Pin_4 );
    GPIO_ResetBits( GPIOF, GPIO_Pin_5 );
}
