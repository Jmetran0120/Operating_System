/* nebula_ui.c - NEBULA OS User Interface Implementation
 * 
 * Implements the complete NEBULA OS graphical interface.
 */

#include "nebula_ui.h"
#include "graphics.h"

/* Draw nebula space background */
void nebula_draw_background(void) {
    /* Fill with dark space color */
    graphics_clear(COLOR_BLACK);
    
    /* Draw stars - make them more visible */
    for (int i = 0; i < 300; i++) {
        uint16_t x = (i * 37 + 13) % SCREEN_WIDTH;
        uint16_t y = (i * 73 + 47) % SCREEN_HEIGHT;
        graphics_set_pixel(x, y, COLOR_WHITE);
        /* Make some stars brighter */
        if (i % 7 == 0) {
            graphics_set_pixel(x + 1, y, COLOR_WHITE);
            graphics_set_pixel(x, y + 1, COLOR_WHITE);
        }
        if (i % 3 == 0 && i % 7 != 0) {
            graphics_set_pixel(x + 1, y, COLOR_LIGHT_GREY);
        }
    }
    
    /* Draw nebula/galaxy in center - swirl pattern */
    uint16_t center_x = SCREEN_WIDTH / 2;
    uint16_t center_y = SCREEN_HEIGHT / 2;
    
    for (int angle = 0; angle < 360; angle += 5) {
        for (int radius = 20; radius < 120; radius += 3) {
            /* Use integer math instead of float */
            int swirl = radius / 10;
            int x = center_x + ((radius + swirl) * (angle < 180 ? 1 : -1) / 2);
            int y = center_y + (radius * (angle < 90 || angle > 270 ? 1 : -1) / 3);
            
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint8_t color = COLOR_BLUE;
                if (radius % 9 == 0) color = COLOR_LIGHT_BLUE;
                if (radius % 15 == 0) color = COLOR_MAGENTA;
                graphics_set_pixel(x, y, color);
            }
        }
    }
}

/* Draw top bar */
void nebula_draw_top_bar(void) {
    /* Draw top bar background */
    graphics_fill_rect(0, 0, SCREEN_WIDTH, 25, COLOR_DARK_GREY);
    
    /* Draw "NEBULA OS" title on left */
    graphics_draw_text(10, 8, "NEBULA OS", COLOR_WHITE);
    
    /* Draw time on right (simulated as "10:30 AM") */
    graphics_draw_text(SCREEN_WIDTH - 70, 8, "10:30 AM", COLOR_WHITE);
    
    /* Draw battery/signal indicator */
    graphics_fill_rect(SCREEN_WIDTH - 90, 10, 15, 8, COLOR_LIGHT_GREY);
    graphics_fill_rect(SCREEN_WIDTH - 90, 10, 12, 8, COLOR_WHITE);
}

/* Draw left sidebar */
void nebula_draw_sidebar(void) {
    uint16_t sidebar_x = 5;
    uint16_t sidebar_y = 30;
    uint16_t sidebar_w = 70;
    uint16_t sidebar_h = 140;
    
    /* Draw glass panel */
    graphics_draw_glass_panel(sidebar_x, sidebar_y, sidebar_w, sidebar_h, 128);
    
    /* Menu items */
    const char* menu_items[] = {"Home", "Applications", "Files", "Files", "Settings", "Files", "Terminal"};
    uint16_t item_y = sidebar_y + 10;
    
    for (int i = 0; i < 7; i++) {
        /* Draw icon (simple circle/square) */
        uint16_t icon_x = sidebar_x + 10;
        uint16_t icon_y = item_y + 2;
        
        if (i == 3) {
            /* Highlighted item */
            graphics_fill_rect(sidebar_x + 5, item_y - 2, sidebar_w - 10, 15, COLOR_LIGHT_BLUE);
        }
        
        /* Draw simple icon */
        graphics_fill_circle(icon_x, icon_y, 4, COLOR_WHITE);
        
        /* Draw text */
        graphics_draw_text(sidebar_x + 20, item_y, menu_items[i], COLOR_WHITE);
        
        item_y += 18;
    }
}

/* Draw simple icon shapes */
void nebula_draw_icon_globe(uint16_t x, uint16_t y, uint8_t color) {
    graphics_draw_circle(x, y, 12, color);
    graphics_draw_line_h(x - 12, y, 24, color);
    graphics_draw_line_v(x, y - 12, 24, color);
}

void nebula_draw_icon_folder(uint16_t x, uint16_t y, uint8_t color) {
    graphics_fill_rect(x - 8, y - 8, 16, 12, color);
    graphics_fill_rect(x - 10, y - 6, 20, 2, color);
}

