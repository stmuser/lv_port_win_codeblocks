#ifndef CHECKBOX_H
#define CHECKBOX_H
#define CH1_EN_Msk (1 << 31)

// 触发模式使用位30、29、28（3个连续位）
#define TRIGGER_MODE_MSK (0x7 << 28) // 0x7是二进制111，左移28位后覆盖位30-28

// 1. 各触发模式对应的3位值（000~101）
#define TRIGGER_AUTO_VAL (0 << 28)        // 000 → AUTO
#define TRIGGER_NORMAL_UP_VAL (1 << 28)   // 001 → Normal_Up
#define TRIGGER_NORMAL_FALL_VAL (2 << 28) // 010 → Normal_Fall
#define TRIGGER_SINGLE_UP_VAL (3 << 28)   // 011 → Single_Up
#define TRIGGER_SINGLE_FALL_VAL (4 << 28) // 100 → Single_Fall

// 2. X时基：位27~24（4位）
#define X_TIMEBASE_BIT_START 24                                       // X时基起始位
#define X_TIMEBASE_MSK (0xF << X_TIMEBASE_BIT_START)                  // 4位掩码（0xF=0b1111）
#define X_TIMEBASE_VAL(index) ((index & 0xF) << X_TIMEBASE_BIT_START) // 索引→4位值（取低4位，避免越界）

// 3. Y时基：位23~20（4位）
#define Y_TIMEBASE_BIT_START 20                                       // Y时基起始位
#define Y_TIMEBASE_MSK (0xF << Y_TIMEBASE_BIT_START)                  // 4位掩码
#define Y_TIMEBASE_VAL(index) ((index & 0xF) << Y_TIMEBASE_BIT_START) // 索引→4位值

// 4. 光标标志位：位19~16（4位，每位对应一个光标是否启用）
#define CURSOR_X1_MSK (1 << 19)                                                        // 位19：X1光标（1=启用，0=禁用）
#define CURSOR_X2_MSK (1 << 18)                                                        // 位18：X2光标
#define CURSOR_Y1_MSK (1 << 17)                                                        // 位17：Y1光标
#define CURSOR_Y2_MSK (1 << 16)                                                        // 位16：Y2光标
#define CURSOR_ALL_MSK (CURSOR_X1_MSK | CURSOR_X2_MSK | CURSOR_Y1_MSK | CURSOR_Y2_MSK) // 所有光标位掩码
/********************************************************
                        外部接口函数声明
 ********************************************************/
/*  获取参数是否显示的标志位*/
// uint32_t Get_Param_Flags(void);
//
 void Checkbox_trals(void);

// /********************************************************
//                          内部静态函数声明
//  ********************************************************/
// /*  测量参数 以及 光标的下拉列表是否显示的 事件处理函数 */
// static void params_label_event_handler(lv_event_t *e);
// static void cursors_label_event_handler(lv_event_t *e);
// /*  参数测量 事件处理函数 置相应的标志位 */
// static void Param_Measure_event_handler(lv_event_t *e);
// static void Osc_mear_params_set(int32_t x_pos, int32_t y_pos, int32_t w, int32_t h, lv_obj_t *parent);

#endif
