#include "MyGUI/Checkbox.h"
#include "lvgl.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
// 波形显示相关参数
#define ALL_WIDTH 1024
#define ALL_HEIGHT 600

#define WAVE_VER_NUM 480
#define WAVE_HOR_NUM 800
#define PI 3.1415926
static void timer_plot(lv_timer_t *timer);
static lv_point_precise_t line_points[WAVE_VER_NUM] = {0};
void Trigger_Volt_Spinbox(lv_obj_t *parent);

typedef struct
{
    int32_t t_x;
    int32_t t_y;
    int32_t t_w;
    int32_t t_h;

    int32_t r_x;
    int32_t r_y;
    int32_t r_w;
    int32_t r_h;

    int32_t b_x;
    int32_t b_y;
    int32_t b_w;
    int32_t b_h;

    int32_t m_x;
    int32_t m_y;
    int32_t m_w;
    int32_t m_h;

    int32_t f_x;
    int32_t f_y;
    int32_t f_w;
    int32_t f_h;

} Pos_Size_t;

static Pos_Size_t osc_layout;

void osc_top_set(int32_t x, int32_t y, int32_t w, int32_t h)
{
    osc_layout.t_x = x;
    osc_layout.t_y = y;
    osc_layout.t_w = w;
    osc_layout.t_h = h;
}
void osc_middle_set(int32_t x, int32_t y, int32_t w, int32_t h)
{
    osc_layout.m_x = x;
    osc_layout.m_y = y;
    osc_layout.m_w = w;
    osc_layout.m_h = h;
}
void osc_bottom_set(int32_t x, int32_t y, int32_t w, int32_t h)
{
    osc_layout.b_x = x;
    osc_layout.b_y = y;
    osc_layout.b_w = w;
    osc_layout.b_h = h;
}

void osc_right_set(int32_t x, int32_t y, int32_t w, int32_t h)
{
    osc_layout.r_x = x;
    osc_layout.r_y = y;
    osc_layout.r_w = w;
    osc_layout.r_h = h;
}
void osc_floatlist_set(int32_t x, int32_t y, int32_t w, int32_t h)
{
    osc_layout.f_x = x;
    osc_layout.f_y = y;
    osc_layout.f_w = w;
    osc_layout.f_h = h;
}

/* 测量参数名称*/
static const char Param_Measure_str[][20] = {{"Vmax"}, {"Vmin"}, {"Vres"}, {"Vm"}, {"Vpp"}, {"T"}, {"Freq"}, {"+Duty"}, {"-Duty"}, {"Harm2"}, {"Harm3"}, {"Harm4"}, {"Harm5"}, {"THD"}};
static uint8_t Param_index[sizeof(Param_Measure_str) / sizeof(Param_Measure_str[0])];
// static uint8_t Measured_Params_num = sizeof(Param_Measure_str) / sizeof(Param_Measure_str[0]);
/*Cursors*/
static const char Cursors_str[][3] = {"X1", "X2", "Y1", "Y2"};
static uint8_t Cursors_index[sizeof(Cursors_str) / sizeof(Cursors_str[0])];

static const char X_Div_Str[][10] = {"1ns", "2ns", "5ns", "10ns", "20ns", "50ns", "100ns", "200ns",
                                     "500ns", "1ms", "2ms", "5ms", "10ms", "20ms", "50ms", "100ms"};

static const char Y_Div_Str[][10] = {"1mV", "2mV", "5mV", "10mV", "20mV", "50mV", "100mV", "200mV",
                                     "500mV", "1V", "2.5V"};

static uint32_t param_flags = CH1_EN_Msk;

/****************************************        上方界面布置   *********************************/
/****************************************        上方界面布置   *********************************/
/****************************************        上方界面布置   *********************************/

static void Top_All_Init(Pos_Size_t *pos_size)
{

    // 容纳复选框的容器
    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(cont, pos_size->t_x, pos_size->t_y);
    lv_obj_set_size(cont, pos_size->t_w, pos_size->t_h);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
}

/********************************* ********** 中间波形显示绘制  ************************************ */
/********************************* ********** 中间波形显示绘制  ************************************ */
/********************************* ********** 中间波形显示绘制  ************************************ */

static float get_wave_phase()
{
    // rand()生成0-RAND_MAX的随机数，取模12得到0-11的范围
    return (float)(lv_rand(0, 12) * PI / 12);
}

/*定时器 模拟adcadc采样的数据  */
static void timer_plot(lv_timer_t *timer)
{
    /*Use the user_data*/
    lv_obj_t *line2 = (lv_obj_t *)lv_timer_get_user_data(timer);
    if (line2 == NULL)
    {
        printf("line2 is NULL\n");
        return;
    }

    float sp = get_wave_phase();
#define N_Cycles (15)
    for (int i = 0; i < WAVE_VER_NUM; i++)
    {
        line_points[i].x = i;
        line_points[i].y = 0.5 * WAVE_HOR_NUM + 0.4 * WAVE_HOR_NUM * sin(2 * PI / WAVE_VER_NUM * i * N_Cycles + sp);
    }

    lv_line_set_points(line2, line_points, WAVE_VER_NUM);
}

