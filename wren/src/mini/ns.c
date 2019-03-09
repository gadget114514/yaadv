#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "vm.h"
#include "wren.h"

#include <stdio.h>

#include "ns.h"

struct WrenVM* ccvm;
static WrenHandle *ccfiber;

// Called by libuv when the timer finished closing.
static void close(void* handle)
{
  free(handle);
}

// Called by user event library when the timer has completed.
void WREN_ns_resume()
{
  if (ccfiber == 0) return;
  WrenHandle* fiber = wrenCopyHandle(ccvm, ccfiber);
  schedulerResume(fiber, false);
}

void WREN_ns_save_fiber(WrenVM* vm)
{
  ccvm = vm;
  WrenHandle* fiber = wrenGetSlotHandle(vm, 1);
 ccfiber = fiber;
}
