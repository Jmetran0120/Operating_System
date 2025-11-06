/* graphics.c - Graphics subsystem implementation for NEBULA OS
 * 
 * Implements VGA Mode 13h graphics with drawing primitives.
 */

#include "graphics.h"

/* Graphics context */
static Graphics gfx;

/* VGA port addresses */
#define VGA_AC_INDEX    0x3C0
#define VGA_AC_WRITE    0x3C0
#define VGA_AC_READ     0x3C1
#define VGA_MISC_WRITE  0x3C2
#define VGA_SEQ_INDEX   0x3C4
#define VGA_SEQ_DATA    0x3C5
#define VGA_DAC_READ    0x3C7
#define VGA_DAC_WRITE   0x3C8
#define VGA_DAC_DATA    0x3C9
#define VGA_GC_INDEX    0x3CE
#define VGA_GC_DATA     0x3CF
#define VGA_CRTC_INDEX  0x3D4
#define VGA_CRTC_DATA   0x3D5

/* Write to VGA port */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ volatile ("outb %0, %1" : : "a" (value), "Nd" (port));
}

/* Read from VGA port */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

/* Wait for VGA to be ready */
static void vga_wait(void) {
    /* Wait for vertical retrace */
    while ((inb(0x3DA) & 0x08));
    while (!(inb(0x3DA) & 0x08));
}

/* Initialize VGA Mode 13h */
void graphics_init(void) {
    /* Setup framebuffer */
    gfx.framebuffer = (uint8_t*) VGA_MEMORY;
    gfx.width = SCREEN_WIDTH;
    gfx.height = SCREEN_HEIGHT;
    
    /* Disable interrupts during mode switch */
    __asm__ volatile ("cli");
    
    /* Unlock CRTC registers */
    outb(VGA_CRTC_INDEX, 0x11);
    uint8_t crtc11 = inb(VGA_CRTC_DATA);
    outb(VGA_CRTC_DATA, crtc11 & ~0x80);
    
    /* Set Mode 13h (320x200, 256 colors) */
    outb(VGA_MISC_WRITE, 0x63);
    
    /* Sequencer registers */
    outb(VGA_SEQ_INDEX, 0x00); outb(VGA_SEQ_DATA, 0x03);
    outb(VGA_SEQ_INDEX, 0x01); outb(VGA_SEQ_DATA, 0x01);
    outb(VGA_SEQ_INDEX, 0x02); outb(VGA_SEQ_DATA, 0x0F);
    outb(VGA_SEQ_INDEX, 0x03); outb(VGA_SEQ_DATA, 0x00);
    outb(VGA_SEQ_INDEX, 0x04); outb(VGA_SEQ_DATA, 0x0E);
    
    /* CRTC registers */
    outb(VGA_CRTC_INDEX, 0x00); outb(VGA_CRTC_DATA, 0x5F);
    outb(VGA_CRTC_INDEX, 0x01); outb(VGA_CRTC_DATA, 0x4F);
    outb(VGA_CRTC_INDEX, 0x02); outb(VGA_CRTC_DATA, 0x50);
    outb(VGA_CRTC_INDEX, 0x03); outb(VGA_CRTC_DATA, 0x82);
    outb(VGA_CRTC_INDEX, 0x04); outb(VGA_CRTC_DATA, 0x54);
    outb(VGA_CRTC_INDEX, 0x05); outb(VGA_CRTC_DATA, 0x80);
    outb(VGA_CRTC_INDEX, 0x06); outb(VGA_CRTC_DATA, 0xBF);
    outb(VGA_CRTC_INDEX, 0x07); outb(VGA_CRTC_DATA, 0x1F);
    outb(VGA_CRTC_INDEX, 0x08); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x09); outb(VGA_CRTC_DATA, 0x41);
    outb(VGA_CRTC_INDEX, 0x0A); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x0B); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x0C); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x0D); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x0E); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x0F); outb(VGA_CRTC_DATA, 0x00);
    outb(VGA_CRTC_INDEX, 0x10); outb(VGA_CRTC_DATA, 0x9C);
    outb(VGA_CRTC_INDEX, 0x11); outb(VGA_CRTC_DATA, 0x8E);
    outb(VGA_CRTC_INDEX, 0x12); outb(VGA_CRTC_DATA, 0x8F);
    outb(VGA_CRTC_INDEX, 0x13); outb(VGA_CRTC_DATA, 0x28);
    outb(VGA_CRTC_INDEX, 0x14); outb(VGA_CRTC_DATA, 0x1F);
    outb(VGA_CRTC_INDEX, 0x15); outb(VGA_CRTC_DATA, 0x96);
    outb(VGA_CRTC_INDEX, 0x16); outb(VGA_CRTC_DATA, 0xB9);
    outb(VGA_CRTC_INDEX, 0x17); outb(VGA_CRTC_DATA, 0xA3);
    
    /* Graphics Controller registers */
    outb(VGA_GC_INDEX, 0x00); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x01); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x02); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x03); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x04); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x05); outb(VGA_GC_DATA, 0x10);
    outb(VGA_GC_INDEX, 0x06); outb(VGA_GC_DATA, 0x0E);
    outb(VGA_GC_INDEX, 0x07); outb(VGA_GC_DATA, 0x00);
    outb(VGA_GC_INDEX, 0x08); outb(VGA_GC_DATA, 0xFF);
    
    /* Attribute Controller - must be set up carefully */
    vga_wait();
    inb(0x3DA);  /* Reset attribute controller */
    
    /* Set attribute controller registers */
    for (uint8_t i = 0; i < 16; i++) {
        outb(VGA_AC_INDEX, i);
        outb(VGA_AC_WRITE, i);
    }
    outb(VGA_AC_INDEX, 0x20);  /* Enable attribute controller */
    
    /* Re-enable interrupts */
    __asm__ volatile ("sti");
    
    /* Setup custom palette */
    graphics_setup_palette();
    
    /* Clear screen */
    graphics_clear(COLOR_BLACK);
}

