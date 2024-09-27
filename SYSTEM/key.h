#ifndef KEY_H
#define KEY_H

enum KEY_NUM {KEY0,KEY1,KEY2,KEY3};
enum KEY_STATE {KEY_DOWN,KEY_UP,KEY_ERROR};

void Key_Init(void);
enum KEY_STATE Key_Status(enum KEY_NUM key_num);



#endif
