#ifndef U8G2MENU_h
#define U8G2MENU_h
// #include <U8g2lib.h>

#include "U8x8lib.h"
#include "clib/u8g2.h"

#define SPACE_BETWEEN_BUTTONS_IN_PIXEL 6
#define SPACE_BETWEEN_TEXT_AND_BUTTONS_IN_PIXEL 3
#define MY_BORDER_SIZE 2
#define LINE_HEIGHT 11
#define TITLEHEIGHT 20
#define X_GAP 10


enum in_user_enum{
    in_user_text,
    in_user_clear,
    in_user_cancel,
    in_user_ok
};

struct u8in_user_t{
    uint8_t cur_pos = 0;
};

uint8_t draw_sl(u8g2_t *u8g2, u8sl_t *u8sl, const char *title, uint8_t start_pos, bool c_align, const char *sl);
void draw_menu_info(u8g2_t *u8g2, u8sl_t *u8sl, const char *sl, const char values[][15]);
u8g2_uint_t get_txt_width(u8g2_t *u8g2, uint8_t idx, const char *sl);
uint8_t u8g2_draw_button_line(u8g2_t *u8g2, u8g2_uint_t y, u8g2_uint_t w, uint8_t cursor, const char *s);

void draw_get_input(u8g2_t *u8g2, u8in_user_t *in_control,  const char *title, uint8_t enc_pos, bool clicked, char *out_data, const char *inchar);
void draw_progress_bar(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t percent) ;
void draw_title(u8g2_t *u8g2, const char *title);
void draw_label_text(u8g2_t *u8g2, uint8_t x, uint8_t y, const char *label, const char *text); 
#endif