#include "u8g2Menu.h"

#include "U8x8lib.h"
#include "clib/u8g2.h"

const char *user_input_btn = 
    "Clear\n"
    "Cancel\n"
    "Ok";

void u8g2_DrawUTF8Line_menu(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, const char *s, uint8_t border_size, uint8_t is_invert)
{
  u8g2_uint_t d, str_width;
  u8g2_uint_t fx, fy, fw, fh;

  /* only horizontal strings are supported, so force this here */
  u8g2_SetFontDirection(u8g2, 0);

  /* revert y position back to baseline ref */
  y += u8g2->font_calc_vref(u8g2);   

  /* calculate the width of the string in pixel */
  str_width = u8g2_GetUTF8Width(u8g2, s);

  /* calculate delta d within the box */
  d = 0;
  if ( str_width < w )
  {
    d = w;
    d -=str_width;
    d /= 2;
  }
  else
  {
    w = str_width;
  }

  /* caluclate text box */
  fx = x;
  fy = y - u8g2_GetAscent(u8g2) ;
  fw = w;
  fh = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) ;

  /* draw the box, if inverted */
  u8g2_SetDrawColor(u8g2, 1);
  if ( is_invert )
  {
    u8g2_DrawBox(u8g2, fx -2, fy-2, fw+3, fh+3);
  }

  /* draw the frame */
  while( border_size > 0 )
  {
    // fx--;
    // fy--;
    // fw +=2;
    // fh +=2;
    fx -= 2;
    fy -= 2;
    fw +=4;
    fh +=4;
    u8g2_DrawFrame(u8g2, fx, fy, fw, fh );
    border_size--;
  }

  if ( is_invert )
  {
    u8g2_SetDrawColor(u8g2, 0);
  }
  else
  {
    u8g2_SetDrawColor(u8g2, 1);
  }

  /* draw the text */
  u8g2_DrawUTF8(u8g2, x+d, y, s);

  /* revert draw color */
  u8g2_SetDrawColor(u8g2, 1);

}

u8g2_uint_t u8g2_draw_selection_list_line(u8g2_t *u8g2, u8sl_t *u8sl, bool c_align, u8g2_uint_t y, uint8_t idx, const char *s)
{
    uint8_t border_size = 0;
    uint8_t is_invert = 0;

    u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

    /* check whether this is the current cursor line */
    if (idx == u8sl->current_pos)
    {
        border_size = MY_BORDER_SIZE;
        is_invert = 1;
    }

    /* get the line from the array */
    s = u8x8_GetStringLineStart(idx, s);

    /* draw the line */
    if (s == NULL)
        s = "";
    u8g2_DrawUTF8Line(u8g2, MY_BORDER_SIZE, y, (c_align) ? u8g2_GetDisplayWidth(u8g2) : 0, s, border_size, is_invert);
    return line_height;
}

void u8g2_DrawSelectionList(u8g2_t *u8g2, u8sl_t *u8sl, bool c_align, u8g2_uint_t y, const char *s)
{
    uint8_t i;
    for (i = 0; i < u8sl->visible; i++)
    {
        y += u8g2_draw_selection_list_line(u8g2, u8sl, c_align, y, i + u8sl->first_pos, s);
    }
}

