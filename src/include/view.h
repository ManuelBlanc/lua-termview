/*!
 * @header View
 * Este modulo se encarga de interactuar con la terminal sobre la que esta
 * corriendo el programa. Internamente, usa ncurses para garantizar la
 * compatibilidad entre los diferentes sistemas operativos y terminales.
 * @version 0.8
 * @author @textblock Manuel Blanc <manuel.blanc@estudiante.uam.es> @/textblock
 * @compilerflag -lpanel
	Extension de paneles de ncurses
 * @compilerflag -lncurses
	Libreria ncurses. Puede que funcione con 'curses' pero no esta garantizado.
 * @updated 2013-11-14
 * @charset utf-8
*/

#ifndef VIEW_H
#define VIEW_H

#include "global.h"

#if 1
/*! @enum KeyCodes
 * Copiado de ncurses.h, puede ser peligroso su uso
 */
enum KeyCodes {
	KEY_DOWN      = 0402, /*! Flecha abajo */
	KEY_UP        = 0403, /*! Flecha arriba */
	KEY_LEFT      = 0404, /*! Flecha izquierda */
	KEY_RIGHT     = 0405, /*! Flecha derecha */
	KEY_MOUSE     = 0631, /*! Evento del raton (no usar) */
	KEY_RESIZE    = 0632  /*! Evento de ventana (no usar) */
};
#endif

/*========================================================
	Enumeraciones & constantes
========================================================*/
/*! Lista de colores para ser usados con FG() o BG(). */
enum _Color {
	BLACK        = 0x00, /*! Negro     = (000, 000, 000) */
	RED          = 0x01, /*! Rojo      = (255, 000, 000) */
	GREEN        = 0x02, /*! Verde     = (000, 255, 000) */
	YELLOW       = 0x03, /*! Amarillo  = (255, 255, 000) */
	BLUE         = 0x04, /*! Azul      = (000, 000, 255) */
	MAGENTA      = 0x05, /*! Magenta   = (255, 000, 255) */
	CYAN         = 0x06, /*! Cyan      = (000, 255, 255) */
	WHITE        = 0x07  /*! Blanco    = (255, 255, 255) */
};
/*! Convierte el color \a col a un color de texto. */
#define FG(COL)     ( (COL) << 8 )
/*! Convierte el color \a col a un color de fondo. */
#define BG(COL)     ( (COL) << 12 )

/*! Opciones de video para los estilos */
enum _Video {
	NORMAL        = 0x000000, /*! Presentacion normal (sin estilo)          	*/
	STANDOUT      = 0x010000, /*! El mejor modo para resaltar de la terminal	*/
	UNDERLINE     = 0x020000, /*! Subrayado                                 	*/
	REVERSE       = 0x030000, /*! Intercambia los colores de texto y fondo  	*/
	BLINK         = 0x040000, /*! Parpadeo (muy molesto, no usar)           	*/
	DIM           = 0x050000, /*! Mitad de intensidad                       	*/
	BOLD          = 0x060000, /*! Doble de intensidad (o negrita)           	*/
	ALTCHARSET    = 0x070000  /*! Set de caracteres alternos (pag. 2)       	*/
};

/*! Extrae el color de texto de un estilo */
#define FG_COLOR(s)  ( (s)>>8 & 0x0F )
/*! Extrae el color de fondo de un estilo */
#define BG_COLOR(s)  ( (s)>>12 & 0x0F )
/*! Extrae el modo de video de un estilo */
#define VID_MODE(s)  ( (s)  & 0xFF0000 )


/*! Convierte un identificador de color de 0 a 7 a un color de texto personalizado. */
#define FG_USR(i)    ( ((i) & 0xF | 0x8) <<  8 )
/*! Convierte un identificador de color de 0 a 7 a un color de fondo personalizado. */
#define BG_USR(i)    ( ((i) & 0xF | 0x8) << 12 )

/*! El equivalente a NULL para los estilos*/
#define STYLE_NONE	((Style) 0)

/*! @brief Tipo de dato para los estilos de texto. */
typedef unsigned long Style;

/*! Una ventana esta definida por (x, y, w, h), y se puede leer y escribir en ella.
 * Antes de crear ventanas, se debe haber empezado el modo grafico, y antes de salir
 * del programa se deberian destruir todas las ventanas para liberar la memoria
 * asociada a estas.
 */
