/*#HEAD*/

/* user */
#include "global.h"
#include "view.h"
/* libc */
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
/* (n)?curses */
#include <curses.h>
#include <panel.h>

/*
** ==============================================
**     DATOS INTERNOS
** ==============================================
*/

/* Macros de acceso a la estructura */
#define view_win(v) ( (WINDOW*) ( (v) ? (v)-> _win : NULL ) )
#define view_pan(v) ( (PANEL*)  ( (v) ? (v)-> _pan : NULL ) )
#define view_sty(v)             ( (v) ? (v)-> _sty :   0L )

/* Anchura y altura de la terminal */
PRIVATE int TERM_W = 0, TERM_H = 0;

/* Estructura de la ventana estandar */
PRIVATE struct View _stdview = {NULL, NULL, 0};
const View stdview = &_stdview;

/* Bandera que guarda el 'nivel de color' la terminal  */
PRIVATE int colorSupport = 0;
/* Tabla de traduccion estilo a attributo de curses */
PRIVATE unsigned vid2attr[] = {
	  A_NORMAL
	, A_STANDOUT
	, A_UNDERLINE
	, A_REVERSE
	, A_BLINK
	, A_DIM
	, A_BOLD
	, A_ALTCHARSET
};

/* Macro para extraer un byte*/
#define GET_BYTE(integer,byte) ( (integer) >> (8*byte) & 0xFF )

/* Devuelve el minimo etre dos numeros */
PRIVATE INLINE int MIN(int x, int y) { return x < y ? x : y; }

/* Esta funcion convierte un estilo a un caracter */
PRIVATE chtype viewP_style2chtype(Style s)
{
	chtype ch = 0;
	ch |= GET_BYTE(s, 0);
	ch |= vid2attr[GET_BYTE(s, 2) & 0x0F];
	if (colorSupport) ch |= COLOR_PAIR(GET_BYTE(s, 1));
	return ch;
}

/* Todo es relativo */
PRIVATE int viewP_posrelat(int x, int m)
{
	     if (x < 0) return x += m;
	else if (x > m) return x += m;
	else            return x;
}

/* Modos de entrada */
enum { COOKED, CBREAK };
PRIVATE void viewP_cbreak(bool b)
{
	if (b) {   cbreak(); noecho(); nonl(); }
	else   { nocbreak();   echo();   nl(); }
}

PRIVATE void viewP_initColor()
{
	short bg, fg, i;
	colorSupport = (has_colors() << 0) | (can_change_color() << 1);
	if (!colorSupport) return;
	start_color();
	/* Colores por defecto en el '0' */
	use_default_colors();
	init_pair(0, -1, -1);
	/* Paleta de colores basicos, definidos en la cabezera */
	init_color(0,    0,    0,    0); /* BLACK    */
	init_color(1, 1000,    0,    0); /* RED      */
	init_color(2,    0, 1000,    0); /* GREEN    */
	init_color(3, 1000, 1000,    0); /* YELLOW   */
	init_color(4,    0,    0, 1000); /* BLUE     */
	init_color(5, 1000,    0, 1000); /* MAGENTA  */
	init_color(6,    0, 1000, 1000); /* CYAN     */
	init_color(7, 1000, 1000, 1000); /* WHITE    */
	/* Inicializacion colores de usuario */
	for (i = 0; i < 8; i++) viewG_defColor(i, 0, 0, 0);
	/* Creacion de paleta de colores (256 en total) */
	for (bg=0x0; bg <= 0xF; bg++)
	for (fg=0x0; fg <= 0xF; fg++) {
		if (fg || bg) init_pair((short)((bg<<4)|fg), fg, bg);
	}
}

/* Callback para los cambios de tamaño de ventana */
PRIVATE void viewP_sigwinch(int i)
{
	UNUSED(i);
	viewG_resizeWindow(TERM_W, TERM_H);
	endwin();
	doupdate();
}


/*
** ==============================================
**     FUNCIONES GLOBALES
** ==============================================
*/

/*#DOC*/
int viewG_start()
{
	if (!initscr()) return ERR;
	viewP_cbreak(FALSE);
	signal(SIGWINCH, viewP_sigwinch);
	keypad(stdscr, TRUE);
	intrflush(stdscr, FALSE);
	_stdview._win = stdscr;
	_stdview._pan = NULL;
	TERM_W = getmaxx(stdscr);
	TERM_H = getmaxy(stdscr);
	viewP_initColor();
	return OK;
}


/*#DOC*/
int viewG_end()
{
	return endwin();
}

/*#DOC*/
int viewG_flush()
{
	if (NULL == view_win(stdview)) return ERR;
	update_panels();
	return doupdate();
}

