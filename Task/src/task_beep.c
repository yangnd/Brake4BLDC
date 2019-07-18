#include "task_beep.h"
#include "beep.h"

/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
void vBeepTask( void* param )
{
    while ( 1 )
    {
        vTaskDelay( 300 );
        BEEP = 0;
        vTaskDelay( 300 );
    }
}
