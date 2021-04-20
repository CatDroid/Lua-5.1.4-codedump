/*
** $Id: lmem.c,v 1.70.1.1 2007/12/27 13:02:25 roberto Exp $
** Interface to Memory Manager
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lmem_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



/*
** About the realloc function:
** void * frealloc (void *ud, void *ptr, size_t osize, size_t nsize);
** (`osize' is the old size, `nsize' is the new size)
**
** Lua ensures that (ptr == NULL) iff (osize == 0).
**
** * frealloc(ud, NULL, 0, x) creates a new block of size `x'
**
** * frealloc(ud, p, x, 0) frees the block `p'
** (in this specific case, frealloc must return NULL).
** particularly, frealloc(ud, NULL, 0, 0) does nothing
** (which is equivalent to free(NULL) in ANSI C)
**
** frealloc returns NULL if it cannot create or reallocate the area
** (any reallocation to an equal or smaller size cannot fail!)
*/



#define MINSIZEARRAY	4


void *luaM_growaux_ (lua_State *L, void *block, int *size, size_t size_elems,
                     int limit, const char *errormsg) {
  void *newblock;
  int newsize;
  if (*size >= limit/2) {  /* cannot double it? */
    if (*size >= limit)  /* cannot grow even a little? */ // 已经超过limit 
      luaG_runerror(L, errormsg);
    newsize = limit;  /* still have at least one free place */ // 如果两倍后会超过limit,那newsize就只能是limit
  }
  else {
    newsize = (*size)*2; // 两倍增加 但是最小不能少于4个
    if (newsize < MINSIZEARRAY)
      newsize = MINSIZEARRAY;  /* minimum size */
  }

  // 每个元素的大小是size_elemts 原来数目是*size 现在数目是newSize  
  // realloc原来block指向的内存  
  newblock = luaM_reallocv(L, block, *size, newsize, size_elems);
  
  *size = newsize;  /* update only when everything else is OK */
  return newblock;
}


void *luaM_toobig (lua_State *L) {
  luaG_runerror(L, "memory allocation error: block too big");
  return NULL;  /* to avoid warnings */
}



/*
** generic allocation routine.
*/
void *luaM_realloc_ (lua_State *L, void *block, size_t osize, size_t nsize) {
  global_State *g = G(L);
  lua_assert((osize == 0) == (block == NULL));
  block = (*g->frealloc)(g->ud, block, osize, nsize);
  if (block == NULL && nsize > 0)
    luaD_throw(L, LUA_ERRMEM);
  lua_assert((nsize == 0) == (block == NULL));
  g->totalbytes = (g->totalbytes - osize) + nsize;
  return block;
}

