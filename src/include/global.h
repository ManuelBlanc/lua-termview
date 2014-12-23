
#ifndef GLOBAL_H
#define GLOBAL_H

/*===============================*/
/*         MEMORIA               */
/*===============================*/
#include <stdlib.h>

/*===============================*/
/*         Tipos de datos         */
/*===============================*/

/*! Definicion de booleanos */
#if !defined(bool) && !defined(TRUE) && !defined(FALSE)
typedef enum { FALSE = 0, TRUE  = 1 } bool;
#endif

/*! NULL no es parte del lenguaje, y tenemos que definirlo por si acaso
 * no estan incluidas ninguna de las cabezeras stdlib.h o string.h
 */
#if !defined(NULL)
#define NULL ((void*) 0)
#endif

/*! Codigos de error */
enum ErrCode { OK =  0, ERR = -1 };

#define SUCCESS(code)	((code) >= OK)
#define FAILED(code) 	((code) <  OK)

/*===============================*/
/*         Macros utiles         */
/*===============================*/

/* Function scoping */
#define PUBLIC 	extern
#define PRIVATE	static
#define RESTRICT
#define INLINE
#define ATTRIBUTE

#ifdef __GNUC__
/* Podemos usar inline y attributos */
#undef  INLINE
#define INLINE		__inline__
#undef  ATTR
#define ATTR(arg)	__attribute__(arg)
#endif


/* Getter/setter *//*
#define __GETTER(obj, prop, var) (*(var) = (obj)->(prop))
#define __SETTER(obj, prop, var) ((obj)->(prop) = *(var))
*/

/*! Macro para marcar argumentos no usados */
#define UNUSED(arg) ((void)(arg))

/*! Macro para proteger las demas macros */
#define MSMACRO(body) do body while(0)

/*===============================*/
/*        DEBUGGING              */
/*===============================*/
#ifdef DEBUGGING
#define TRACE(msg) \
	MSMACRO( \
		fprintf(stderr, "trace: %s:%i: %s\n", __FILE__, __LINE__, (msg)); \
	)
#define TRACEF(msg, fmt) \
	MSMACRO( \
		printf(stderr, "trace: %s:%i: ", __FILE__, __LINE__); \
		tracef_printf fmt; putc('\n', stderr); \
	)

#else
#define TRACE(m)   	/* DISABLED */
#define TRACEF(m,f)	/* DISABLED */
#endif

#endif /* GLOBAL_H */
