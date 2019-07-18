#include "key.h"

//按键初始化函数
void KEY_Init( void )  // IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE );  //使能PORTA,PORTE时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_3;  // KEY0-KEY1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;            //设置成上拉输入
    GPIO_Init( GPIOE, &GPIO_InitStructure );                 //初始化GPIOE4,3

    //初始化 WK_UP-->GPIOA.0	  下拉输入
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // PA0设置成输入，默认下拉
    GPIO_Init( GPIOA, &GPIO_InitStructure );       //初始化GPIOA.0
}