typedef struct View {
	void*	_win;
	void*	_pan;
	Style	_sty;
}* View;

/*! Un lienzo no esta asociada a ninguna parte de la pantalla y se puede usar para
 * guardar informacion visual compleja o reutilizable. Para escribir en un lienzo,
 * se usan las mismas funciones que para una ventana. Hay algunas funciones que no
 * tiene sentido usar sobre un lienzo, y su comportamiento no esta definidio.
 */
typedef View Canvas;

/*! Ventana estandar.
 * Esta variable apunta a la ventana principal, que siempre esta debajo de todas las
 * demas en la pila de ventanas. Por lo general, no esta recomendado interactuar con
 * ella directamente excepto para programa o ejemplos pequeños. Las aplicaciones mas
 * complicadas deben instanciar y usar ventanas nuevas.
 */
extern const View stdview;

/*
 * ========================================================
 *    Funciones globales
 * ========================================================
 */

/*! Inicializa el modo visual.
 * Crear una ventana antes de llamara a esta funcion es un error
 * programatico grave.
 * @return Codigo de error
 */
int viewG_start(void);
/*! Finaliza el modo visual.
 * Para volver al modo visual, basta con llamar a viewG_flush()
 * aunque puede que haya mas funciones que lo restauren.
 * @return Codigo de error
 */
int viewG_end(void);

/*! Actualiza la pantalla de la terminal.
 * Hasta que no se llame a esta funcion, todos los cambios
 * hechos son internos. Esta funcion actualiza la pantalla
 * de la terminal lo mas eficientemente posible, para
 * reflejar rapidamente por pantalla el estado actual.
 * @return Codigo de error
 */
int viewG_flush(void);

/*! Intenta cambiar el tamaño de la terminal.
 * Esta funcion solo funciona cuando el programa esta siendo
 * ejecutado a traves de una terminal de una interfaz grafica
 * (e.g., xwindows). Ademas, puede fallar silenciosamente si
 * la terminal esta en modo pantalla-completa. Su uso no esta
 * recomendado.
 * @param width La anchura objetivo
 * @param height La altura objetivo
 * @return Codigo de error. La funcion fallara si
 * cualquiera de sus dos argumentos es negativo.
 */
int viewG_resizeWindow(int width, int height);

/*! Deslee un caracter
 * El caracter se pone al principio del buffer de entrada. El
 * proximo caracter leido sera este.
 * @param chr Un caracter
 * @return Codigo de error.
 */
int viewG_ungetc(int chr);

/*! Cambia la visibilidad del cursor.
 * @param visib La nueva visibilidad, puede ser 0, 1 o 2
 * @return Codigo de error.
 */
int viewG_cursorVisibility(int visib);


/*! Define un color de usuario
 * @param i Indice del 0 al 7 inclusive
 * @param r Valor rojo  (0-255)
 * @param g Valor verde (0-255)
 * @param b Valor azul  (0-255)
 * @return Codigo de error
 */
int viewG_defColor(int i, unsigned char r, unsigned char g, unsigned char b);

/*! Provoca un destello de la pantalla
 * Si no es posible, hace que suene la camapana (bell)
 */
void viewG_flash(void);

/*! Hace que suene una campana (bell)
 */
void viewG_beep(void);

/*========================================================*/
/*! @group Creacion y destrucion                          */
/*========================================================*/


#define view_new(x, y, w, h)   (view_init(malloc(sizeof(struct View)),(x),(y),(w),(h)))
#define view_delete(v)         (free(view_finalize(v)))
/*======================================================*/

/*! Inicializa una ventana
 * @param v La ventana
 * @param x Posicion x
 * @param y Posicion y
 * @param width La anchura
 * @param height La altura
 * @return la ventana, o NULL en caso de error
 */
View view_init(View v, int x, int y, int width, int height);
/*! Finaliza una ventana para que pueda ser liberada.
 * @param v La ventana
 * @return la ventana
 */
View view_finalize(View v);

/*========================================================*/
/*! @group Posicion y tamaño                              */
/*========================================================*/

