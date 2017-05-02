#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "SYS.h"

void LED_Initialize(void);
extern void     LED_On           (uint32_t num);
extern void     LED_Off          (uint32_t num);
extern void     LED_Out          (uint32_t val);
extern uint32_t LED_Num          (void);

#endif		/* end bsp_led */
