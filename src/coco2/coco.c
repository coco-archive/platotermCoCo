#include <stdio.h>
#include <string.h>
#include <serial.h>
#include <stdint.h>
#include <protocol.h>
#include <mouse.h>
#include <tgi.h>

extern unsigned char font[];
extern unsigned char fontm23[];
extern unsigned char FONT_SIZE_X;
extern unsigned char FONT_SIZE_Y;
extern unsigned short scalex[];
extern unsigned short scaley[];
extern unsigned int fontptr[];


extern void (*io_recv_serial_flow_off)(void);
extern void (*io_recv_serial_flow_on)(void);

extern padBool FastText;

#define screen ((unsigned char *)0x6000)


void noop(void) {
}

/********************
stdlib.h API
*********************/
int atoi(const char *nptr)
{
}

/********************
stdio.h API
*********************/
FILE *fopen(const char *pathname, const char *mode)
{
    return NULL;
}


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb,
	      FILE *stream)
{
    return 0;
}

int fclose(FILE *stream)
{
    return -1;
}


/********************
 string.h API
*********************/
void *memset(void *s, int c, size_t n)
{
    unsigned char *p = (unsigned char *)s;
    while (n--) *p++ = c;
}

size_t strlen (const char* s)
{
    char *e = (char *)s;
    while (*e++);
    return e-s;
}

int tolower (int c)
{
    if (c >= 'A' || c <= 'Z')
	return c + 40;
    return c;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1) {
	if (*s1++ != *s2++)
	    return -1;
    }
    return 0;
}

/********************
 PlatoTerm API
*********************/


void config_init_hook(void)
{
}

void config_set_defaults(void)
{
}

void screen_update_colors(void)
{
}

void io_init_funcptrs(void)
{
    io_recv_serial_flow_off = noop;
    io_recv_serial_flow_on = noop;
}

void io_send_byte(uint8_t b)
{
    ser_put_clean(b);
}

void prefs_driver(void)
{
}

void prefs_touch(void)
{
}

void screen_beep(void)
{
}

void screen_char_draw(padPt* Coord, unsigned char* ch, unsigned char count)
{
    int16_t offset; /* due to negative offsets */
    uint16_t x;      /* Current X and Y coordinates */
    uint16_t y;
    uint16_t* px;   /* Pointers to X and Y coordinates used for actual plotting */
    uint16_t* py;
    uint8_t i; /* current character counter */
    uint8_t a; /* current character byte */
    uint8_t j,k; /* loop counters */
    int8_t b; /* current character row bit signed */
    uint8_t width = FONT_SIZE_X;
    uint8_t height = FONT_SIZE_Y;
    uint16_t deltaX = 1;
    uint16_t deltaY = 1;
    uint8_t mainColor = 1;
    uint8_t altColor = 0;
    uint8_t *p;
    uint8_t* curfont;
    switch(CurMem) {
    case M0:
	curfont = font;
	offset = -32;
	break;
    case M1:
	curfont = font;
	offset = 64;
	break;
    case M2:
	curfont = fontm23;
	offset = -32;
	break;
    case M3:
	curfont = fontm23;
	offset = 32;
	break;
    }

    if (CurMode == ModeRewrite)
	altColor = 0;
    else if (CurMode == ModeInverse)
      altColor = 1;

    if (CurMode == ModeErase || CurMode == ModeInverse)
	mainColor = 0;
    else
	mainColor = 1;

    tgi_setcolor(mainColor);

    x = scalex[Coord->x & 0x1FF];
    y = scaley[(Coord->y + 15) & 0x1FF];

    if (FastText == padF)
    	goto chardraw_with_fries;

    /* the diet chardraw routine - fast text output. */
    for (i = 0; i < count; ++i) {
	a = *ch;
	++ch;
	a += offset;
	p = &curfont[fontptr[a]];
	for (j = 0; j < FONT_SIZE_Y; ++j) {
	    b = *p;
	    for (k = 0; k < FONT_SIZE_X; ++k) {
		if (b < 0) { /* check sign bit. */
		    tgi_setcolor(mainColor);
		    tgi_setpixel(x, y);
		}
		++x;
		b <<= 1;
	    }
	    ++y;
	    x -= width;
	    ++p;
	}
	x += width;
	y -= height;
    }
    return;

 chardraw_with_fries:
    if (Rotate) {
	deltaX = -abs(deltaX);
	width = -abs(width);
	px = &y;
	py = &x;
    }
    else {
	px = &x;
	py = &y;
    }
    if (ModeBold) {
	deltaX = deltaY = 2;
	width <<= 1;
	height <<= 1;
    }
    for (i = 0; i < count; ++i) {
	a = *ch;
	++ch;
	a += offset;
	p = &curfont[fontptr[a]];
	for (j = 0; j < FONT_SIZE_Y; ++j) {
	    b = *p;
	    if (Rotate) {
		px = &y;
		py = &x;
	    }
	    else {
		px = &x;
		py = &y;
	    }
	    for (k = 0; k < FONT_SIZE_X; ++k) {
		if (b < 0) { /* check sign bit. */
		    tgi_setcolor(mainColor);
		    if (ModeBold) {
			tgi_setpixel(*px + 1, *py);
			tgi_setpixel(*px, *py + 1);
			tgi_setpixel(*px + 1, *py + 1);
		    }
		    tgi_setpixel(*px, *py);
		}
		else {
		    if (CurMode == ModeInverse || CurMode == ModeRewrite) {
			tgi_setcolor(altColor);
			if (ModeBold) {
			    tgi_setpixel(*px + 1, *py);
			    tgi_setpixel(*px, *py + 1);
			    tgi_setpixel(*px + 1, *py + 1);
			}
			tgi_setpixel(*px, *py);
		    }
		}
		x += deltaX;
		b <<= 1;
	    }
	    y += deltaY;
	    x -= width;
	    ++p;
	}
	Coord->x += width;
	x += width;
	y -= height;
    }
    return;
}