/*! Lee la posicion x de una ventana.
 * @param v La ventana
 * @return coordenada x o ERR en caso de error
 */
int view_x(View v);
/*! Lee la posicion y de una ventana.
 * @param v La ventana
 * @return coordenada y o ERR en caso de error
 */
int view_y(View v);
/*! Lee la anchura de una ventana.
 * @param v La ventana
 * @return anchura o ERR en caso de error
 */
int view_w(View v);
/*! Lee la altura de una ventana.
 * @param v La ventana
 * @return altura o ERR en caso de error
 */
int view_h(View v);
/*! Mueve una ventana.
 * Los contenidos de la ventana no cambiaran.
 * Esta funcion fallara si en la nueva posicion
 * habra parte de la ventana que quede fuera de
 * la pantalla.
 * @param v La ventana
 * @param x La nueva coordenada x
 * @param y La nueva coordenada y
 * @return Codigo de error
 */
int view_move(View v, int x, int y);
/*! Cambia las dimensiones de una ventana.
 * Los contenidos de la ventana no cambiaran.
 * Esta funcion fallara si en la nueva posicion
 * habra parte de la ventana que quede fuera de
 * la pantalla. Esta es una operacion muy costosa
 * y debe ser evitada siempre que sea posible.
 * @param v La ventana
 * @param width La nueva anchura
 * @param height La nueva altura
 * @return Codigo de error
 */
int view_resize(View v, int width, int height);

/*========================================================*/
/*! @group Estilos                                        */
/*========================================================*/

/*! Cambia el estilo con el que se pintara.
 * Todas las operaciones de escritura que se hagan
 * sobre la ventana, tendran el estilo especificado.
 * Este estilo tiene precedencia sobre el estilo
 * 'por defecto' de la ventana definido con
 * \c view_setBGStyle()
 * @param v La ventana
 * @param sty El estilo al que cambiar
 * @return Codigo de error
 */
int view_setStyle(View v, Style sty);

/*! Cambia el estilo de fondo.
 * El estilo de fondo cambia todas los caracteres
 * de la ventana que no esten escritos al nuevo
 * estilo. Este estilo, puede estar OR'eado con un
 * caracter ASCII. Se substituiran todas las ocurrencias de
 * el anterior caracter de fondo (normalmente ' ') por el
 * nuevo caracter. Si no se define ningun caracter de fondo,
 * se usara el espacio.
 * @param v La ventana
 * @param sty El estilo al que cambiar
 * @return Codigo de error
 */
int view_setBGStyle(View v, Style sty);

/*! Guarda el ultimo estilo usado en una variable.
 * @param v La ventana
 * @param sty Donde guardar el estilo
 * @return Codigo de error
 */
int view_getStyle(View v, Style* sty);

/*========================================================
	Visibilidad y profundidad
========================================================*/
/*! @group Visibilidad y profundidad */

/*! Esconde una ventana.
 * La ventana pierde la informacion sobre su profundidad.
 * @param v La ventana
 * @return Codigo de error
 */
int view_show(View v);
/*! Muestra una ventana.
 * Ademas, la ventana se mostrara encima de todas las demas.
 * No es necesario llamar a esta funcion para las ventanas
 * nuevas, ya son visibles por defecto.
 * @param v La ventana
 * @return Codigo de error
 */
int view_hide(View v);
/*! Lee la visibilidad de una ventana.
 * @param v La ventana
 * @return o FALSE (o ERR en caso de error)
 */
int view_isHidden(View v);

/*! Manda una ventana al frente.
 * @param v La ventana
 * @return Codigo de error
 */
int view_toTop(View v);
/*! Manda una ventana al fondo.
 * @param v La ventana
 * @return Codigo de error
 */
int view_toBottom(View v);

/*========================================================
	Cursor
========================================================*/
/*! @group Cursor */

/*! Lee la posicion x del cursor.
 * Las coordenadas del cursor son relativas a la ventana.
 * @param v La ventana
 * @return La posicion x o ERR en caso de error
 */
int view_curX(View v);
/*! Lee la posicion y del cursor.
 * Las coordenadas del cursor son relativas a la ventana.
 * @param v La ventana
 * @return La posicion y o ERR en caso de error
 */
