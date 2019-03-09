#ifndef ns_h
#define ns_h

#include "wren.h"

#if __cplusplus
extern "C" {
#endif
	
	void WREN_ns_say(WrenVM *);
// void exampleStr(WrenVM *);

	void WREN_ns_save_fiber(WrenVM *);
    void WREN_ns_resume();
        
        
        
        
        
         void WREN_ns_leave(WrenVM *vm) ;

 void WREN_ns_bg(WrenVM *vm);

 void WREN_ns_fg(WrenVM *vm) ;

 void WREN_ns_jump(WrenVM *vm);

 void WREN_ns_bgm(WrenVM *vm) ;
 void WREN_ns_stopbgm(WrenVM *vm);
 void WREN_ns_sound(WrenVM *vm);
 void WREN_ns_stopsound(WrenVM *vm) ;
 void WREN_ns_gameover(WrenVM *vm) ;
 void WREN_ns_set(WrenVM *vm) ;
 void WREN_ns_select(WrenVM *vm) ;
 void WREN_ns_select_add_option(WrenVM *vm) ;
 void WREN_ns_select_exec(WrenVM *vm) ;

 void WREN_ns_print(WrenVM *vm) ;
 void WREN_ns_clear(WrenVM *vm) ;
 void WREN_ns_leaveimg(WrenVM *vm) ;
 void WREN_ns_imgtween(WrenVM *vm);
 void WREN_ns_img(WrenVM *vm);
        

 void WREN_ns__s(WrenVM *vm);
 void WREN_ns__ss(WrenVM *vm);
 void WREN_ns__sss(WrenVM *vm);
 void WREN_ns__v(WrenVM *vm);


#if __cplusplus
};
#endif

#endif
