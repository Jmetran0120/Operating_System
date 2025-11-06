/* nebula_ui.h - NEBULA OS User Interface
 * 
 * Provides functions to render the NEBULA OS graphical interface
 * with glassmorphism design and space theme.
 */

#ifndef NEBULA_UI_H
#define NEBULA_UI_H

#include "graphics.h"

/* Render the complete NEBULA OS interface */
void nebula_render_ui(void);

/* Draw nebula space background */
void nebula_draw_background(void);

/* Draw top bar with title and time */
void nebula_draw_top_bar(void);

/* Draw left sidebar navigation */
void nebula_draw_sidebar(void);

/* Draw main application grid */
void nebula_draw_app_grid(void);

/* Draw right-side information panel */
void nebula_draw_info_panel(void);

/* Draw bottom dock */
void nebula_draw_dock(void);

/* Draw app icon in grid */
void nebula_draw_app_icon(uint16_t x, uint16_t y, const char* name, uint8_t icon_type);

/* Draw dock icon */
void nebula_draw_dock_icon(uint16_t x, uint16_t y, uint8_t icon_type, uint8_t color);

/* Draw simple icon shapes */
void nebula_draw_icon_globe(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_folder(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_settings(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_play(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_notes(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_cloud(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_home(uint16_t x, uint16_t y, uint8_t color);
void nebula_draw_icon_terminal(uint16_t x, uint16_t y, uint8_t color);

#endif /* NEBULA_UI_H */