int view_curY(View v);
/*! Guarda la posicion x e y del cursor en punteros.
 * Las coordenadas del cursor son relativas a la ventana.
 * @param v La ventana
 * @param x Puntero a donde guardar la coord x
 * @param y Puntero a donde guardar la coord y
 * @return Codigo de error
 */
int view_getCursor(View v, int* x, int* y);
/*! Cambia la posicion del cursor a la posicion objetivo.
 * Las coordenadas del cursor son relativas a la ventana.
 * Fallara si las coordenadas estan fuera de la ventana.
 * @param v La ventana
 * @param x Nueva coordenada x
 * @param y Nueva coordenada y
 * @return Codigo de error
 */
int view_setCursor(View v, int x, int y);
/*! Desplaza el cursor.
 * Si la nueva posicion del cursor se sale por el lado
 * derecho de la ventana, el cursor se movera al primer
 * caracter de la siguiente linea. Lo mismo ocurrira para
 * el lado izquierdo. Si el cursor se sale por abajo,
 * dependendera del estado de scroll de la ventana.
 * @param v La ventana
 * @param dx El desplazamiento en la x
 * @param dy El desplazamiento en la y
 * @return Codigo de error
 */
int view_moveCursor(View v, int dx, int dy);

/*========================================================
	Lineas y bordes
========================================================*/
/*! @group Lineas y bordes */

/*! Pinta un borde interno en una view.
 * @param v La ventana
 * @return Codigo de error
 */
int view_border(View v);
/*! Pinta una linea vertical.
 * La linea empieza en las coordenadas x e y
 * y mide len caracteres. Si la terminal no
 * permite el caracter 'linea vertical', se utilizara
 * un substituto adecuado ASCII (\). Cuando finalize esta
 * funcion, la posicion del cursor sera la de las
 * coordenadas de inicio.
 * @param v La ventana
 * @param x La coordenada x de inicio
 * @param y la coordenada y de inicio
 * @param len La longitud de la linea
 * @return Codigo de error
 */
int view_vline(View v, int x, int y, int len);
/*! Pinta una linea horizontal.
 * La linea empieza en las coordenadas x e y
 * y mide len caracteres. Si la terminal no
 * permite el caracter 'linea horizontal', se utilizara
 * un substituto adecuado ASCII (-). Cuando finalize esta
 * funcion, la posicion del cursor sera la de las
 * coordenadas de inicio.
 * @param v La ventana
 * @param x La coordenada x de inicio
 * @param y la coordenada y de inicio
 * @param len La longitud de la linea
 * @return Codigo de error
 */
int view_hline(View v, int x, int y, int len);

/*========================================================
	Misc.
========================================================*/
/*! @group Misc. */

/*! Marca una ventana para que sea vuelta a pintar.
 * Esta funcion sirve para marcar una ventana como
 * cambiada para que se recalculen sus solapamientos
 * y sea repintada. Normalmente no es necesario llamarla
 * directamente, ya que cuando se escribe en una ventana
 * se marca como 'cambiada' automaticamente. No obstante,
 * hay situaciones en las que puede ser util si ha
 * ocurrido algo que ha podido corromper el estado
 * interno de la ventana (como por ejemplo, una
 * interrupcion).
 * @return Codigo de error
 */
int view_touch(View v);
/*! Borra una ventana.
 * Hasta que no se llame a viewG_flush, la ventana
 * seguira en la pantalla.
 * @param v La ventana
 * @return Codigo de error
 */
int view_clear(View v);
/*! Mueve el cursor a una linea y la borra.
 * @param v La ventana
 * @param y La linea a borrar
 * @return Codigo de error
 */
int view_clearLine(View v, int y);
/*! Activa/desactiva el scrolling.
 * @param v La ventana
 * @param b Un booleano, TRUE o FALSE
 * @return Codigo de error
 */
int view_setScroll(View v, int b);
/*! Fuerza un scroll de varias lineas.
 * @param v La ventana
 * @param n El numero de lineas del scroll
 * @return Codigo de error
 */