uint8_t draw_sl(u8g2_t *u8g2, u8sl_t *u8sl, const char *title, uint8_t start_pos, bool c_align, const char *sl)
{
    u8g2_uint_t yy;
    u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

    uint8_t title_lines = u8x8_GetStringLineCnt(title);
    uint8_t display_lines;

    if (title_lines > 0)
    {
        display_lines = (u8g2_GetDisplayHeight(u8g2) - 3) / line_height;
        u8sl->visible = display_lines;
        u8sl->visible -= title_lines;
    }
    else
    {
        display_lines = u8g2_GetDisplayHeight(u8g2) / line_height;
        u8sl->visible = display_lines;
    }

    u8sl->total = u8x8_GetStringLineCnt(sl);
    u8sl->first_pos = 0;
    u8sl->current_pos = start_pos;

    if (u8sl->current_pos >= u8sl->total)
        u8sl->current_pos = u8sl->total - 1;
    if (u8sl->first_pos + u8sl->visible <= u8sl->current_pos)
        u8sl->first_pos = u8sl->current_pos - u8sl->visible + 1;

    u8g2_SetFontPosBaseline(u8g2);

    u8g2_FirstPage(u8g2);
    do
    {
        yy = u8g2_GetAscent(u8g2);
        if (title_lines > 0)
        {
            yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, (c_align) ? u8g2_GetDisplayWidth(u8g2) : 0, line_height, title);

            u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));

            yy += 3;
        }
        u8g2_DrawSelectionList(u8g2, u8sl, c_align, yy, sl);
    } while (u8g2_NextPage(u8g2));

    // update_sl(u8g2, u8sl, title, sl);

    // u8sl_Next(u8sl);
    // update_sl(u8g2, u8sl, title, sl);

    // u8sl_Prev(u8sl);
    // update_sl(u8g2, u8sl, title, sl);
    return 0;
}

uint8_t u8g2_draw_button_line(u8g2_t *u8g2, u8g2_uint_t y, u8g2_uint_t w, uint8_t cursor, const char *s)
{
  u8g2_uint_t button_line_width;
	
  uint8_t i;
  uint8_t cnt;
  uint8_t is_invert;
	
  u8g2_uint_t d;
  u8g2_uint_t x;
	
  cnt = u8x8_GetStringLineCnt(s);
  
	
  /* calculate the width of the button line */
  button_line_width = 0;
  for( i = 0; i < cnt; i++ )
  {
    button_line_width += u8g2_GetUTF8Width(u8g2, u8x8_GetStringLineStart(i, s));
  }
  button_line_width += (cnt-1)*SPACE_BETWEEN_BUTTONS_IN_PIXEL;	/* add some space between the buttons */
  
  /* calculate the left offset */
  d = 0;
  if ( button_line_width < w )
  {
    d = w;
    d -= button_line_width;
    d /= 2;
  }
  
  /* draw the buttons */
  x = d;
  for( i = 0; i < cnt; i++ )
  {
    is_invert = 0;
    if ( i == cursor )
      is_invert = 1;

    u8g2_DrawUTF8Line_menu(u8g2, x, y, 0, u8x8_GetStringLineStart(i, s), 1, is_invert);
    x += u8g2_GetUTF8Width(u8g2, u8x8_GetStringLineStart(i, s));
    x += SPACE_BETWEEN_BUTTONS_IN_PIXEL;
  }
  
  /* return the number of buttons */
  return cnt;
}

u8g2_uint_t get_txt_width(u8g2_t *u8g2, uint8_t idx, const char *sl)
{
    const char *s = sl;
    sl = u8x8_GetStringLineStart(idx, sl);
    return u8g2_GetUTF8Width(u8g2, sl);
}

void draw_menu_info(u8g2_t *u8g2, u8sl_t *u8sl, const char *sl, const char values[][15])
{
    for (int i = 0; i < u8sl->visible; i++)
    {
        u8g2_DrawStr(u8g2, get_txt_width(u8g2, u8sl->first_pos + i, sl) + X_GAP, (TITLEHEIGHT + (i * LINE_HEIGHT) + MY_BORDER_SIZE), values[u8sl->first_pos + i]);
    }
    u8g2_SendBuffer(u8g2);
}

