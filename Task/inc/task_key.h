#ifndef __TASK_KEY_H
#define __TASK_KEY_H
#include "sys.h"

//按键状态
#define KEY0 GPIO_ReadInputDataBit( GPIOE, GPIO_Pin_4 )   //读取按键0
#define KEY1 GPIO_ReadInputDataBit( GPIOE, GPIO_Pin_3 )   //读取按键1
#define WK_UP GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 )  //读取按键3(WK_UP)

#define KEY0_SHORT_PRESS    1
#define KEY0_MID_PRESS      2
#define KEY0_LONG_PRESS     3
#define KEY1_SHORT_PRESS    4
#define KEY1_MID_PRESS      5
#define KEY1_LONG_PRESS     6
#define WKUP_SHORT_PRESS    7
#define WKUP_MID_PRESS      8
#define WKUP_LONG_PRESS     9

#define MID_PRESS_COUNT     200     /*判断为中长按时间（ms）*/
#define LONG_PRESS_COUNT    1500    /*判断为长按时间（ms）*/

void vKeyTask( void* param );
u8   getKeyState( void );

#endif