/* Setup custom color palette for NEBULA OS */
void graphics_setup_palette(void) {
    /* Set up VGA palette - we'll use standard colors plus some custom ones */
    /* For simplicity, we'll map custom colors to available palette indices */
    /* In a real implementation, you'd modify the DAC palette */
}

/* Set a pixel at (x, y) */
void graphics_set_pixel(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return;
    gfx.framebuffer[y * SCREEN_WIDTH + x] = color;
}

/* Get pixel color at (x, y) */
uint8_t graphics_get_pixel(uint16_t x, uint16_t y) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) return 0;
    return gfx.framebuffer[y * SCREEN_WIDTH + x];
}

/* Clear screen */
void graphics_clear(uint8_t color) {
    for (uint32_t i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        gfx.framebuffer[i] = color;
    }
}

/* Draw filled rectangle */
void graphics_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color) {
    for (uint16_t py = y; py < y + h && py < SCREEN_HEIGHT; py++) {
        for (uint16_t px = x; px < x + w && px < SCREEN_WIDTH; px++) {
            graphics_set_pixel(px, py, color);
        }
    }
}

/* Draw rectangle outline */
void graphics_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color) {
    graphics_draw_line_h(x, y, w, color);           /* Top */
    graphics_draw_line_h(x, y + h - 1, w, color);   /* Bottom */
    graphics_draw_line_v(x, y, h, color);           /* Left */
    graphics_draw_line_v(x + w - 1, y, h, color);   /* Right */
}

/* Draw filled circle */
void graphics_fill_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color) {
    for (int16_t dy = -radius; dy <= radius; dy++) {
        for (int16_t dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius) {
                graphics_set_pixel(x + dx, y + dy, color);
            }
        }
    }
}

/* Draw circle outline */
void graphics_draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color) {
    int16_t x0 = 0;
    int16_t y0 = radius;
    int16_t d = 3 - 2 * radius;
    
    while (x0 <= y0) {
        graphics_set_pixel(x + x0, y + y0, color);
        graphics_set_pixel(x - x0, y + y0, color);
        graphics_set_pixel(x + x0, y - y0, color);
        graphics_set_pixel(x - x0, y - y0, color);
        graphics_set_pixel(x + y0, y + x0, color);
        graphics_set_pixel(x - y0, y + x0, color);
        graphics_set_pixel(x + y0, y - x0, color);
        graphics_set_pixel(x - y0, y - x0, color);
        
        if (d < 0) {
            d = d + 4 * x0 + 6;
        } else {
            d = d + 4 * (x0 - y0) + 10;
            y0--;
        }
        x0++;
    }
}

/* Draw horizontal line */
void graphics_draw_line_h(uint16_t x, uint16_t y, uint16_t length, uint8_t color) {
    for (uint16_t i = 0; i < length; i++) {
        graphics_set_pixel(x + i, y, color);
    }
}

/* Draw vertical line */
void graphics_draw_line_v(uint16_t x, uint16_t y, uint16_t length, uint8_t color) {
    for (uint16_t i = 0; i < length; i++) {
        graphics_set_pixel(x, y + i, color);
    }
}

