#include <stdlib.h>
#include <string.h>

#include "modules.h"

#include "io.wren.inc"
#include "os.wren.inc"
#include "scheduler.wren.inc"
#include "timer.wren.inc"
#include "example.wren.inc"
#include "example.h"
#include "ns.wren.inc"
#include "ns.h"

extern void platformIsPosix(WrenVM* vm);
extern void platformName(WrenVM* vm);
extern void processAllArguments(WrenVM* vm);
extern void stdoutFlush(WrenVM* vm);

extern void schedulerCaptureMethods(WrenVM* vm);
extern void timerStartTimer(WrenVM* vm);

// The maximum number of foreign methods a single class defines. Ideally, we
// would use variable-length arrays for each class in the table below, but
// C++98 doesn't have any easy syntax for nested global static data, so we
// just use worst-case fixed-size arrays instead.
//
// If you add a new method to the longest class below, make sure to bump this.
// Note that it also includes an extra slot for the sentinel value indicating
// the end of the list.
#define MAX_METHODS_PER_CLASS 29

// The maximum number of foreign classes a single built-in module defines.
//
// If you add a new class to the largest module below, make sure to bump this.
// Note that it also includes an extra slot for the sentinel value indicating
// the end of the list.
#define MAX_CLASSES_PER_MODULE 6

// Describes one foreign method in a class.
typedef struct
{
  bool isStatic;
  const char* signature;
  WrenForeignMethodFn method;
} MethodRegistry;

// Describes one class in a built-in module.
typedef struct
{
  const char* name;

  MethodRegistry methods[MAX_METHODS_PER_CLASS];
} ClassRegistry;

// Describes one built-in module.
typedef struct
{
  // The name of the module.
  const char* name;

  // Pointer to the string containing the source code of the module. We use a
  // pointer here because the string variable itself is not a constant
  // expression so can't be used in the initializer below.
  const char **source;

  ClassRegistry classes[MAX_CLASSES_PER_MODULE];
} ModuleRegistry;

// To locate foreign classes and modules, we build a big directory for them in
// static data. The nested collection initializer syntax gets pretty noisy, so
// define a couple of macros to make it easier.
#define SENTINEL_METHOD { false, NULL, NULL }
#define SENTINEL_CLASS { NULL, { SENTINEL_METHOD } }
#define SENTINEL_MODULE {NULL, NULL, { SENTINEL_CLASS } }

#define MODULE(name) { #name, &name##ModuleSource, {
#define END_MODULE SENTINEL_CLASS } },

#define CLASS(name) { #name, {
#define END_CLASS SENTINEL_METHOD } },

#define METHOD(signature, fn) { false, signature, fn },
#define STATIC_METHOD(signature, fn) { true, signature, fn },
#define FINALIZER(fn) { true, "<finalize>", (WrenForeignMethodFn)fn },

