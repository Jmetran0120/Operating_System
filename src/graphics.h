/* graphics.h - Graphics subsystem for NEBULA OS
 * 
 * Provides VGA graphics mode (Mode 13h - 320x200, 256 colors) support
 * and drawing functions for the NEBULA OS interface.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

/* Standard integer types */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef uint32_t           size_t;

/* VGA Mode 13h constants */
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define VGA_MEMORY    0xA0000     /* VGA graphics memory address */
#define NUM_COLORS    256

/* Color definitions - VGA palette colors */
#define COLOR_BLACK       0x00
#define COLOR_BLUE        0x01
#define COLOR_GREEN       0x02
#define COLOR_CYAN        0x03
#define COLOR_RED         0x04
#define COLOR_MAGENTA     0x05
#define COLOR_BROWN       0x06
#define COLOR_LIGHT_GREY  0x07
#define COLOR_DARK_GREY   0x08
#define COLOR_LIGHT_BLUE  0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN  0x0B
#define COLOR_LIGHT_RED   0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW      0x0E
#define COLOR_WHITE       0x0F

/* NEBULA OS color palette (custom colors) */
#define COLOR_NEBULA_DARK_BLUE   0x20
#define COLOR_NEBULA_PURPLE      0x21
#define COLOR_NEBULA_BLUE        0x22
#define COLOR_NEBULA_LIGHT_BLUE  0x23
#define COLOR_NEBULA_WHITE       0x0F
#define COLOR_GLASS_TRANSPARENT  0x24
#define COLOR_GLASS_BORDER       0x0F

/* Graphics context structure */
typedef struct {
    uint8_t* framebuffer;    /* Pointer to VGA framebuffer */
    uint16_t width;           /* Screen width */
    uint16_t height;          /* Screen height */
} Graphics;

/* Initialize VGA Mode 13h (320x200, 256 colors) */
void graphics_init(void);

/* Set a pixel at (x, y) to color */
void graphics_set_pixel(uint16_t x, uint16_t y, uint8_t color);

/* Get pixel color at (x, y) */
uint8_t graphics_get_pixel(uint16_t x, uint16_t y);

/* Clear screen with color */
void graphics_clear(uint8_t color);

/* Draw a filled rectangle */
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);

/* Draw a rectangle outline */
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color);

/* Draw a filled circle */
void graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color);

/* Draw a circle outline */
void graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color);

/* Draw a horizontal line */
void graphics_draw_line_h(uint16_t x, uint16_t y, uint16_t length, uint8_t color);

/* Draw a vertical line */
void graphics_draw_line_v(uint16_t x, uint16_t y, uint16_t length, uint8_t color);

/* Draw text (simple 8x8 font) */
void graphics_draw_text(uint16_t x, uint16_t y, const char* text, uint8_t color);

/* Draw a translucent (glassmorphism) panel */
void graphics_draw_glass_panel(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alpha);

/* Setup custom color palette */
void graphics_setup_palette(void);

#endif /* GRAPHICS_H */

