
#ifndef __CHECK_GUN_H
#define __CHECK_GUN_H
#include <stdio.h>

#include "stm32f10x.h"
#include "global.h"

#define LABEL_MIS_MATCHED 1
#define LABEL_MATCHED 0

u8 Gun_Init(GUN_CHECKING *p_gun);

//return 0: new label, others: repeat label
u8 If_Label_Repeat(GUN_CHECKING *p_had_read_gun,u8 *p_label_buf);

//return 0:match! others: not match, match sequence in *p_match_seq
u8 If_Label_Match_Seq(GUN_CHECKING *p_req_gun, u8 *p_label_buf,u8 *p_match_seq);

//return insert location: ins
u8 InsertBy_match_seq(GUN_CHECKING *p_read_gun,u8 valid_num,u8 cmp_match_seq);

u8 Check_Gun(GUN_CHECKING *p_req_gun,GUN_CHECKING *p_read_gun);

#endif
