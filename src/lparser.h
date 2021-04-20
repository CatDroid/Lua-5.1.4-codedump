/*
** $Id: lparser.h,v 1.57.1.1 2007/12/27 13:02:25 roberto Exp $
** Lua Parser
** See Copyright Notice in lua.h
*/

#ifndef lparser_h
#define lparser_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/*
** Expression descriptor
*/

typedef enum {
  VVOID,	/* no value */
  VNIL,
  VTRUE,
  VFALSE,
  VK,		/* info = index of constant in `k' */
  VKNUM,	/* nval = numerical value 数字字面量 */
  VLOCAL,	/* info = local register */
  VUPVAL,       /* info = index of upvalue in `upvalues' */
  VGLOBAL,	/* info = index of table; aux = index of global name in `k' */
  VINDEXED,	/* info = table register; aux = index register (or `k') */
  VJMP,		/* info = instruction pc */
  VRELOCABLE,	/* info = instruction pc */ // 指令的地址 ??
  VNONRELOC,	/* info = result register */
  VCALL,	/* info = instruction pc */
  VVARARG	/* info = instruction pc */
} expkind;

// 存放表达式的数据结构
typedef struct expdesc {
  // 表达式类型        直接是=9 就是VKNUM       u.nval 存放这个值
  expkind k;
  // 保存表达式信息的联合体
  union {
    struct { // info 和aux里面存储的数据信息，根据不同的表达式类型有区分，见前面expkind的注释
    	int info,
    	aux; } s;
    lua_Number nval;  // 类型为数字的时候存储具体的数据
  } u;
  int t;  /* patch list of `exit when true' */
  int f;  /* patch list of `exit when false' */
} expdesc;


typedef struct upvaldesc {
  lu_byte k;
  lu_byte info;
} upvaldesc;


struct BlockCnt;  /* defined in lparser.c */


/* state needed to generate code for a given function */
typedef struct FuncState {
  Proto *f;  /* current function header */
  
  Table *h;  /* table to find (and reuse) elements in `k' */  
  
  // 以常量作为key value是proto.k中的索引
  // fs->h 目的就是 通过另外一个table来判断是否有proto.k已经有同样的
  
  struct FuncState *prev;  /* enclosing function */ 
  // 从大范围到小范围 全局函数 局部函数 局部函数中的局部函数
  
  struct LexState *ls;  /* lexical state */
  struct lua_State *L;  /* copy of the Lua state */
  struct BlockCnt *bl;  /* chain of current blocks */
  int pc;  /* next position to code (equivalent to `ncode') */
  int lasttarget;   /* `pc' of last `jump target' */
  // 这里存放的是所有空悬,也就是没有确定好跳转位置的pc链表
  int jpc;  /* list of pending jumps to `pc' */
  
  int freereg;  /* first free register */  // 下一个可用寄存器的位置 局部变量、函数参数
  
  int nk;  /* number of elements in `k' */
  int np;  /* number of elements in `p' */
  
  short nlocvars;  /* number of elements in `locvars' */ 
  
  lu_byte nactvar;  /* number of active local variables */
  
  upvaldesc upvalues[LUAI_MAXUPVALUES];  /* upvalues */
  
  unsigned short actvar[LUAI_MAXVARS];  /* declared-variable stack */ // 记录了局部变量在 proto.locvars数组的位置
} FuncState;


LUAI_FUNC Proto *luaY_parser (lua_State *L, ZIO *z, Mbuffer *buff,
                                            const char *name);


#endif