void nebula_draw_icon_settings(uint16_t x, uint16_t y, uint8_t color) {
    graphics_draw_circle(x, y, 8, color);
    graphics_set_pixel(x, y - 8, color);
    graphics_set_pixel(x, y + 8, color);
    graphics_set_pixel(x - 8, y, color);
    graphics_set_pixel(x + 8, y, color);
}

void nebula_draw_icon_play(uint16_t x, uint16_t y, uint8_t color) {
    /* Triangle pointing right */
    for (int i = 0; i < 8; i++) {
        graphics_draw_line_h(x - 6 + i, y - i, 2 * i + 1, color);
    }
}

void nebula_draw_icon_notes(uint16_t x, uint16_t y, uint8_t color) {
    graphics_draw_rect(x - 8, y - 10, 16, 20, color);
    graphics_draw_line_h(x - 6, y - 5, 12, color);
    graphics_draw_line_h(x - 6, y, 12, color);
    graphics_draw_line_h(x - 6, y + 5, 12, color);
}

void nebula_draw_icon_cloud(uint16_t x, uint16_t y, uint8_t color) {
    graphics_fill_circle(x - 4, y, 5, color);
    graphics_fill_circle(x + 4, y, 5, color);
    graphics_fill_circle(x, y, 5, color);
    graphics_fill_rect(x - 9, y - 2, 18, 7, color);
}

void nebula_draw_icon_home(uint16_t x, uint16_t y, uint8_t color) {
    /* House shape */
    graphics_fill_rect(x - 6, y - 2, 12, 8, color);
    /* Roof */
    graphics_set_pixel(x, y - 6, color);
    graphics_draw_line_h(x - 4, y - 5, 9, color);
    graphics_draw_line_h(x - 3, y - 4, 7, color);
}

void nebula_draw_icon_terminal(uint16_t x, uint16_t y, uint8_t color) {
    graphics_draw_rect(x - 8, y - 6, 16, 12, color);
    graphics_fill_circle(x - 4, y - 2, 2, color);
    graphics_fill_circle(x, y - 2, 2, color);
    graphics_fill_circle(x + 4, y - 2, 2, color);
}

/* Draw app icon in grid */
void nebula_draw_app_icon(uint16_t x, uint16_t y, const char* name, uint8_t icon_type) {
    uint16_t icon_size = 50;
    uint16_t icon_x = x + icon_size / 2;
    uint16_t icon_y = y + 15;
    
    /* Draw glass panel for app tile */
    graphics_draw_glass_panel(x, y, icon_size, icon_size, 128);
    
    (void)icon_size;  /* Suppress unused variable warning */
    
    /* Draw icon based on type */
    switch (icon_type) {
        case 0: nebula_draw_icon_globe(icon_x, icon_y, COLOR_WHITE); break;
        case 1: nebula_draw_icon_settings(icon_x, icon_y, COLOR_WHITE); break;
        case 2: nebula_draw_icon_folder(icon_x, icon_y, COLOR_WHITE); break;
        case 3: nebula_draw_icon_play(icon_x, icon_y, COLOR_WHITE); break;
        case 4: nebula_draw_icon_notes(icon_x, icon_y, COLOR_WHITE); break;
        case 5: nebula_draw_icon_cloud(icon_x, icon_y, COLOR_WHITE); break;
    }
    
    /* Draw app name */
    uint16_t text_x = x + (icon_size - 8 * 6) / 2;  /* Center text */
    graphics_draw_text(text_x, y + icon_size - 10, name, COLOR_WHITE);
}

/* Draw main application grid */
void nebula_draw_app_grid(void) {
    /* Center grid on screen */
    uint16_t grid_start_x = 90;
    uint16_t grid_start_y = 40;
    uint16_t icon_size = 50;
    uint16_t icon_spacing = 60;
    
    /* App definitions */
    struct {
        const char* name;
        uint8_t icon_type;
    } apps[] = {
        {"Browser", 0},
        {"Settings", 1},
        {"Files", 2},
        {"Media", 3},
        {"Notes", 4},
        {"Cloud", 5}
    };
    
    /* Draw 3x2 grid */
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 3; col++) {
            int idx = row * 3 + col;
            uint16_t x = grid_start_x + col * icon_spacing;
            uint16_t y = grid_start_y + row * icon_spacing;
            nebula_draw_app_icon(x, y, apps[idx].name, apps[idx].icon_type);
        }
    }
}