static lv_obj_t *WaveCont = NULL;
static lv_obj_t *line = NULL;
static lv_obj_t *cursor_cont[4];
static lv_point_precise_t cur_line_pos[4 * 2] = {
    {0, 300},   // 点1
    {600, 300}, // 点2
    {0, 200},   // 点3
    {600, 200}, // 点4
    {100, 0},   // 点5
    {100, 350}, // 点6
    {150, 0},   // 点7
    {150, 350}  // 点8
};

static void cursor_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);
    lv_indev_t *indev = lv_indev_get_act(); // 获取当前的输入设备（如触摸屏）

    if (event == LV_EVENT_PRESSING)
    {
        for (int i = 0; i < 4; i++)
        {
            if (cursor_cont[i] == obj)
            {

                lv_point_t vect;
                lv_indev_get_vect(indev, &vect); // 获取输入设备自上次报告以来的位移向量(x, y)

                // 计算对象的新位置：当前位置 + 位移偏移量
                lv_coord_t y = lv_obj_get_y(obj);
                lv_coord_t x = lv_obj_get_x(obj);

                if (i < 2)
                {
                    y += vect.y;
                    lv_obj_set_pos(obj, x, y);
                }
                else
                {
                    x += vect.x;

                    lv_obj_set_pos(obj, x, y);
                }
                // lv_obj_get
                printf("Cursor %d clicked at (%d, %d)\n", i, x, y);
                break;
            }
        }
    }
}