void draw_get_input(u8g2_t *u8g2, u8in_user_t *in_control,  const char *title, uint8_t enc_pos, bool clicked, char *out_data, const char *inchar)
{
    char temp[2] =" ";
    u8g2_uint_t yy;
    u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

    yy = u8g2_GetAscent(u8g2);
    yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
    u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));

    if (in_control->cur_pos == in_user_text)
    {
        uint8_t len = strlen(out_data);

        if (clicked && inchar[enc_pos] == '<')
        {
            if(len > 1)
                out_data[--len] = '\0';
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_DrawUTF8Line_menu(u8g2, 35 + u8g2_GetUTF8Width(u8g2, out_data), 30, 0, temp, 1, true);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 4, user_input_btn);
        }
        else if (clicked && inchar[enc_pos] == 'v')
        {
            in_control->cur_pos = in_user_clear;
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 0, user_input_btn);
        }
        else if (clicked)
        {
            temp[0] = inchar[enc_pos];
            strcat(out_data, temp);

            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_DrawUTF8Line_menu(u8g2, 35 + u8g2_GetUTF8Width(u8g2, out_data), 30, 0, temp, 1, true);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 4, user_input_btn);
        }
        else
        {
            temp[0] = inchar[enc_pos];

            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_DrawUTF8Line_menu(u8g2, 35 + u8g2_GetUTF8Width(u8g2, out_data), 30, 0, temp, 1, true);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 4, user_input_btn);
        }
        
        // u8g2_DrawStr(u8g2, 35 + u8g2_GetUTF8Width(u8g2, out_data), 30, temp);
        
    }
    else if(in_control->cur_pos == in_user_clear)
    {
        if (clicked)
        {
            strcpy(out_data, " ");
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 0, user_input_btn);
        }
        else 
        {
            in_control->cur_pos = in_user_cancel;
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 1, user_input_btn);
        }
    }
    else if(in_control->cur_pos == in_user_cancel)
    {
        if (clicked)
        {
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 1, user_input_btn);
        }
        else 
        {
            in_control->cur_pos = in_user_ok;
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 2, user_input_btn);
        }
    }
    else if(in_control->cur_pos == in_user_ok)
    {
        if (clicked)
        {
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 2, user_input_btn);
        }
        else 
        {
            in_control->cur_pos = in_user_text;
            
            temp[0] = inchar[enc_pos];
            u8g2_DrawStr(u8g2, 30, 30, out_data);
            u8g2_DrawUTF8Line_menu(u8g2, 35 + u8g2_GetUTF8Width(u8g2, out_data), 30, 0, temp, 1, true);
            u8g2_draw_button_line(u8g2, 50, u8g2_GetDisplayWidth(u8g2), 4, user_input_btn);
        }
    }
    
    u8g2_SendBuffer(u8g2);
}

void draw_progress_bar(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t percent) 
{
    uint8_t width, height;

    width = u8g2_GetDisplayWidth(u8g2) - x *2;
    height = 5;

    uint8_t filled_width = width * percent / 100;

    // u8g2_DrawBox(u8g2, x, y, width, height);
    u8g2_DrawFrame(u8g2, x, y, width, height);

    u8g2_DrawBox(u8g2, x + 1, y + 1, filled_width - 1, height - 1);

    char percentage_text[4];
    snprintf(percentage_text, 4, "%d%%", percent);
    // u8g2_DrawUTF8(u8g2, x + width - u8g2_GetUTF8Width(u8g2, percentage_text), y, percentage_text);
    u8g2_DrawStr(u8g2, x + width - u8g2_GetUTF8Width(u8g2, percentage_text), y + height + 10, percentage_text);
}

void draw_title(u8g2_t *u8g2, const char *title)
{
    u8g2_uint_t yy;
    u8g2_uint_t line_height = u8g2_GetAscent(u8g2) - u8g2_GetDescent(u8g2) + MY_BORDER_SIZE;

    yy = u8g2_GetAscent(u8g2);
    yy += u8g2_DrawUTF8Lines(u8g2, 0, yy, u8g2_GetDisplayWidth(u8g2), line_height, title);
    u8g2_DrawHLine(u8g2, 0, yy - line_height - u8g2_GetDescent(u8g2) + 1, u8g2_GetDisplayWidth(u8g2));
}

void draw_label_text(u8g2_t *u8g2, uint8_t x, uint8_t y, const char *label, const char *text) 
{
    u8g2_DrawUTF8(u8g2, x, y, label);
    u8g2_DrawUTF8(u8g2, x + u8g2_GetUTF8Width(u8g2, label) + 5, y, text);
}