/*#DOC*/
int viewG_resizeWindow(int w, int h)
{
	if (is_term_resized(h, w)) return ERR;
	if (OK == resizeterm(h, w)) {
		TERM_W = w; TERM_H = h;
		return OK;
	}
	return ERR;
}

/*#DOC*/
int viewG_ungetc(int c)
{
	return ungetch(c);
}

/*#DOC*/
int viewG_cursorVisibility(int visib)
{
       if (ERR == curs_set(visib)) return ERR;
       return OK;
}


/*#DOC*/
int viewG_defColor(int i, unsigned char r, unsigned char g, unsigned char b)
{
	short sr, sg, sb;
	if (2 != colorSupport) return ERR;
	sr = (short) ((double)r / 255.0 * 999.0);
	sg = (short) ((double)g / 255.0 * 999.0);
	sb = (short) ((double)b / 255.0 * 999.0);
	return init_color(i&0xF, sr, sg, sb);
}

/*#DOC*/
void viewG_flash()
{
	flash();
}

/*#DOC*/
void viewG_beep()
{
	beep();
}

/* = INITIALIZATION & FINALIZATION = */
/*#DOC*/
View view_init(View v, int x, int y, int w, int h)
{
	if (NULL == v) return NULL;
	/* Allow relative positions */
	x = viewP_posrelat(x, TERM_W);
	y = viewP_posrelat(y, TERM_H);
	w = viewP_posrelat(w, TERM_W - x);
	h = viewP_posrelat(h, TERM_H - y);

	v->_win = newwin(h, w, y, x);
	v->_pan = new_panel(view_win(v));
	v->_sty = 0;
	if (NULL == view_win(v) || NULL == view_pan(v)) {
		view_delete(v); return NULL;
	} /* set_panel_userptr(view_pan(v), v); /@* Loopback pointer */
	keypad(view_win(v), TRUE);
	return v;
}

/*#DOC*/
View view_finalize(View v)
{
	if (NULL == v || stdview == v) return NULL;
	del_panel(view_pan(v)); delwin(view_win(v));
	return v;
}

/*
** ==============================================
**     POSITION & SIZE
** ==============================================
*/

/*#DOC*/
int view_x(View v) { return getbegx(view_win(v));
}
/*#DOC*/
int view_y(View v) { return getbegy(view_win(v));
}
/*#DOC*/
int view_w(View v) { return getmaxx(view_win(v));
}
/*#DOC*/
int view_h(View v) { return getmaxy(view_win(v));
}


/*#DOC*/
int view_move(View v, int x, int y)
{
	x = viewP_posrelat(x, TERM_W);
	y = viewP_posrelat(y, TERM_H);
	return move_panel(view_pan(v), y, x);
}

/*#DOC*/
int view_resize(View v, int w, int h)
{
	w = viewP_posrelat(w, TERM_W);
	h = viewP_posrelat(h, TERM_H);
	if (ERR == wresize(view_win(v), h, w)) return ERR;
	return replace_panel(view_pan(v), view_win(v));
}

/*
** ==============================================
**     STYLING
** ==============================================
*/

/*#DOC*/
int view_setBGStyle(View v, Style s)
{
	return wbkgd(view_win(v), viewP_style2chtype(s));
}

/*#DOC*/
int view_setStyle(View v, Style s)
{
	if (NULL == v) return ERR;
	if (OK == wattrset(view_win(v), (int)viewP_style2chtype(s))) {
		v->_sty = s;
		return OK;
	}
	return ERR;
}

/*#DOC*/
int view_getStyle(View v, Style* s)
{
	if (NULL == v) return ERR;
	*s = view_sty(v); return OK;
}

/*
** ==============================================
**     VISIBILITY
** ==============================================
*/

/*#DOC*/
int view_show(View v)
{
	return show_panel(view_pan(v));
}

/*#DOC*/
int view_hide(View v)
{
	return hide_panel(view_pan(v));
}

/*#DOC*/
int view_isHidden(View v)
{
	return panel_hidden(view_pan(v));
}

/*
** ==============================================
**     DEPTH
** ==============================================
*/

/*#DOC*/
int view_toTop(View v)
{
	return top_panel(view_pan(v));
}

/*#DOC*/
int view_toBottom(View v)
{
	return bottom_panel(view_pan(v));
}

/*
** ==============================================
**     CURSOR
** ==============================================
*/

/*#DOC*/
int view_curX(View v)
{
	return getcurx(view_win(v));
}

/*#DOC*/
int view_curY(View v)
{
	return getcury(view_win(v));
}

/*#DOC*/
int view_getCursor(View v, int* x, int* y)
{
	if (NULL == v || NULL == x || NULL == y) return ERR;
	getyx(view_win(v), *y, *x);
	return OK;
}