/* Simple 8x8 font data for ASCII characters */
static const uint8_t font_8x8[][8] = {
    /* Space (0x20) */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '!' */
    {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
    /* '"' */
    {0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '#' */
    {0x36, 0x7F, 0x36, 0x36, 0x7F, 0x36, 0x36, 0x00},
    /* '$' */
    {0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00},
    /* '%' */
    {0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00},
    /* '&' */
    {0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00},
    /* ''' */
    {0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* '(' */
    {0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00},
    /* ')' */
    {0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00},
    /* '*' */
    {0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00},
    /* '+' */
    {0x00, 0x0C, 0x0C, 0x7F, 0x0C, 0x0C, 0x00, 0x00},
    /* ',' */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x06, 0x00},
    /* '-' */
    {0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00},
    /* '.' */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00},
    /* '/' */
    {0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00},
    /* '0' */
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00},
    /* '1' */
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00},
    /* '2' */
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00},
    /* '3' */
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00},
    /* '4' */
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00},
    /* '5' */
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00},
    /* '6' */
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00},
    /* '7' */
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00},
    /* '8' */
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00},
    /* '9' */
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00},
    /* ':' */
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00},
    /* ';' */
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x06, 0x00},
    /* '<' */
    {0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00},
    /* '=' */
    {0x00, 0x00, 0x7F, 0x00, 0x00, 0x7F, 0x00, 0x00},
    /* '>' */
    {0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00},
    /* '?' */
    {0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00},
    /* '@' */
    {0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00},
    /* 'A' */
    {0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00},
    /* 'B' */
    {0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00},
    /* 'C' */
    {0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00},
    /* 'D' */
    {0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00},
    /* 'E' */
    {0x7F, 0x06, 0x06, 0x3E, 0x06, 0x06, 0x7F, 0x00},
    /* 'F' */
    {0x7F, 0x06, 0x06, 0x3E, 0x06, 0x06, 0x06, 0x00},
    /* 'G' */
    {0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00},
    /* 'H' */
    {0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00},
    /* 'I' */
    {0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    /* 'J' */
    {0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00},
    /* 'K' */
    {0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00},
    /* 'L' */
    {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x7F, 0x00},
    /* 'M' */
    {0x63, 0x77, 0x7F, 0x6B, 0x63, 0x63, 0x63, 0x00},
    /* 'N' */
    {0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00},
    /* 'O' */
    {0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00},
    /* 'P' */
    {0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x06, 0x00},
    /* 'Q' */
    {0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00},
    /* 'R' */
    {0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00},
    /* 'S' */
    {0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00},
    /* 'T' */
    {0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00},
    /* 'U' */
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x00},
    /* 'V' */
    {0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00},
    /* 'W' */
    {0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00},
    /* 'X' */
    {0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00},
    /* 'Y' */
    {0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00},
    /* 'Z' */
    {0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00},
};

/* Draw text using simple 8x8 font */
void graphics_draw_text(uint16_t x, uint16_t y, const char* text, uint8_t color) {
    uint16_t cx = x;
    uint16_t cy = y;
    
    for (size_t i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        
        if (c == '\n') {
            cx = x;
            cy += 10;  /* Line height */
            continue;
        }
        
        /* Convert lowercase to uppercase */
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        
        if (c < 0x20 || c > 0x5A) {
            cx += 8;  /* Space for unknown characters */
            continue;
        }
        
        /* Get font data for character */
        const uint8_t* font_data = font_8x8[c - 0x20];
        
        /* Draw character */
        for (uint8_t row = 0; row < 8; row++) {
            for (uint8_t col = 0; col < 8; col++) {
                if (font_data[row] & (1 << (7 - col))) {
                    graphics_set_pixel(cx + col, cy + row, color);
                }
            }
        }
        
        cx += 8;  /* Move to next character */
        
        if (cx >= SCREEN_WIDTH - 8) {
            cx = x;
            cy += 10;
        }
    }
}

/* Draw glassmorphism panel (translucent with border) */
void graphics_draw_glass_panel(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t alpha) {
    /* Draw semi-transparent background (simulated by using a lighter color) */
    /* Note: alpha parameter is for future use - real transparency would require blending */
    (void)alpha;  /* Suppress unused parameter warning */
    uint8_t glass_color = COLOR_DARK_GREY;
    graphics_fill_rect(x, y, w, h, glass_color);
    
    /* Draw white border */
    graphics_draw_rect(x, y, w, h, COLOR_WHITE);
}

