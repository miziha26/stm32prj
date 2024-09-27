#ifndef LED_H
#define LED_H

enum LED_NUM {LED1,LED2,LED3,LED4};
enum LED_STATE {LED_ON,LED_OFF};

void Init_Led(void);
void Led_Ctl(enum LED_NUM led_num,enum LED_STATE led_state);

#define LED1_ON   Led_Ctl(LED1,LED_ON)
#define LED1_OFF  Led_Ctl(LED1,LED_OFF)

#define LED2_ON   Led_Ctl(LED2,LED_ON)
#define LED2_OFF  Led_Ctl(LED2,LED_OFF)

#define LED3_ON   Led_Ctl(LED3,LED_ON)
#define LED3_OFF  Led_Ctl(LED3,LED_OFF)

#define LED4_ON   Led_Ctl(LED4,LED_ON)
#define LED4_OFF  Led_Ctl(LED4,LED_OFF)

#define LED_ALL_ON  (LED1_ON,LED2_ON,LED3_ON,LED4_ON)
#define LED_ALL_OFF (LED1_OFF,LED2_OFF,LED3_OFF,LED4_OFF)

#endif 