/*#DOC*/
int view_setCursor(View v, int x, int y)
{
	if (x < 0) x += view_w(v);
	if (y < 0) y += view_h(v);
	return wmove(view_win(v), y, x);
}

/*#DOC*/
int view_moveCursor(View v, int dx, int dy)
{
	int w, h;
	if (NULL == v) return ERR;
	w = view_w(v);
	h = view_h(v);
	dx += getcurx(view_win(v));
	dy += getcury(view_win(v));
	while (dx < 0) {
		/* Si el destino x es negativo,
		** 'wrappeamos' al otro lado */
		dy -= 1;
		dx += w;
	}
	while (dx > w) {
		/* Igual para el lado derecho de la pantalla */
		dy += 1;
		dx -= w;
	}
	if (dy < 0) dy = 0;
	if (dy > h) dy = h;
	return wmove(view_win(v), dy, dx);
}

/*
** ==============================================
**     BORDER
** ==============================================
*/

/*#DOC*/
int view_border(View v)
{
	return box(view_win(v), 0, 0);
}

/*#DOC*/
int view_vline(View v, int x, int y, int n)
{
	x = viewP_posrelat(x, view_w(v));
	y = viewP_posrelat(y, view_h(v));
	return mvwvline(view_win(v), y, x, ACS_VLINE, n);
}

/*#DOC*/
int view_hline(View v, int x, int y, int n)
{
	x = viewP_posrelat(x, view_w(v));
	y = viewP_posrelat(y, view_h(v));
	return mvwhline(view_win(v), y, x, ACS_HLINE, n);
}

/*
** ==============================================
**     INPUT
** ==============================================
*/

/*#DOC*/
int view_setTimeout(View v, int delay)
{
	wtimeout(view_win(v), delay);
	return OK;
}

/*#DOC*/
int view_getc(View v)
{
	viewP_cbreak(TRUE);
	return wgetch(view_win(v));
}

/*#DOC*/
char* view_gets(View v, char* s, int n)
{
	viewP_cbreak(FALSE);
	if (OK == wgetnstr(view_win(v), s, n)) return s;
	return NULL;
}

/*#DOC*/
int view_scanf(View v, char* fmt, ...)
{
	va_list        ap;
	int            ret;
	viewP_cbreak(FALSE);
	va_start(ap, fmt);
	ret = vw_scanw(view_win(v), fmt, ap);
	va_end(ap);
	return ret;
}

/*
** ==============================================
**     OUTPUT
** ==============================================
*/

/*#DOC*/
int view_insc(View v, int chr)
{
	return winsch(view_win(v), viewP_style2chtype((Style) chr));
}

/*#DOC*/
int view_putc(View v, int chr)
{
	return waddch(view_win(v), viewP_style2chtype((Style) chr));
}

/*#DOC*/
int view_puts(View v, const char* s)
{
	return waddstr(view_win(v), s);
}

/*#DOC*/
int view_printf(View v, const char *fmt, ...)
{
	va_list        ap;
	int            ret;
	va_start(ap, fmt);
	ret = vw_printw(view_win(v), fmt, ap);
	va_end(ap);
	return ret;
}

/*
** ==============================================
**     MISC
** ==============================================
*/

/*#DOC*/
int view_touch(View v)
{
	return touchwin(view_win(v));
}

/*#DOC*/
int view_clear(View v)
{
	return werase(view_win(v));
}

/*#DOC*/
int view_clearLine(View v, int y)
{
	wmove(view_win(v), y, 0);
	return wclrtoeol(view_win(v));
}

/*#DOC*/
int view_setScroll(View v, int b)
{
	return scrollok(view_win(v), b);
}

/*#DOC*/
int view_scroll(View v, int n)
{
	return wscrl(view_win(v), n);
}
int view_copy(View d, View s, int sx, int sy, int dx, int dy, int dw, int dh) {
	return copywin(view_win(s), view_win(d), sy, sx, dy, dx, dh-1, dw-1, 0);
}

/*
** ==============================================
**     CANVAS
** ==============================================
*/

/*#DOC*/
Canvas canvas_new(int w, int h)
{
	Canvas c = malloc(sizeof(struct View));
	if (NULL == c) return NULL;
	c->_win = newpad(h, w);
	c->_pan = NULL;
	c->_sty = (Style) 0;
	if (NULL == view_win(c)) {
		free(c);
		return NULL;
	}
	return c;
}

/*#DOC*/
int canvas_drawTo(Canvas c, View d, int x, int y)
{
	int w = MIN( view_w(c)-x-1, view_w(d)-1 );
	int h = MIN( view_h(c)-y-1, view_h(d)-1 );
	return copywin(view_win(c), view_win(d), y, x, 0, 0, h, w, 0);
}