int view_scroll(View v, int n);
/*! Copia una ventana sobre otra.
 * Intentara copiar una ventana sobre otra.
 * Esta funcion es nueva y no ha sido probada a
 * fondo. Ademas, debido a su implementacion tiene
 * algunas peculariades con las que hay que tener
 * cuidado. Su uso no esta recomendado.
 * @param dst La ventana destino
 * @param src La ventana origen
 * @param sx La coordenada x desde la que empezar a
 * copiar desde origen
 * @param sy La coordenada y desde la que empezar a
 * copiar desde origen
 * @param dx La coordenada x a la que copiar del destino
 * @param dy La coordenada y a la que copiar del destino
 * @param dw La anchura maxima que copiar al destino
 * @param dh La altura  maxima que copiar al destino
 * @return Codigo de error
 */
int view_copy(View dst, View src, int sx, int sy, int dx, int dy, int dw, int dh);

/*========================================================
	Entrada
========================================================*/
/*! @group Entrada */


/*! Cambia el bloqueo al leer caracteres.
 * Si el retardo es no-negativo, se esperara esa cantidad de decimas
 * de segundo hasta que haya entrada disponible. Si despues de ese
 * tiempo sigue sin haber, se devolvera ERR.
 * Con un retardo negativo, se bloquea el proceso al leer un caracter
 * hasta que haya uno disponible. Este es el modo por defecto.
 * @param v La ventana
 * @param delay Un retardo en decimas de segundo
 * @return Codigo de error
 */
int view_setTimeout(View v, int delay);


/*! Lee un caracter de pantalla.
 * El caracter leido no se imprimira por pantalla.
 * @param v La ventana
 * @return El caracter leido (o ERR)
 */
int view_getc(View v);

/*! Lee una cadena de caracteres.
 * Como maximo se leeran len caracteres y no
 * permitira que se escriban mas. En el modo de
 * escritura, funcionaran los atajos definidos
 * en la terminal como ^U o ^W.
 * @param v La ventana
 * @param str Donde guardar la cadena
 * @param len Longitud maxima de la cadena
 * @return Codigo de error
 */
char* view_gets(View v, char* str, int len);

/*! Lee entrada usando el formato proporcionado.
 * Esta funcion opera de la misma manera que fscanf
 * de la biblioteca estandar, usando los mismos formatos.
 * @param v La ventana
 * @param fmt El formato de la entrada
 * @return El numero de parametros leidos, o ERR
 */
int view_scanf(View v, char* fmt, ...)
ATTR((format(scanf, 2, 3)));

/*========================================================*/
/*! @group Salida                                         */
/*========================================================*/

/*! Inserta un caracter en la posicion del cursor.
 * @param v La ventana
 * @param chr Caracter a insertar (puede contener un estilo)
 * @return Codigo de error
 */
int view_insc(View v, int chr);

/*! Escribe un caracter.
 * @param v La ventana
 * @param chr Caracter a escribir (puede contener un estilo)
 * @return Codigo de error
 */
int view_putc(View v, int chr);

/*! Escribe una cadena.
 * @param v La ventana
 * @param str Cadena a escribir
 * @return Codigo de error
 */
int view_puts(View v, const char* str);

/*! Imprime datos usando el formato proporcionado.
 * Esta funcion opera de la misma manera que fprintf
 * de la biblioteca estandar, usando los mismos formatos.
 * @param v La ventana
 * @param fmt El formato de la salida
 * @return Codigo de error
 */
int view_printf(View v, const char* fmt, ...)
ATTR((format(printf, 2, 3)));

/*========================================================*/
/*! @group Lienzos                                        */
/*========================================================*/

/*! Crea un nuevo lienzo.
 * @param width Dimension x
 * @param height Dimension y
 * @return El lienzo creado o NULL en caso de error
 */
Canvas canvas_new(int width, int height);

/*! Destruye un lienzo. Los lienzos deben ser destruidos asi.
 * @param canvas Lienzo
 * @return devuelve NULL esta funcion
 */
Canvas canvas_delete(Canvas canvas);
#define canvas_delete view_delete

/*! Copia los contenidos de un lienzo a una ventana.
 * @param src El lienzo
 * @param dst La ventana a la que copiar
 * @param x La coordenada x del liezo
 * @param y La coordenada y del lienzo
 * @return Codigo de error
 */
int canvas_drawTo(Canvas src, View dst, int x, int y);

#endif /* VIEW_H */