void Mid_All_Init(Pos_Size_t *pos_size)
{

    WaveCont = lv_obj_create(lv_scr_act());

    lv_obj_set_pos(WaveCont, pos_size->m_x, pos_size->m_y);
    lv_obj_set_size(WaveCont, pos_size->m_w, pos_size->m_h);
    lv_obj_set_style_bg_color(WaveCont, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(WaveCont, LV_OPA_COVER, LV_PART_MAIN);

    // 移除容器内边距和边框，确保网格线可以绘制到边缘
    lv_obj_set_style_pad_all(WaveCont, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(WaveCont, 1, LV_PART_MAIN);

    lv_obj_clear_flag(WaveCont, LV_OBJ_FLAG_SCROLLABLE);

#define GRID_VER_CNT 8                    // 5列网格
#define GRID_HOR_CNT 6                    // 5行网格
#define GRID_DASH_LEN 6                   // 实线段长度(px)
#define GRID_DASH_GAP 4                   // 间隔长度(px)
#define GRID_COLOR lv_color_hex(0xFF0000) // 红色

    // 绘制水平网格线 (不包含上下边界)
    for (int i = 1; i < GRID_HOR_CNT; i++)
    {
        lv_value_precise_t y_pos = (lv_value_precise_t)(i * pos_size->m_h * 1.0f / GRID_HOR_CNT);
        //  printf("(0,%f)  (599,%f) \n", y_pos, y_pos);
        static lv_point_precise_t p[2 * (GRID_HOR_CNT - 1)];
        p[2 * i - 2].x = 0;
        p[2 * i - 2].y = y_pos;
        p[2 * i - 1].x = pos_size->m_w - 1;
        p[2 * i - 1].y = y_pos;

        lv_obj_t *h_line = lv_line_create(WaveCont);
        lv_line_set_points(h_line, &p[2 * i - 2], 2);

        // 设置线条样式
        lv_obj_set_style_line_color(h_line, GRID_COLOR, LV_PART_MAIN);
        lv_obj_set_style_line_width(h_line, 1, LV_PART_MAIN);
        lv_obj_set_style_line_dash_width(h_line, GRID_DASH_LEN, LV_PART_MAIN);
        lv_obj_set_style_line_dash_gap(h_line, GRID_DASH_GAP, LV_PART_MAIN);

        lv_obj_clear_flag(h_line, LV_OBJ_FLAG_SCROLLABLE);
    }

    // 绘制竖直网格线 (不包含左右边界)
    for (int i = 1; i < GRID_VER_CNT; i++)
    {
        lv_value_precise_t x_pos = (lv_value_precise_t)(i * pos_size->m_w * 1.0f / GRID_VER_CNT);
        //    printf("(%f,0)  (%f,359) \n", x_pos, x_pos);

        static lv_point_precise_t p2[2 * (GRID_VER_CNT - 1)];
        p2[2 * i - 2].x = x_pos;
        p2[2 * i - 2].y = 0;
        p2[2 * i - 1].x = x_pos;
        p2[2 * i - 1].y = pos_size->m_h - 1;

        lv_obj_t *v_line = lv_line_create(WaveCont);
        lv_line_set_points(v_line, &p2[2 * i - 2], 2);

        // 设置线条样式
        lv_obj_set_style_line_color(v_line, GRID_COLOR, LV_PART_MAIN);
        lv_obj_set_style_line_width(v_line, 1, LV_PART_MAIN);
        lv_obj_set_style_line_dash_width(v_line, GRID_DASH_LEN, LV_PART_MAIN);
        lv_obj_set_style_line_dash_gap(v_line, GRID_DASH_GAP, LV_PART_MAIN);

        lv_obj_clear_flag(v_line, LV_OBJ_FLAG_SCROLLABLE);
    }

    line = lv_line_create(WaveCont);

    lv_obj_set_style_line_color(line, lv_color_hex(0x0000ff), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 1, LV_PART_MAIN);

    lv_timer_t *timer1 = lv_timer_create(timer_plot, 100, line);
    lv_timer_set_repeat_count(timer1, -1);

    float ratio = (float)pos_size->m_h / (float)pos_size->m_w;
    for (uint8_t i = 0; i < 4; i++)
    {
        if (i < 2) // 绘制X轴游标线
        {
            cursor_cont[i] = lv_obj_create(WaveCont); // 存放 光标的容器  光标由 圆形 加上 矩形 构成

            lv_obj_set_pos(cursor_cont[i], 0, (i + 1) * 50);

            lv_obj_set_size(cursor_cont[i], pos_size->m_w, pos_size->m_h * 0.008);

            lv_obj_set_style_bg_color(cursor_cont[i], lv_color_hex(0x0000ff), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(cursor_cont[i], LV_OPA_50, LV_PART_MAIN);

            lv_obj_add_event_cb(cursor_cont[i], cursor_cb, LV_EVENT_PRESSING, NULL);
        }
        else // 绘制Y 轴游标线
        {
            cursor_cont[i] = lv_obj_create(WaveCont); // 存放 光标的容器  光标由 圆形 加上 矩形 构成

            lv_obj_set_pos(cursor_cont[i], (i + 1) * 50, 0);

            lv_obj_set_size(cursor_cont[i], pos_size->m_w * 0.008 * ratio, pos_size->m_h);

            lv_obj_set_style_bg_color(cursor_cont[i], lv_color_hex(0x0000ff), LV_PART_MAIN);
            lv_obj_set_style_bg_opa(cursor_cont[i], LV_OPA_50, LV_PART_MAIN);

            lv_obj_add_event_cb(cursor_cont[i], cursor_cb, LV_EVENT_PRESSING, NULL);
        }
    }
}
/********************************* ********** 下方测量参数显示绘制  ************************************ */
/********************************* ********** 下方测量参数显示绘制  ************************************ */
/********************************* ********** 下方测量参数显示绘制  ************************************ */
static lv_obj_t *Params_show_labels[sizeof(Param_Measure_str) / sizeof(Param_Measure_str[0])];
static void Bottom_All_Init(Pos_Size_t *pos_size)
{
    // flex 布局样式设置
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_COLUMN_WRAP);   // 竖向自动 换列 -----> 主轴则为 Y轴
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_CENTER); // 主轴

    /*交叉轴在自己轨道对齐方式  如 这里 Y轴 如果 obj1 宽50 obj2 30 此时 以最大宽度位
     所以obj2 在水平方向 可以0(start)  10（center） 20(end)
                            *****       *****       *****
                            ***          ***          ***

     */
    lv_style_set_flex_cross_place(&style, LV_FLEX_ALIGN_START); // 交叉轴 LV_FLEX_ALIGN_CENTER
    /*用于处里交叉轴 剩余空间分配 */
    lv_style_set_flex_track_place(&style, LV_FLEX_ALIGN_CENTER);
    // lv_style_set_flex_grow(&style, 8);

    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_style_set_bg_color(&style, lv_color_hex(0x000000));
    lv_style_set_opa(&style, LV_OPA_50);
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_style_set_border_width(&style, 2);
    // lv_style_set_outline_width(&style, 0);

    // 容纳复选框的容器
    lv_obj_t *cont = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(cont, pos_size->b_x, pos_size->b_y);
    lv_obj_set_size(cont, pos_size->b_w, pos_size->b_h);
    // lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN);
    // lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);

    // 添加上样式
    lv_obj_add_style(cont, &style, 0);

    uint8_t num = sizeof(Param_Measure_str) / sizeof(Param_Measure_str[0]);
    for (uint8_t i = 0; i < num; i++)
    {
        Params_show_labels[i] = lv_label_create(cont);
        lv_obj_set_style_text_color(Params_show_labels[i], lv_palette_main(LV_PALETTE_RED), 0);
        lv_label_set_text_fmt(Params_show_labels[i], "%s:%.4f", Param_Measure_str[i], lv_rand(0, 99999) * 0.001);

        lv_obj_center(Params_show_labels[i]);
    }
}

/********************************* ********** 右侧功能区显示绘制  ************************************ */
/********************************* ********** 右侧功能区显示绘制  ************************************ */
/********************************* ********** 右侧功能区显示绘制  ************************************ */

lv_obj_t *right_bottom_cont = NULL;

/*
右侧功能区的底层容器样式 大小 flex布局初始化
*/
void Right_Bottom_Container_Init(Pos_Size_t *pos_size)
{

    // flex 布局样式设置
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_flex_flow(&style, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_style_set_flex_main_place(&style, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_style_set_layout(&style, LV_LAYOUT_FLEX);

    lv_style_set_bg_color(&style, lv_color_hex(0x0000f0));
    lv_style_set_border_width(&style, 2);
    lv_style_set_pad_all(&style, 0);

    // 容器位置 大小设置
    right_bottom_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_pos(right_bottom_cont, pos_size->r_x, pos_size->r_y);
    lv_obj_set_size(right_bottom_cont, pos_size->r_w, pos_size->r_h);

    // 添加上样式
    lv_obj_add_style(right_bottom_cont, &style, 0);
}

// 通道开关事件处理函数  :反转开关 以及置为相应的标志位
static void switch_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_get_state(obj) & LV_STATE_CHECKED)
        {
            param_flags |= CH1_EN_Msk;
            lv_label_set_text(label, "CH1:ON");
        }
        else
        {
            param_flags &= ~CH1_EN_Msk;
            lv_label_set_text(label, "CH1:OFF");
        }
        LV_LOG_USER("State: %s  status :%d",
                    lv_obj_has_state(obj, LV_STATE_CHECKED) ? "On" : "Off",
                    (param_flags & CH1_EN_Msk) ? 1 : 0);
    }
}