void prefs_show_greeting(void)
{
}


void screen_load_driver(void)
{
}

void screen_init_hook(void)
{
    /* 256x192 bw mode green/light green */
    *(unsigned char *)0xff22 = 0xf0;
    *(unsigned char *)0xffc0 = 0x00;
    *(unsigned char *)0xffc3 = 0x00;
    *(unsigned char *)0xffc5 = 0x00;
    /* screen ram address to 0x6000 */
    *(unsigned char *)0xffc6 = 0x00;
    *(unsigned char *)0xffc8 = 0x00;
    *(unsigned char *)0xffca = 0x00;
    *(unsigned char *)0xffcc = 0x00;
    *(unsigned char *)0xffcf = 0x00;
    *(unsigned char *)0xffd1 = 0x00;
    *(unsigned char *)0xffd2 = 0x00;
}

/*
void keyboard_main(void)
{
}

void keyboard_clear(void)
{
}
*/

void terminal_char_load(padWord charnum, charData theChar)
{
}



/**********************
 Misc CC65 libs
***********************/
unsigned char ser_ioctl (unsigned char code, void* data)
{
}

unsigned char ser_uninstall (void)
{
}

unsigned char ser_unload (void)
{
}

unsigned char ser_load_driver (const char* driver)
{
    return 0;
}

char cgetc (void)
{
}

void screen_wait(void)
{
}

unsigned char mouse_unload (void)
{
}

unsigned char mouse_load_driver (const struct mouse_callbacks* c,
				 const char* driver)
{
}

void mouse_show (void)
{
}

void mouse_move (int x, int y)
{
}

unsigned char mouse_uninstall (void)
{
}

const struct mouse_callbacks mouse_def_callbacks;
const struct mouse_info mouse_info;

/**********************
 TGI API
***********************/

unsigned int pen = 0xff;


// fixme: do most of this stuff in asm

unsigned char tgi_getcolor (void)
{
}

void tgi_setcolor (unsigned char color)
{
    pen = color;
}

void tgi_bar (int x1, int y1, int x2, int y2)
{
    int d;
    int h;
    if (y1 < y2){
        d = 1;
        h = y2 - y1;
    } else {
        d = -1;
        h = y1 - y2;
    }
    do {
        tgi_hline(x2,y1,x1);
        y1 += d;
    } while (h--);   
}

void tgi_init (void)
{
}

void tgi_clear (void)
{
    memset(screen, 0, 32*192);
}

void tgi_setpixel (int x, int y)
{
    //    unsigned char *a = screen + y * 32 + x/8;
    //*a |= 0x80 >> (x & 7);
    unsigned char mask;
    int off;

    mask = 0x80 >> (x & 7);
    x >>= 3;
    off = y * 32 + x;
    /* for 1 bpp this works, but should be
       more like: screen = (screen & ~mask) | (color & mask) */
    if (pen){
        screen[off] = screen[off] | mask;
    }
    else
        screen[off] = screen[off] & ~mask;
}

void tgi_line (int x1, int y1, int x2, int y2)
{
    int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */

    for(;;){  /* loop */
	tgi_setpixel(x1,y1);
	if (x1==x2 && y1==y2) break;
	e2 = 2*err;
	if (e2 >= dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
	if (e2 <= dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
    }
}

void tgi_hline (int x1, int y1, int x2)
{
    int t;
    if (x1 > x2){
	t = x1;
	x1 = x2;
	x2 = t;
    }
    while(x1 <= x2)
	tgi_setpixel(x1++, y1);
}

void tgi_done (void)
{
}

void tgi_uninstall (void)
{
}
