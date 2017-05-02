#include "BSP_LED.h"

const  GPIO_PIN_ID Pin_LED[] = {
	{ GPIOF, GPIO_Pin_9  },
	{ GPIOF, GPIO_Pin_10 }
};

#define NUM_LEDS (sizeof(Pin_LED)/sizeof(GPIO_PIN_ID))
/*-----------------------------------------------------------------------------
 *      LED_Initialize:  Initialize LEDs
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Initialize(void)
{
	uint8_t n;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* 初始化LED的GPIO时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	for (n = 0; n < NUM_LEDS; n++)
	{
		/* LED基本配置为推挽输出 */
		GPIO_InitStructure.GPIO_Pin   = Pin_LED[n].num;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		
		/* LED初始化 */
		GPIO_Init(Pin_LED[n].port, &GPIO_InitStructure);
		/* 初始时将LED都关闭 */
		GPIO_SetBits(Pin_LED[n].port, Pin_LED[n].num);
	}
}

/*-----------------------------------------------------------------------------
 *      LED_On: Turns on requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_On (uint32_t num) {
  GPIO_ResetBits(Pin_LED[num].port, Pin_LED[num].num);
}

/*-----------------------------------------------------------------------------
 *      LED_Off: Turns off requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Off (uint32_t num) {
  GPIO_SetBits(Pin_LED[num].port, Pin_LED[num].num);
}

/*-----------------------------------------------------------------------------
 *      LED_Out: Write value to LEDs
 *
 * Parameters:  val - value to be displayed on LEDs
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Out (uint32_t val) {
  uint32_t n;

  for (n = 0; n < NUM_LEDS; n++) {
    if (val & (1<<n)) {
      LED_On (n);
    } else {
      LED_Off(n);
    }
  }
}

/*-----------------------------------------------------------------------------
 *      LED_Num: Get number of available LEDs
 *
 * Parameters: (none)
 * Return:      number of available LEDs
 *----------------------------------------------------------------------------*/
uint32_t LED_Num (void) {
  return (NUM_LEDS);
}