/*设置示波器 通道开关初始化*/
void CH_Switch_Init(lv_obj_t *parent)
{

    lv_obj_t *sw = lv_switch_create(parent);
    lv_obj_set_style_pad_all(sw, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(sw, 0, LV_PART_MAIN);
    lv_obj_set_state(sw, LV_STATE_CHECKED, 1);
    lv_obj_set_size(sw, LV_PCT(100), 40);

    lv_obj_t *label = lv_label_create(sw);
    lv_obj_set_style_pad_all(label, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(label, 0, LV_PART_MAIN);
    lv_label_set_text(label, "CH1:ON");
    //   lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN);
    lv_obj_center(label);

    lv_obj_add_event_cb(sw, switch_event_handler, LV_EVENT_ALL, label);
}

/*触发回调函数：设置触发模式标志位**/
static void TriggerDropdown_event_cb(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    /* 获取选中的索引  */
    uint32_t index = lv_dropdown_get_selected(dropdown); // 核心函数：
    char buf[64];
    lv_dropdown_get_selected_str(dropdown, buf, sizeof(buf));
    lv_obj_t *child = lv_obj_get_child(dropdown, 0);
    LV_LOG_USER("'%s'  index: %d is selected", buf, index);
    switch (index)
    {
    case 0: // AUTO → 000
        param_flags |= TRIGGER_AUTO_VAL;
        break;
    case 1: // Normal_Up → 001
        param_flags |= TRIGGER_NORMAL_UP_VAL;
        break;
    case 2: // Normal_Fall → 010
        param_flags |= TRIGGER_NORMAL_FALL_VAL;
        break;
    case 3: // Single_Up → 011
        param_flags |= TRIGGER_SINGLE_UP_VAL;
        break;
    case 4: // Single_Fall → 100
        param_flags |= TRIGGER_SINGLE_FALL_VAL;
        break;
    default:
        break;
    }
    LV_LOG_USER("Trigger mode flags: 0x%08X", param_flags);
}
/*设置示波器触发模式初始化*/
void Trigger_Mode_Init(lv_obj_t *parent)
{
    static const char *opts = "AUTO\n"
                              "Normal_Up\n"
                              "Normal_Fall\n"
                              "Single_Up\n"
                              "Single_Fall";

    lv_obj_t *trigger_mode_list = lv_dropdown_create(parent);
    lv_obj_set_size(trigger_mode_list, LV_PCT(100), 40);

    //  lv_obj_align(trigger_mode_list, LV_ALIGN_TOP_MID, 0, 10 + 30 + 30);
    lv_obj_set_style_pad_all(trigger_mode_list, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(trigger_mode_list, 0, LV_PART_MAIN);
    lv_dropdown_set_dir(trigger_mode_list, LV_DIR_LEFT);
    lv_dropdown_set_symbol(trigger_mode_list, LV_SYMBOL_LEFT);
    lv_dropdown_set_options_static(trigger_mode_list, opts);

    /* show the last clicked item*/
    lv_dropdown_set_selected_highlight(trigger_mode_list, true);
    /* 添加事件回调 */
    lv_obj_add_event_cb(trigger_mode_list, TriggerDropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
}

static lv_obj_t *spinbox = NULL;
/* 外部获取触发电压接口  */
float Get_Triggle_Volt(void)
{
    return lv_spinbox_get_value(spinbox) / 1000.0f;
}
static void lv_spinbox_increment_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t *spinbox = (lv_obj_t *)lv_event_get_user_data(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_increment(spinbox);

        LV_LOG_USER("Trigger Volt: %.3f V", Get_Triggle_Volt());
    }
}

static void lv_spinbox_decrement_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t *spinbox = (lv_obj_t *)lv_event_get_user_data(e);
    if (code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT)
    {
        lv_spinbox_decrement(spinbox);

        LV_LOG_USER("Trigger Volt: %.3f V", Get_Triggle_Volt());
    }
}
/*触发电压调节部件初始化 */
void Trigger_Volt_Spinbox(lv_obj_t *parent)
{

    lv_obj_t *cont = lv_obj_create(parent);

    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x0), LV_PART_MAIN);

    spinbox = lv_spinbox_create(cont);
    lv_spinbox_set_range(spinbox, -2500, 25000);
    // 25.000
    lv_spinbox_set_digit_format(spinbox, 5, 2);

    lv_spinbox_step_prev(spinbox);
    lv_obj_set_size(spinbox, LV_PCT(100), 40);
    lv_obj_set_align(spinbox, LV_ALIGN_TOP_MID);
    lv_obj_set_style_text_align(spinbox, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    //  lv_spinbox_set_step(spinbox, 100);

    int32_t h = lv_obj_get_height(spinbox);

    lv_obj_t *btn = lv_button_create(cont);
    lv_obj_set_size(btn, LV_PCT(50), h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_increment_event_cb, LV_EVENT_ALL, spinbox);

    btn = lv_button_create(cont);
    lv_obj_set_size(btn, LV_PCT(50), h);
    lv_obj_align_to(btn, spinbox, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn, lv_spinbox_decrement_event_cb, LV_EVENT_ALL, spinbox);
}
/*   X Y分辨率范围选取 均为独立 即n种模式中的一个 所以可以用4位实现16种情况 每次设置只需先清旧值( &= ~ (F<<24))，再写新值   |= index<<24  */

// X时基回调：更新位27~24的4位标志位
static void XDiv_event_cb(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    // 1. 获取选中项的索引
    uint32_t x_index = lv_dropdown_get_selected(dropdown);
    // 2. 获取选中项的字符串
    char x_buf[64] = {0};
    lv_dropdown_get_selected_str(dropdown, x_buf, sizeof(x_buf));

    // 3. 安全校验：确保索引不超出X_Div_Str的选项数量（避免异常）
    uint32_t x_option_cnt = sizeof(X_Div_Str) / sizeof(X_Div_Str[0]);
    if (x_index >= x_option_cnt)
    {
        LV_LOG_WARN("X_index:%d,max_index:%d", x_index, x_option_cnt - 1);
        return;
    }

    // 4. 更新X时基标志位：先清旧值，再写新值
    param_flags &= ~X_TIMEBASE_MSK;         // 清除位27~24的旧值（不影响其他位）
    param_flags |= X_TIMEBASE_VAL(x_index); // 写入新的4位值（索引→位27~24）

    // 5. 调试打印
    LV_LOG_USER("X_div:selected:%s,index:%d,code27~24:0x%X",
                x_buf, x_index, (param_flags & X_TIMEBASE_MSK) >> X_TIMEBASE_BIT_START);
}

// Y时基回调：更新位23~20的4位标志位
static void YDiv_event_cb(lv_event_t *e)
{
    lv_obj_t *dropdown = lv_event_get_target(e);
    // 1. 获取选中项的索引
    uint32_t y_index = lv_dropdown_get_selected(dropdown);
    // 2. 获取选中项的字符串
    char y_buf[64] = {0};
    lv_dropdown_get_selected_str(dropdown, y_buf, sizeof(y_buf));

    // 3. 安全校验：确保索引不超出Y_Div_Str的选项数量
    uint32_t y_option_cnt = sizeof(Y_Div_Str) / sizeof(Y_Div_Str[0]);
    if (y_index >= y_option_cnt)
    {
        LV_LOG_WARN("Y_div:selected:%d,max_index:%d", y_index, y_option_cnt - 1);
        return;
    }

    // 4. 更新Y时基标志位：先清旧值，再写新值
    param_flags &= ~Y_TIMEBASE_MSK;         // 清除位23~20的旧值
    param_flags |= Y_TIMEBASE_VAL(y_index); // 写入新的4位值（索引→位23~20）

    // 5. 调试打印
    LV_LOG_USER("Y_div:selected:%s,index:%d,code23~20:0x%X",
                y_buf, y_index, (param_flags & Y_TIMEBASE_MSK) >> Y_TIMEBASE_BIT_START);
}
/*
X Y 分辨率范围选取 均使用下拉列表实现

*/
void XY_TimeBase_Init(lv_obj_t *parent)
{
    /*
        X时基下拉框
    */
    lv_obj_t *x_dropdown = lv_dropdown_create(parent);
    lv_obj_set_size(x_dropdown, LV_PCT(100), 40);
    lv_obj_set_style_pad_all(x_dropdown, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(x_dropdown, 0, LV_PART_MAIN);
    lv_dropdown_set_dir(x_dropdown, LV_DIR_LEFT);
    lv_dropdown_set_symbol(x_dropdown, LV_SYMBOL_LEFT);

    lv_dropdown_clear_options(x_dropdown);

    for (uint16_t i = 0; i < sizeof(X_Div_Str) / sizeof(X_Div_Str[0]); i++)
    {
        lv_dropdown_add_option(x_dropdown, X_Div_Str[i], i);
    }
    // lv_dropdown_set_options_static(x_dropdown, "1ns\n10ns\n50ns\n200ns\n1ms\n2ms\n5ms\n10ms");

    /* show the last clicked item*/
    lv_dropdown_set_selected_highlight(x_dropdown, true);
    /* 添加事件回调 */
    lv_obj_add_event_cb(x_dropdown, XDiv_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    //  lv_obj_set_style_text_align(x_dropdown, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

    /*
        Y时基下拉框
    */
    lv_obj_t *y_dropdown = lv_dropdown_create(parent);
    lv_obj_set_size(y_dropdown, LV_PCT(100), 40);
    lv_obj_set_style_pad_all(y_dropdown, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(y_dropdown, 0, LV_PART_MAIN);
    lv_dropdown_set_dir(y_dropdown, LV_DIR_LEFT);
    lv_dropdown_set_symbol(y_dropdown, LV_SYMBOL_LEFT);

    lv_dropdown_clear_options(y_dropdown);
    //    lv_dropdown_set_options_static(y_dropdown, "2mV\n6mV\n20mV\n80mV\n400mV\n1V\n2.5V");
    for (uint16_t i = 0; i < sizeof(Y_Div_Str) / sizeof(Y_Div_Str[0]); i++)
    {
        lv_dropdown_add_option(y_dropdown, Y_Div_Str[i], i);
    }

    /* show the last clicked item*/
    lv_dropdown_set_selected_highlight(y_dropdown, true);
    /* 添加事件回调 */
    lv_obj_add_event_cb(y_dropdown, YDiv_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    // lv_obj_set_style_text_align(y_dropdown, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

/* 光标 4个 X1 X2 Y1 Y2     可以同同时1存在   故使用 4bit代表每一个光标是否显示*/
/* 光标复选框事件处理 */
static void Cursors_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *cb = lv_event_get_target(e);                      // 当前复选框对象
    uint8_t *cursor_idx = (uint8_t *)lv_event_get_user_data(e); // 获取光标索引（0~3）

    // 只处理状态变化事件（勾选/取消勾选）
    if (code != LV_EVENT_VALUE_CHANGED)
        return;

    // 判断复选框是否被选中
    bool is_checked = lv_obj_has_state(cb, LV_STATE_CHECKED);

    // 根据索引更新对应的光标标志位
    switch (*cursor_idx)
    {
    case 0: // X1（Cursors_str[0]）
        if (is_checked)
            param_flags |= CURSOR_X1_MSK; // 勾选：置位
        else
            param_flags &= ~CURSOR_X1_MSK; // 取消勾选：清位
        break;
    case 1: // X2（Cursors_str[1]）
        if (is_checked)
            param_flags |= CURSOR_X2_MSK;
        else
            param_flags &= ~CURSOR_X2_MSK;
        break;
    case 2: // Y1（Cursors_str[2]）
        if (is_checked)
            param_flags |= CURSOR_Y1_MSK;
        else
            param_flags &= ~CURSOR_Y1_MSK;
        break;
    case 3: // Y2（Cursors_str[3]）
        if (is_checked)
            param_flags |= CURSOR_Y2_MSK;
        else
            param_flags &= ~CURSOR_Y2_MSK;
        break;
    default:
        LV_LOG_WARN("index_error:%d", *cursor_idx);
        break;
    }

    // 日志打印
    LV_LOG_USER("Cursor[%s]-->%s,Code23-20:0x%X",
                Cursors_str[*cursor_idx],
                is_checked ? "Enabled" : "Disabled",
                param_flags & CURSOR_ALL_MSK); // 只打印光标相关位
}

// 光标标签事件处理函数：按下显示/关闭 不断反转状态
static void cursors_label_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *cont = (lv_obj_t *)lv_event_get_user_data(e);
    static bool is_cursor_fold = false;
    is_cursor_fold = !is_cursor_fold;
    if (code == LV_EVENT_CLICKED)
    {
        if (is_cursor_fold)
        {
            lv_obj_clear_flag(cont, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
        }

        LV_LOG_USER("%s", is_cursor_fold ? "Unfolded" : "Folded");
    }
}

void Measure_Cursor_Init(Pos_Size_t *pos_size, lv_obj_t *parent)
{

    /* 容纳复选框的容器 */
    lv_obj_t *cont = lv_obj_create(lv_layer_top()); // 一定要在顶层  在屏幕活动成会被刷新的波形覆盖 ！！！
    lv_obj_set_pos(cont, pos_size->f_x, pos_size->f_y);
    lv_obj_set_size(cont, pos_size->f_w, pos_size->f_h);
    lv_obj_set_style_pad_all(cont, 1, LV_PART_MAIN);

    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont, LV_OPA_10, LV_PART_MAIN);
    // flex布局
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t *cb;

    int len = sizeof(Cursors_str) / sizeof(Cursors_str[0]);

    /* 复选框 主体样式 */
    static lv_style_t style_box;
    lv_style_init(&style_box);
    lv_style_set_bg_color(&style_box, lv_color_hex(0x0));
    lv_style_set_bg_opa(&style_box, LV_OPA_10);
    lv_style_set_border_color(&style_box, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_box, 0);
    lv_style_set_radius(&style_box, 5);
    lv_style_set_outline_width(&style_box, 3);
    lv_style_set_pad_all(&style_box, 0);

    lv_style_set_text_color(&style_box, lv_palette_main(LV_PALETTE_YELLOW));

    for (int i = 0; i < len; i++)
    {
        Cursors_index[i] = i;
        cb = lv_checkbox_create(cont);
        lv_obj_set_size(cb, 75, LV_SIZE_CONTENT);
        lv_checkbox_set_text(cb, Cursors_str[i]);
        lv_obj_add_event_cb(cb, Cursors_event_handler, LV_EVENT_ALL, &Cursors_index[i]);
        // 复选框 主体样式
        lv_obj_add_style(cb, &style_box, LV_PART_MAIN);
    }
    lv_obj_update_layout(cb);

    lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);

    // 光标参数控制标签
    lv_obj_t *con2 = lv_obj_create(parent);
    lv_obj_set_size(con2, LV_PCT(100), 40);
    lv_obj_set_style_pad_all(con2, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(con2, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(con2, lv_color_hex(0x0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(con2, LV_OPA_10, LV_PART_MAIN);

    lv_obj_t *Cursor_label = lv_label_create(con2);
    lv_label_set_text(Cursor_label, "Cursor");
    lv_obj_align(Cursor_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_flag(Cursor_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(Cursor_label, cursors_label_event_handler, LV_EVENT_CLICKED, cont);
}

/************************************************************************************
函数名：event_handler 事件回调函数
函数参数：lv_event_t *e
函数功能：处理复选框事件 设置对应参数标志位
返回值：无
************************************************************************************/
static void Param_Measure_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    uint8_t *param_index = (uint8_t *)lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_UNUSED(obj);
        const char *txt = lv_checkbox_get_text(obj);
        const char *state = lv_obj_get_state(obj) & LV_STATE_CHECKED ? "Checked" : "Unchecked";

        // 更新参数标志位
        if (lv_obj_get_state(obj) & LV_STATE_CHECKED)
        {
            param_flags |= (1 << *param_index);
        }
        else
        {
            param_flags &= ~(1 << *param_index);
        }
        // LV_UNUSED(txt);
        // LV_UNUSED(state);
        LV_LOG_USER("%s: %s", txt, state);

        LV_LOG_USER("Param index: %d, Param name: %s", *param_index, Param_Measure_str[*param_index]);
    }
}
// 测量参数标签事件处理函数：按下显示/关闭 不断反转状态
static void params_label_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *cont = (lv_obj_t *)lv_event_get_user_data(e);
    static bool is_params_fold = false;
    is_params_fold = !is_params_fold;
    if (code == LV_EVENT_CLICKED)
    {
        if (is_params_fold)
        {
            lv_obj_clear_flag(cont, LV_OBJ_FLAG_HIDDEN);
        }
        else
        {
            lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
        }
        LV_LOG_USER("%s", is_params_fold ? "Unfolded" : "Folded");
    }
}

/*设置参数复选框 来控制示波器的测量参数是否显示*/
static void Osc_mearsure_params_Init(Pos_Size_t *pos_size, lv_obj_t *parent)
{

    // 容纳复选框的容器
    lv_obj_t *cont = lv_obj_create(lv_layer_top());
    lv_obj_set_pos(cont, pos_size->f_x, pos_size->f_y);
    lv_obj_set_size(cont, pos_size->f_w, pos_size->f_h);
    lv_obj_set_style_pad_all(cont, 1, LV_PART_MAIN);

    lv_obj_set_style_border_width(cont, 2, LV_PART_MAIN);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont, LV_OPA_10, LV_PART_MAIN);
    // flex布局
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    lv_obj_t *cb;

    int len = sizeof(Param_Measure_str) / sizeof(Param_Measure_str[0]);

    // 复选框 主体样式
    static lv_style_t style_box;
    lv_style_init(&style_box);
    lv_style_set_bg_color(&style_box, lv_color_hex(0x0));
    lv_style_set_bg_opa(&style_box, LV_OPA_10);
    lv_style_set_border_color(&style_box, lv_color_hex(0xFFFFFF));
    lv_style_set_border_width(&style_box, 0);
    lv_style_set_radius(&style_box, 5);
    lv_style_set_outline_width(&style_box, 3);
    // lv_style_set_pad_hor(&style_box, 1);
    // lv_style_set_pad_ver(&style_box, 2);
    lv_style_set_pad_all(&style_box, 0);

    lv_style_set_text_color(&style_box, lv_palette_main(LV_PALETTE_YELLOW));

    for (int i = 0; i < len; i++)
    {
        Param_index[i] = i;
        cb = lv_checkbox_create(cont);
        lv_obj_set_size(cb, 75, LV_SIZE_CONTENT);
        lv_checkbox_set_text(cb, Param_Measure_str[i]);
        lv_obj_add_event_cb(cb, Param_Measure_event_handler, LV_EVENT_ALL, &Param_index[i]);
        // // 复选框 主体样式
        lv_obj_add_style(cb, &style_box, LV_PART_MAIN);
        // 设置复选框样式
        // lv_obj_add_style(cb, &style_box, LV_PART_INDICATOR);
    }
    lv_obj_update_layout(cb);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);

    /*    */
    lv_obj_t *con2 = lv_obj_create(parent);
    lv_obj_set_size(con2, LV_PCT(100), 40);
    lv_obj_set_style_pad_all(con2, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(con2, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(con2, lv_color_hex(0x0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(con2, LV_OPA_10, LV_PART_MAIN);

    lv_obj_t *Para_label = lv_label_create(con2);
    lv_label_set_text(Para_label, "Parameters");
    lv_obj_align(Para_label, LV_ALIGN_CENTER, 0, 0);
    //

    lv_obj_add_flag(Para_label, LV_OBJ_FLAG_CLICKABLE);
    // Bottom_All_Init
    lv_obj_add_event_cb(Para_label, params_label_event_handler, LV_EVENT_CLICKED, cont);
    // lv_obj_align(Para_label, LV_ALIGN_TOP_LEFT, 10, 10);
}

/*外部接口: 参数是否显示的标志位*/
uint32_t Get_Param_Flags(void)
{

    return param_flags;
}

/*
    设置示波器右侧功能：
            通道开关 触发模式 触发电压 XY时基 测量光标 测量参数选择
*/

void Right_All_Init(Pos_Size_t *pos_size)
{
    /*右侧底部容器初始化*/
    Right_Bottom_Container_Init(pos_size);
    /*通道开关*/
    CH_Switch_Init(right_bottom_cont);
    /*触发模式*/
    Trigger_Mode_Init(right_bottom_cont);
    /*触发电压调节*/
    Trigger_Volt_Spinbox(right_bottom_cont);
    /* XY时基 */
    XY_TimeBase_Init(right_bottom_cont);
    /* 测量光标 */
    Measure_Cursor_Init(pos_size, right_bottom_cont);
    /*测量参数选择   */
    Osc_mearsure_params_Init(pos_size, right_bottom_cont);
}
/*创建主背景对象并设置各个部分其位置和大小*/
static void Main_Bg_Pos_Size_Init(void)
{
    // 背景设为黑色
    lv_obj_t *bg = lv_obj_create(lv_screen_active());
    lv_coord_t scr_w = lv_disp_get_hor_res(NULL); /* 获取物理屏幕宽度 */
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL); /* 获取物理屏幕高度 */
    LV_LOG_USER("scr_w:%d,scr_h:%d  \r\n", scr_w, scr_h);
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_size(bg, scr_w, scr_h);
    lv_obj_set_style_pad_all(bg, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(bg, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(bg, lv_color_hex(0x0), LV_PART_MAIN);

    osc_top_set(0, 0, WAVE_HOR_NUM, ALL_HEIGHT - WAVE_VER_NUM);
    osc_right_set(WAVE_HOR_NUM, ALL_HEIGHT - WAVE_VER_NUM, ALL_WIDTH - WAVE_HOR_NUM, WAVE_VER_NUM);
    osc_middle_set(0, ALL_HEIGHT - WAVE_VER_NUM, WAVE_HOR_NUM, WAVE_VER_NUM);
    osc_bottom_set(0, 0, WAVE_HOR_NUM, WAVE_VER_NUM * 0.3);
    osc_floatlist_set(0, 0, WAVE_HOR_NUM, WAVE_VER_NUM * 0.3);
}
void Osc_UI_Init(void)
{
    Main_Bg_Pos_Size_Init();

    Top_All_Init(&osc_layout);

    Right_All_Init(&osc_layout);

    Mid_All_Init(&osc_layout);

    Bottom_All_Init(&osc_layout);
}

void Checkbox_trals(void)
{
#if 1
    Osc_UI_Init();

#elif 0

#elif 0

#elif 0

#endif
}
