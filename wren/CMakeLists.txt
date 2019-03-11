
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/vm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/optional)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(WREN_SRC
src/optional/wren_opt_meta.c
src/optional/wren_opt_random.c
src/vm/wren_core.c
src/vm/wren_compiler.c
src/vm/wren_vm.c
src/vm/wren_value.c
src/vm/wren_primitive.c
src/vm/wren_debug.c
src/vm/wren_utils.c
src/mini/vm.c
src/mini/path.c
src/mini/userev.c
src/mini/ns.c
src/mini/modules.c
src/mini/timer.c
src/mini/main.c
src/mini/os.c
src/mini/scheduler.c
src/mini/io.c
src/mini/example.c
  )


add_library(libwren STATIC ${WREN_SRC})
set_target_properties(libwren PROPERTIES OUTPUT_NAME "libwren")

target_include_directories(libwren PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
