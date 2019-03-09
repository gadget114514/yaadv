#ifndef wren_opt_random_h
#define wren_opt_random_h
#define WREN_OPT_RANDOM 1
#include "wren_common.h"
#include "wren.h"


#if WREN_OPT_RANDOM

#ifdef __cplusplus
extern "C" {
#endif

const char* wrenRandomSource();
WrenForeignClassMethods wrenRandomBindForeignClass(WrenVM* vm,
                                                   const char* module,
                                                   const char* className);
WrenForeignMethodFn wrenRandomBindForeignMethod(WrenVM* vm,
                                                const char* className,
                                                bool isStatic,
                                                const char* signature);

#ifdef __cplusplus
}
#endif
#endif

#endif
