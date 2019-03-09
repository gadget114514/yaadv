#ifndef wren_opt_meta_h
#define wren_opt_meta_h
#define WREN_OPT_META 1
#include "wren_common.h"
#include "wren.h"


// This module defines the Meta class and its associated methods.
#if WREN_OPT_META
#ifdef __cplusplus
extern "C" {
#endif

const char* wrenMetaSource();
WrenForeignMethodFn wrenMetaBindForeignMethod(WrenVM* vm,
                                              const char* className,
                                              bool isStatic,
                                              const char* signature);

#ifdef __cplusplus
}
#endif
#endif

#endif
