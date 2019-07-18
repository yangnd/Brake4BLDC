#include "task_key.h"
#include <stdbool.h>
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"

static bool key0_pressed;
static bool key1_pressed;
static bool WK_UP_pressed;
static u8   uKeyState;
static u32  uPressedTime;
static u32  uReleasedTime;

#if 0
void vKeyTask( void* param )
{
    while ( 1 )
    {
        vTaskDelay( 20 );
        if ( key0_pressed == false && KEY0 == 0 )
        {
            key0_pressed = true;
            uPressedTime = xTaskGetTickCount();
        }
        if ( key1_pressed == false && KEY1 == 0 )
        {
            key1_pressed = true;
            uPressedTime = xTaskGetTickCount();
        }
        if ( WK_UP_pressed == false && WK_UP == 1 )
        {
            WK_UP_pressed = true;
            uPressedTime  = xTaskGetTickCount();
        }

        if ( key0_pressed == true )
        {
            if ( KEY0 == 1 )  //按键松开后刷新状态
            {
                key0_pressed  = false;
                uReleasedTime = xTaskGetTickCount();
                if ( ( uReleasedTime - uPressedTime ) > LONG_PRESS_COUNT )
                    uKeyState = KEY0_LONG_PRESS;
                else if ( ( uReleasedTime - uPressedTime ) < MID_PRESS_COUNT )
                    uKeyState = KEY0_SHORT_PRESS;
                else
                    uKeyState = KEY0_MID_PRESS;
            }
        }
        if ( key1_pressed == true )
        {
            if ( KEY1 == 1 )
            {
                key1_pressed  = false;
                uReleasedTime = xTaskGetTickCount();
                if ( ( uReleasedTime - uPressedTime ) > LONG_PRESS_COUNT )
                    uKeyState = KEY1_LONG_PRESS;
                else if ( ( uReleasedTime - uPressedTime ) < MID_PRESS_COUNT )
                    uKeyState = KEY1_SHORT_PRESS;
                else
                    uKeyState = KEY1_MID_PRESS;
            }
        }
        if ( WK_UP_pressed == true )
        {
            if ( WK_UP == 0 )
            {
                WK_UP_pressed = false;
                uReleasedTime = xTaskGetTickCount();
                if ( ( uReleasedTime - uPressedTime ) > LONG_PRESS_COUNT )
                    uKeyState = WKUP_LONG_PRESS;
                else if ( ( uReleasedTime - uPressedTime ) < MID_PRESS_COUNT )
                    uKeyState = WKUP_SHORT_PRESS;
                else
                    uKeyState = WKUP_MID_PRESS;
            }
        }
    }
}
#endif
#if 1
void vKeyTask( void* param )
{
    while ( 1 )
    {
        vTaskDelay( 20 );
        if (KEY0 == 0 )
        {
            uKeyState = KEY0_SHORT_PRESS;
        }
        else if (KEY1 == 0 )
        {
            uKeyState = KEY1_SHORT_PRESS;
        }
        else if ( WK_UP == 1 )
        {
            uKeyState = WKUP_SHORT_PRESS;
        }
		else
		{
			uKeyState=0;
		}
    }
}
#endif
//读取按键状态
u8 getKeyState( void )
{

    return uKeyState;
}
