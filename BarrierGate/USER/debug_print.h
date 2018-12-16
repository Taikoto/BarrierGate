
#ifndef  __DEBUG_PRINT__H_
#define  __DEBUG_PRINT__H_

#include "stm32f10x.h"


extern void debug_print_c (u8 ch);
extern void debug_print_nr(void);
extern void debug_printf(u8* fmt,...);

extern void debug_print_conti(u8* buf,unsigned int len);
extern void debug_print_buf(u8* buf,unsigned int len);             //print buf one line
extern u16  debug_print_buf_multi(u8* buf,u16 totlen,u16 column);  //print buf multi line

extern u16  debug_input_ch (u16 waittime,u8 *ibuf);
extern u8   debug_getch(u8 *in_buf);

extern u16  debug_getid(u8 *in_buf);

extern vu16 tim_pass(vu16 cur_ticks,vu16 last_ticks);

extern u8 print_gun_checking(GUN_CHECKING *pgun);


#endif