/* Draw right-side information panel */
void nebula_draw_info_panel(void) {
    uint16_t panel_x = SCREEN_WIDTH - 80;
    uint16_t panel_y = 30;
    uint16_t panel_w = 75;
    uint16_t panel_h = 140;
    
    /* Draw glass panel */
    graphics_draw_glass_panel(panel_x, panel_y, panel_w, panel_h, 128);
    
    /* Draw window controls (top right) */
    graphics_draw_text(panel_x + 5, panel_y + 5, "X", COLOR_WHITE);
    
    /* Draw progress indicator (circle) */
    uint16_t progress_x = panel_x + 15;
    uint16_t progress_y = panel_y + 20;
    graphics_draw_circle(progress_x, progress_y, 15, COLOR_LIGHT_BLUE);
    /* Fill part of circle (simplified - fill arc) */
    for (int angle = 0; angle < 270; angle += 2) {
        /* Use integer approximation for circle arc */
        int px, py;
        if (angle < 90) {
            px = progress_x + (angle * 15) / 90;
            py = progress_y - (angle * 15) / 90;
        } else if (angle < 180) {
            px = progress_x + 15 - ((angle - 90) * 15) / 90;
            py = progress_y - 15 + ((angle - 90) * 15) / 90;
        } else {
            px = progress_x - ((angle - 180) * 15) / 90;
            py = progress_y + ((angle - 180) * 15) / 90;
        }
        if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
            graphics_set_pixel(px, py, COLOR_LIGHT_BLUE);
        }
    }
    
    /* Draw file list */
    const char* files[] = {"Files", "Played Cross", "Piya Dans", "Sppam Soes", "baros"};
    uint16_t file_y = panel_y + 50;
    
    for (int i = 0; i < 5; i++) {
        /* Draw checkbox */
        graphics_draw_rect(panel_x + 5, file_y, 8, 8, COLOR_WHITE);
        if (i == 0 || i == 2) {
            graphics_draw_line_h(panel_x + 6, file_y + 4, 6, COLOR_WHITE);
        }
        
        /* Draw filename */
        graphics_draw_text(panel_x + 15, file_y, files[i], COLOR_WHITE);
        
        file_y += 12;
    }
}

/* Draw dock icon */
void nebula_draw_dock_icon(uint16_t x, uint16_t y, uint8_t icon_type, uint8_t color) {
    uint16_t icon_size = 30;
    graphics_fill_circle(x, y, icon_size / 2, color);
    
    /* Draw icon inside */
    uint8_t icon_color = COLOR_WHITE;
    switch (icon_type) {
        case 0: /* N */
            graphics_draw_text(x - 4, y - 4, "N", icon_color);
            break;
        case 1: /* D */
            graphics_draw_text(x - 4, y - 4, "D", icon_color);
            break;
        case 2: /* M */
            graphics_draw_text(x - 4, y - 4, "M", icon_color);
            break;
        default:
            graphics_fill_circle(x, y, 6, icon_color);
            break;
    }
}

/* Draw bottom dock */
void nebula_draw_dock(void) {
    uint16_t dock_y = SCREEN_HEIGHT - 35;
    uint16_t dock_h = 30;
    
    /* Draw glass panel for dock */
    graphics_draw_glass_panel(0, dock_y, SCREEN_WIDTH, dock_h, 128);
    
    /* Draw dock icons */
    uint16_t icon_spacing = 40;
    uint16_t start_x = 20;
    uint16_t icon_y = dock_y + dock_h / 2;
    
    /* N icon - dark blue */
    nebula_draw_dock_icon(start_x, icon_y, 0, COLOR_BLUE);
    
    /* D icon - purple */
    nebula_draw_dock_icon(start_x + icon_spacing, icon_y, 1, COLOR_MAGENTA);
    
    /* M icon - dark blue */
    nebula_draw_dock_icon(start_x + icon_spacing * 2, icon_y, 2, COLOR_BLUE);
    
    /* Other icons - light blue */
    nebula_draw_dock_icon(start_x + icon_spacing * 3, icon_y, 3, COLOR_LIGHT_BLUE);
    nebula_draw_dock_icon(start_x + icon_spacing * 4, icon_y, 4, COLOR_LIGHT_BLUE);
    
    /* Green icon (Spotify-like) */
    nebula_draw_dock_icon(start_x + icon_spacing * 5, icon_y, 5, COLOR_GREEN);
    
    /* Settings and power icons on right */
    nebula_draw_icon_settings(SCREEN_WIDTH - 40, icon_y, COLOR_WHITE);
    graphics_fill_circle(SCREEN_WIDTH - 20, icon_y, 8, COLOR_WHITE);
}

/* Render complete UI */
void nebula_render_ui(void) {
    /* Draw all components in order */
    nebula_draw_background();
    nebula_draw_top_bar();
    nebula_draw_sidebar();
    nebula_draw_app_grid();
    nebula_draw_info_panel();
    nebula_draw_dock();
}