// The array of built-in modules.
static ModuleRegistry modules[] =
{
  MODULE(io)
    CLASS(Stdout)
      STATIC_METHOD("flush()", stdoutFlush)
    END_CLASS
  END_MODULE
  MODULE(os)
    CLASS(Platform)
      STATIC_METHOD("isPosix", platformIsPosix)
      STATIC_METHOD("name", platformName)
    END_CLASS
    CLASS(Process)
      STATIC_METHOD("allArguments", processAllArguments)
    END_CLASS
  END_MODULE
  MODULE(scheduler)
    CLASS(Scheduler)
      STATIC_METHOD("captureMethods_()", schedulerCaptureMethods)
    END_CLASS
  END_MODULE
  MODULE(timer)
    CLASS(Timer)
      STATIC_METHOD("startTimer_(_,_)", timerStartTimer)
    END_CLASS
  END_MODULE
  MODULE(example)
    CLASS(Example) 
      STATIC_METHOD("exampleAdd(_,_)", exampleAdd)
      STATIC_METHOD("exampleStr(_,_)", exampleStr)
    END_CLASS
  END_MODULE
  
  MODULE(ns)
    CLASS(Ns) 
      STATIC_METHOD("say_(_,_,_,_)", WREN_ns_say)
      STATIC_METHOD("saveFiber(_)", WREN_ns_save_fiber)
//      STATIC_METHOD("exampleStr(_,_)", exampleStr)

	STATIC_METHOD("leave(_)", WREN_ns_leave)
	STATIC_METHOD("fg(_)", WREN_ns_fg)
	STATIC_METHOD("bg(_,_)", WREN_ns_bg)
	STATIC_METHOD("jump_(_)", WREN_ns_jump)
	STATIC_METHOD("bgm(_)", WREN_ns_bgm)
	STATIC_METHOD("stopbgm()", WREN_ns_stopbgm)
	STATIC_METHOD("sound(_)", WREN_ns_sound)
	STATIC_METHOD("stopsound()", WREN_ns_stopsound)
	STATIC_METHOD("gameover()", WREN_ns_gameover)
	STATIC_METHOD("set(_,_,_)", WREN_ns_set)
	STATIC_METHOD("select(_)", WREN_ns_select)
    
	STATIC_METHOD("selectoption(_,_)", WREN_ns_select_add_option)
 
	STATIC_METHOD("selectshow_(_)", WREN_ns_select_exec)
	STATIC_METHOD("clear(_)", WREN_ns_clear)
	STATIC_METHOD("print(_)", WREN_ns_print)
	STATIC_METHOD("tween(_,_,_,_)", WREN_ns_imgtween)
	STATIC_METHOD("img(_,_,_)", WREN_ns_img)
	STATIC_METHOD("leaveimg(_)", WREN_ns_leaveimg)

	STATIC_METHOD("s_(_,_)", WREN_ns__s)
    STATIC_METHOD("ss_(_,_,_)", WREN_ns__ss)
    STATIC_METHOD("sss_(_,_,_,_)",  WREN_ns__sss)
    STATIC_METHOD("v_(_)",  WREN_ns__v)
	
    END_CLASS
  END_MODULE
  
  SENTINEL_MODULE
};

#undef SENTINEL_METHOD
#undef SENTINEL_CLASS
#undef SENTINEL_MODULE
#undef MODULE
#undef END_MODULE
#undef CLASS
#undef END_CLASS
#undef METHOD
#undef STATIC_METHOD
#undef FINALIZER

// Looks for a built-in module with [name].
//
// Returns the BuildInModule for it or NULL if not found.
static ModuleRegistry* findModule(const char* name)
{
  for (int i = 0; modules[i].name != NULL; i++)
  {
    if (strcmp(name, modules[i].name) == 0) return &modules[i];
  }

  return NULL;
}

// Looks for a class with [name] in [module].
static ClassRegistry* findClass(ModuleRegistry* module, const char* name)
{
  for (int i = 0; module->classes[i].name != NULL; i++)
  {
    if (strcmp(name, module->classes[i].name) == 0) return &module->classes[i];
  }

  return NULL;
}

// Looks for a method with [signature] in [clas].
static WrenForeignMethodFn findMethod(ClassRegistry* clas,
                                      bool isStatic, const char* signature)
{
  for (int i = 0; clas->methods[i].signature != NULL; i++)
  {
    MethodRegistry* method = &clas->methods[i];
    if (isStatic == method->isStatic &&
        strcmp(signature, method->signature) == 0)
    {
      return method->method;
    }
  }

  return NULL;
}

char* readBuiltInModule(const char* name)
{
  ModuleRegistry* module = findModule(name);
  if (module == NULL) return NULL;

  size_t length = strlen(*module->source);
  char* copy = (char*)malloc(length + 1);
  memcpy(copy, *module->source, length + 1);
  return copy;
}

WrenForeignMethodFn bindBuiltInForeignMethod(
    WrenVM* vm, const char* moduleName, const char* className, bool isStatic,
    const char* signature)
{
  // TODO: Assert instead of return NULL?
  ModuleRegistry* module = findModule(moduleName);
  if (module == NULL) return NULL;

  ClassRegistry* clas = findClass(module, className);
  if (clas == NULL) return NULL;

  return findMethod(clas, isStatic, signature);
}

WrenForeignClassMethods bindBuiltInForeignClass(
    WrenVM* vm, const char* moduleName, const char* className)
{
  WrenForeignClassMethods methods = { NULL, NULL };

  ModuleRegistry* module = findModule(moduleName);
  if (module == NULL) return methods;

  ClassRegistry* clas = findClass(module, className);
  if (clas == NULL) return methods;

  methods.allocate = findMethod(clas, true, "<allocate>");
  methods.finalize = (WrenFinalizerFn)findMethod(clas, true, "<finalize>");

  return methods;
}
