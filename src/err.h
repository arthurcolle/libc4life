#ifndef C4ERR_H
#define C4ERR_H

#include <stdbool.h>
#include <stdio.h>
#include "seqs/ls.h"
#include "macros.h"

#define _C4CATCH(var, type, _next)					\
  for (struct c4err *var = c4err_first(type),				\
	 *_next = var ? c4err_next(&var->errs_node, type) : NULL;	\
       var != NULL;							\
       var = _next,							\
	 _next = var ? c4err_next(&var->errs_node, type) : NULL)	\

#define C4CATCH(var, type)			\
  _C4CATCH(var, type, C4GSYM(next))		\

#define _C4THROW(type, msg)					\
  ({								\
    struct c4err *e = malloc(sizeof(struct c4err));		\
    c4err_init(e, c4ctx()->try, type, msg, __FILE__, __LINE__);	\
  })								\

#define C4THROW(type, msg)			\
  _C4THROW(type, (msg))				\
  
#define _C4TRY(msg, _try)						\
  for (struct c4try *_try = c4try_init(malloc(sizeof(struct c4try)),	\
				       msg, __FILE__, __LINE__);	\
       _try != NULL;							\
       c4try_close(_try), _try = NULL)					\
    
#define C4TRY(msg)				\
  _C4TRY((msg), C4GSYM(try))			\

struct c4try {
  char *msg;
  const char *file;
  int line, refs;
  struct c4ls errs;
  struct c4try *super;
};

struct c4try *c4try_init(struct c4try *self,
			 const char *msg,
			 const char *file, int line);

void c4try_free(struct c4try *self);
void c4try_close(struct c4try *self);
void c4try_ref(struct c4try *self);

struct c4err_t {
  char *name;
  struct c4err_t *super;
  struct c4ls ts_node;
};

struct c4ls *c4err_ts();

struct c4err_t *c4err_t_init(struct c4err_t *self,
			     struct c4err_t *super,			     
			     const char *name);

void c4err_t_free(struct c4err_t *self);

struct c4err {
  char *msg;
  const char *file;
  int line;
  struct c4err_t *type;
  struct c4ls errs_node;
  struct c4try *try;
  void *data;
};

struct c4err *c4err_first(struct c4err_t *type);
struct c4err *c4err_next(struct c4ls *start, struct c4err_t *type);

struct c4err *c4err_init(struct c4err *self,
			 struct c4try *try,
			 struct c4err_t *type,
			 const char *msg,
			 const char *file, int line);

void c4err_free(struct c4err *self);
bool c4err_isa(struct c4err *self, struct c4err_t *type);
void c4err_print(struct c4err *self, FILE *out);

#endif
